/*
** Copyright 2018 Bloomberg Finance L.P.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
// NOTE: DO NOT INCLUDE DIRECTLY

// ##############################################################################################
// #################################### IMPLEMENTATIONS #########################################
// ##############################################################################################

#include <limits>

namespace Bloomberg::quantum
{
        inline TaskQueue::WorkItem::WorkItem(
            TaskPtr task, TaskListIter iter, bool isBlocked, unsigned int blockedQueueRound)
            : _task(task)
            , _iter(iter)
            , _isBlocked(isBlocked)
            , _blockedQueueRound(blockedQueueRound)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline TaskQueue::ProcessTaskResult::ProcessTaskResult(bool isBlocked, unsigned int blockedQueueRound)
            : _isBlocked(isBlocked)
            , _blockedQueueRound(blockedQueueRound)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline TaskQueue::TaskQueue()
            : TaskQueue(Configuration(), nullptr)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline TaskQueue::TaskQueue(const Configuration & configuration, std::shared_ptr<TaskQueue> sharedQueue)
            : m_alloc(Allocator<QueueListAllocator>::instance(AllocatorTraits::queueListAllocSize()))
            , m_runQueue(m_alloc)
            , m_waitQueue(m_alloc)
            , m_queueIt(m_runQueue.end())
            , m_blockedIt(m_runQueue.end())
            , m_isBlocked(false)
            , m_isEmpty(true)
            , m_isSharedQueueEmpty(true)
            , m_isInterrupted(false)
            , m_isIdle(true)
            , m_terminated(false)
            , m_isAdvanced(false)
            , m_sharedQueue(sharedQueue)
            , m_queueRound(0)
            , m_lastSleptQueueRound(std::numeric_limits<unsigned int>::max())
            , m_lastSleptSharedQueueRound(std::numeric_limits<unsigned int>::max())
            , m_taskStateConfiguration(configuration.getTaskStateConfiguration())
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            TaskStateHandler taskStateHandler;
            if (isIntersection(m_taskStateConfiguration.getHandledTaskTypes(), TaskType::Coroutine))
            {
                taskStateHandler = makeExceptionSafe(m_taskStateConfiguration.getTaskStateHandler());
            }
            m_taskStateConfiguration.setTaskStateHandler(taskStateHandler);
            
            if (m_sharedQueue)
            {
                m_sharedQueue->m_helpers.push_back(this);
            }
            
            m_thread = std::make_shared<std::thread>(std::bind(&TaskQueue::run, this));
        }

        inline TaskQueue::TaskQueue(const TaskQueue &)
            : TaskQueue()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline TaskQueue::~TaskQueue()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            terminate();
        }

        inline void TaskQueue::pinToCore(int coreId)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
#if defined(_WIN32) && !defined(__CYGWIN__)
            SetThreadAffinityMask(_thread->native_handle(), 1 << coreId);
#else
            int cpuSetSize = sizeof(cpu_set_t);
            if (coreId >= 0 && (coreId <= cpuSetSize * 8))
            {
                cpu_set_t cpuSet;
                CPU_ZERO(&cpuSet);
                CPU_SET(coreId, &cpuSet);
                pthread_setaffinity_np(m_thread->native_handle(), cpuSetSize, &cpuSet);
            }
#endif
        }

        inline void TaskQueue::run()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            while (!isInterrupted())
            {
                const ProcessTaskResult result = processTask();
                if (m_sharedQueue)
                {
                    const ProcessTaskResult sharedResult = m_sharedQueue->processTask();
                    sleepOnBlockedQueue(result, sharedResult);
                }
                else
                {
                    sleepOnBlockedQueue(result);
                }
            }
            LOG4CXX_INFO(logger, "Clear remaining tasks");
            // Clear remaining tasks
            while (!m_runQueue.empty())
            {
                m_runQueue.front()->terminate();
                m_runQueue.pop_front();
                m_stats.decNumElements();
            }
            //========================= LOCKED SCOPE =========================
            SpinLock::Guard lock(m_waitQueueLock);
            while (!m_waitQueue.empty())
            {
                m_waitQueue.front()->terminate();
                m_waitQueue.pop_front();
                m_stats.decNumElements();
            }
            m_isIdle = true;
        }

        inline void TaskQueue::sleepOnBlockedQueue(const ProcessTaskResult & mainQueueResult)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (mainQueueResult._isBlocked && mainQueueResult._blockedQueueRound != m_lastSleptQueueRound)
            {
                m_lastSleptQueueRound = mainQueueResult._blockedQueueRound;
                YieldingThread()();
            }
        }

        inline void TaskQueue::sleepOnBlockedQueue(
            const ProcessTaskResult & mainQueueResult, const ProcessTaskResult & sharedQueueResult)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            const bool allQueuesEmpty = m_isEmpty && m_isSharedQueueEmpty;
            if (allQueuesEmpty)
            {
                LOG4CXX_WARN(logger, "if both queues are blocked, the thread will be waiting on _notEmptyCond");
                // if both queues are blocked, the thread will be waiting on _notEmptyCond
                return;
            }

            const bool isQueueBlocked =
                mainQueueResult._isBlocked && mainQueueResult._blockedQueueRound != m_lastSleptQueueRound;
            const bool isSharedQueueBlocked =
                sharedQueueResult._isBlocked && sharedQueueResult._blockedQueueRound != m_lastSleptSharedQueueRound;
            
            if ((isQueueBlocked || m_isEmpty) && (isSharedQueueBlocked || m_isSharedQueueEmpty))
            {
                m_lastSleptQueueRound       = mainQueueResult._blockedQueueRound;
                m_lastSleptSharedQueueRound = sharedQueueResult._blockedQueueRound;
                YieldingThread()();
            }
        }

        inline TaskQueue::ProcessTaskResult TaskQueue::processTask()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            WorkItem workItem {nullptr, m_runQueue.end(), false, 0};
            try
            {
                // Process a task
                workItem = grabWorkItem();

                TaskPtr task = workItem._task;
                if (!task)
                {
                    return ProcessTaskResult(workItem._isBlocked, workItem._blockedQueueRound);
                }

                int rc = 0;
                {
                    LOG4CXX_DEBUG(logger, "set the current task for local-storage queries");
                    // set the current task for local-storage queries
                    IQueue::TaskSetterGuard taskSetter(*this, task);
                    //========================= START/RESUME COROUTINE =========================
                    rc = task->run(
                        m_taskStateConfiguration.getTaskStateHandler(), TaskType::Coroutine,
                        m_taskStateConfiguration.getHandledTaskStates());
                    //=========================== END/YIELD COROUTINE ==========================
                }
                switch (rc)
                {
                case static_cast<int>(ITask::RetCode::NotCallable):
                    handleNotCallable(workItem);
                    break;
                case static_cast<int>(ITask::RetCode::AlreadyResumed):
                    handleAlreadyResumed(workItem);
                    break;
                case static_cast<int>(ITask::RetCode::Blocked):
                    handleBlocked(workItem);
                    break;
                case static_cast<int>(ITask::RetCode::Sleeping):
                    handleSleeping(workItem);
                    break;
                case static_cast<int>(ITask::RetCode::Running):
                    handleRunning(workItem);
                    break;
                case static_cast<int>(ITask::RetCode::Success):
                    handleSuccess(workItem);
                    break;
                default:
                    handleError(workItem);
                    break;
                }
            }
            catch (const std::exception & ex)
            {
                handleException(workItem, &ex);
            }
            catch (...)
            {
                handleException(workItem);
            }
            return ProcessTaskResult(workItem._isBlocked, workItem._blockedQueueRound);
        }

        inline void TaskQueue::enqueue(ITask::Ptr task)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!task)
            {
                LOG4CXX_DEBUG(logger, "nothing to do.");
                return; // nothing to do
            }
            //========================= LOCKED SCOPE =========================
            SpinLock::Guard lock(m_waitQueueLock);
            doEnqueue(task);
        }

        inline bool TaskQueue::tryEnqueue(ITask::Ptr task)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!task)
            {
                LOG4CXX_DEBUG(logger, "nothing to do.");
                return false; // nothing to do
            }
            //========================= LOCKED SCOPE =========================
            SpinLock::Guard lock(m_waitQueueLock, lock::tryToLock);
            if (lock.ownsLock())
            {
                doEnqueue(task);
            }
            return lock.ownsLock();
        }

        inline void TaskQueue::doEnqueue(ITask::Ptr task)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // NOTE: _queueIt remains unchanged following this operation
            m_stats.incPostedCount();
            m_stats.incNumElements();
            bool isEmpty = m_waitQueue.empty();
            if (task->isHighPriority())
            {
                LOG4CXX_DEBUG(logger, "Task will be at the head of the queue.");
                // insert before the current position. If _queueIt == begin(), then the new
                // task will be at the head of the queue.
                m_waitQueue.emplace_front(std::static_pointer_cast<Task>(task));
            }
            else
            {
                LOG4CXX_DEBUG(logger, "Task will be the last element in the queue.");
                // insert after the current position. If next(_queueIt) == end()
                // then the new task will be the last element in the queue
                m_waitQueue.emplace_back(std::static_pointer_cast<Task>(task));
            }
            if (task->isHighPriority())
            {
                m_stats.incHighPriorityCount();
            }
            if (isEmpty)
            {
                LOG4CXX_WARN(logger, "Signal on transition from 0 to 1 element only.");
                // signal on transition from 0 to 1 element only
                signalEmptyCondition(false);
            }
        }

        inline ITask::Ptr TaskQueue::dequeue(std::atomic_bool & hint)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return doDequeue(hint, m_queueIt);
        }

        inline ITask::Ptr TaskQueue::tryDequeue(std::atomic_bool & hint)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return doDequeue(hint, m_queueIt);
        }

        inline ITask::Ptr TaskQueue::doDequeue(std::atomic_bool &, TaskListIter iter)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            //========================= LOCKED SCOPE =========================
            SpinLock::Guard lock(m_runQueueLock);
            if (iter == m_runQueue.end())
            {
                return nullptr;
            }
            if (iter == m_blockedIt)
            {
                LOG4CXX_DEBUG(logger, "We don't really know what's the next blocked task in the queue, so reset it.");
                // we don't really know what's the next blocked task in the queue, so reset it
                m_blockedIt = m_runQueue.end();
            }
            ITask::Ptr task = *iter;

            task->terminate();
            if (m_queueIt == iter)
            {
                m_queueIt    = m_runQueue.erase(iter);
                m_isAdvanced = true;
            }
            else
            {
                m_runQueue.erase(iter);
            }
            m_stats.decNumElements();
            return task;
        }

        inline size_t TaskQueue::size() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_isIdle ? m_stats.numElements() : m_stats.numElements() + 1;
        }

        inline bool TaskQueue::empty() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return size() == 0;
        }

        inline void TaskQueue::terminate()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            bool value {false};
            if (m_terminated.compare_exchange_strong(value, true))
            {
                {
                    std::unique_lock<std::mutex> lock(m_notEmptyMutex);
                    m_isInterrupted = true;
                }
                m_notEmptyCond.notify_all();
                m_thread->join();
            }
        }

        inline IQueueStatistics & TaskQueue::stats()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_stats;
        }

        inline SpinLock & TaskQueue::getLock()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_waitQueueLock;
        }

        inline void TaskQueue::signalEmptyCondition(bool value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            {
                //========================= LOCKED SCOPE =========================
                std::lock_guard<std::mutex> lock(m_notEmptyMutex);
                m_isEmpty = value;
            }
            if (!value)
            {
                m_notEmptyCond.notify_all();
            }
            LOG4CXX_DEBUG(logger, "Notify helpers as well.");
            // Notify helpers as well
            for (TaskQueue * helper : m_helpers)
            {
                helper->signalSharedQueueEmptyCondition(value);
            }
        }

        inline void TaskQueue::signalSharedQueueEmptyCondition(bool value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            {
                //========================= LOCKED SCOPE =========================
                std::lock_guard<std::mutex> lock(m_notEmptyMutex);
                m_isSharedQueueEmpty = value;
            }
            if (!value)
            {
                m_notEmptyCond.notify_all();
            }
        }

        inline bool TaskQueue::handleNotCallable(const WorkItem & workItem)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return handleError(workItem);
        }

        inline bool TaskQueue::handleAlreadyResumed(WorkItem & entry)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            //========================= LOCKED SCOPE =========================
            SpinLock::Guard lock(m_runQueueLock);
            onBlockedTask(entry);
            return false;
        }

        inline bool TaskQueue::handleBlocked(WorkItem & entry)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            //========================= LOCKED SCOPE =========================
            SpinLock::Guard lock(m_runQueueLock);
            onBlockedTask(entry);
            return false;
        }

        inline bool TaskQueue::handleSleeping(WorkItem & entry)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            //========================= LOCKED SCOPE =========================
            SpinLock::Guard lock(m_runQueueLock);
            onBlockedTask(entry);
            return false;
        }

        inline bool TaskQueue::handleRunning(WorkItem & entry)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            //========================= LOCKED SCOPE =========================
            SpinLock::Guard lock(m_runQueueLock);
            onActiveTask(entry);
            return true;
        }

        inline bool TaskQueue::handleSuccess(const WorkItem & workItem)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            ITaskContinuation::Ptr nextTask;
            // check if there's another task scheduled to run after this one
            nextTask = workItem._task->getNextTask();
            if (nextTask && (nextTask->getType() == ITask::Type::ErrorHandler))
            {
                // skip error handler since we don't have any errors
                nextTask->terminate(); // invalidate the error handler
                nextTask = nextTask->getNextTask();
            }
            // queue next task and de-queue current one
            enqueue(nextTask);
            doDequeue(m_isIdle, workItem._iter);
            // Coroutine ended normally with "return 0" statement
            m_stats.incCompletedCount();
            return true;
        }

        inline bool TaskQueue::handleError(const WorkItem & workItem)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            ITaskContinuation::Ptr nextTask;
            // Check if we have a final task to run
            nextTask = workItem._task->getErrorHandlerOrFinalTask();
            // queue next task and de-queue current one
            enqueue(nextTask);
            doDequeue(m_isIdle, workItem._iter);
            // Coroutine ended with explicit user error
            m_stats.incErrorCount();
#ifdef __QUANTUM_PRINT_DEBUG
            std::lock_guard<std::mutex> guard(Util::LogMutex());
            if (rc == (int)ITask::RetCode::Exception)
            {
                std::cerr << "Coroutine exited with user exception." << std::endl;
            }
            else
            {
                std::cerr << "Coroutine exited with error : " << rc << std::endl;
            }
#endif
            return false;
        }

        inline bool TaskQueue::handleException(const WorkItem & workItem, [[maybe_unused]] const std::exception * ex)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
//            UNUSED(ex);
            doDequeue(m_isIdle, workItem._iter);
#ifdef __QUANTUM_PRINT_DEBUG
            std::lock_guard<std::mutex> guard(Util::LogMutex());
            if (ex != nullptr)
            {
                std::cerr << "Caught exception: " << ex.what() << std::endl;
            }
            else
            {
                std::cerr << "Caught unknown exception." << std::endl;
            }
#endif
            return false;
        }

        inline bool TaskQueue::isInterrupted()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_isEmpty && m_isSharedQueueEmpty)
            {
                std::unique_lock<std::mutex> lock(m_notEmptyMutex);
                //========================= BLOCK WHEN EMPTY =========================
                // Wait for the queue to have at least one element
                m_notEmptyCond.wait(
                    lock, [this]() -> bool { return !m_isEmpty || !m_isSharedQueueEmpty || m_isInterrupted; });
            }
            return m_isInterrupted;
        }

        inline TaskQueue::WorkItem TaskQueue::grabWorkItem()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            //========================= LOCKED SCOPE =========================
            SpinLock::Guard lock(m_runQueueLock);
            if ((m_queueIt == m_runQueue.end()) || (!m_isAdvanced && (++m_queueIt == m_runQueue.end())))
            {
                acquireWaiting();
            }
            m_isAdvanced = false; // reset flag
            m_isIdle     = m_runQueue.empty();
            if (m_runQueue.empty())
            {
                return WorkItem(nullptr, m_runQueue.end(), m_isBlocked, m_queueRound);
            }
            return WorkItem((*m_queueIt), m_queueIt, false, 0);
        }

        inline void TaskQueue::onBlockedTask(WorkItem & entry)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_blockedIt == m_runQueue.end())
            {
                m_blockedIt = entry._iter;
            }
            else if (m_blockedIt == entry._iter)
            {
                m_isBlocked = true;
                m_blockedIt = m_runQueue.end();
            }
            entry._isBlocked         = m_isBlocked;
            entry._blockedQueueRound = m_queueRound;
        }

        inline void TaskQueue::onActiveTask(WorkItem & entry)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_isBlocked               = false;
            m_blockedIt               = m_runQueue.end();
            entry._isBlocked         = m_isBlocked;
            entry._blockedQueueRound = m_queueRound;
        }

        inline bool TaskQueue::isIdle() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_isIdle;
        }

        inline const std::shared_ptr<std::thread> & TaskQueue::getThread() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_thread;
        }

        inline void TaskQueue::acquireWaiting()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            //========================= LOCKED SCOPE =========================
            SpinLock::Guard lock(m_waitQueueLock);
            bool isEmpty = m_runQueue.empty();
            if (m_waitQueue.empty())
            {
                if (isEmpty)
                {
                    signalEmptyCondition(true);
                }
                m_queueIt = m_runQueue.begin();
                ++m_queueRound;
                return;
            }
            if (!isEmpty)
            {
                LOG4CXX_INFO(logger, "rewind by one since we are at end()");
                // rewind by one since we are at end()
                --m_queueIt;
            }
            {
                LOG4CXX_INFO(logger, "splice wait queue unto run queue.");
                // splice wait queue unto run queue.
                m_runQueue.splice(m_runQueue.end(), m_waitQueue);
            }
            if (!isEmpty)
            {
                LOG4CXX_INFO(logger, "move to first element from spliced queue.");
                // move to first element from spliced queue
                ++m_queueIt;
            }
            else
            {
                m_queueIt = m_runQueue.begin();
                ++m_queueRound;
            }
        }
}
