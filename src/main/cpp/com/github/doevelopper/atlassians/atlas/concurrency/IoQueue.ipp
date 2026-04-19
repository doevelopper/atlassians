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
#include <cmath>

namespace Bloomberg::quantum
{
        inline IoQueue::IoQueue()
            : IoQueue(Configuration(), nullptr)
        {
        }

        inline IoQueue::IoQueue(const Configuration & config, std::vector<IoQueue> * sharedIoQueues)
            : m_sharedIoQueues(sharedIoQueues)
            , m_loadBalanceSharedIoQueues(config.getLoadBalanceSharedIoQueues())
            , m_loadBalancePollIntervalMs(config.getLoadBalancePollIntervalMs())
            , m_loadBalancePollIntervalBackoffPolicy(config.getLoadBalancePollIntervalBackoffPolicy())
            , m_loadBalancePollIntervalNumBackoffs(config.getLoadBalancePollIntervalNumBackoffs())
            , m_loadBalanceBackoffNum(0)
            , m_queue(Allocator<IoQueueListAllocator>::instance(AllocatorTraits::ioQueueListAllocSize()))
            , m_isEmpty(true)
            , m_isInterrupted(false)
            , m_isIdle(true)
            , m_terminated(false)
            , m_taskStateConfiguration(config.getTaskStateConfiguration())
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_sharedIoQueues)
            {
                // The shared queue doesn't have its own thread
                m_thread = std::make_shared<std::thread>(std::bind(&IoQueue::run, this));
            }

            TaskStateHandler taskStateHandler;
            if (isIntersection(m_taskStateConfiguration.getHandledTaskTypes(), TaskType::IoTask))
            {
                taskStateHandler = makeExceptionSafe(m_taskStateConfiguration.getTaskStateHandler());
            }
            m_taskStateConfiguration.setTaskStateHandler(taskStateHandler);
        }

        inline IoQueue::IoQueue(const IoQueue & other)
            : m_sharedIoQueues(other.m_sharedIoQueues)
            , m_loadBalanceSharedIoQueues(other.m_loadBalanceSharedIoQueues)
            , m_loadBalancePollIntervalMs(other.m_loadBalancePollIntervalMs)
            , m_loadBalancePollIntervalBackoffPolicy(other.m_loadBalancePollIntervalBackoffPolicy)
            , m_loadBalancePollIntervalNumBackoffs(other.m_loadBalancePollIntervalNumBackoffs)
            , m_loadBalanceBackoffNum(0)
            , m_queue(Allocator<IoQueueListAllocator>::instance(AllocatorTraits::ioQueueListAllocSize()))
            , m_isEmpty(true)
            , m_isInterrupted(false)
            , m_isIdle(true)
            , m_terminated(false)
            , m_taskStateConfiguration(other.m_taskStateConfiguration)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_sharedIoQueues)
            {
                // The shared queue doesn't have its own thread
                m_thread = std::make_shared<std::thread>(std::bind(&IoQueue::run, this));
            }
        }

        inline IoQueue::~IoQueue()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            terminate();
        }

        inline void IoQueue::pinToCore(int)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // Not used
        }

        inline void IoQueue::run()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            while (true)
            {
                try
                {
                    ITask::Ptr task;
                    if (m_loadBalanceSharedIoQueues)
                    {
                        do
                        {
                            task = grabWorkItemFromAll();
                            if (task)
                            {
                                m_loadBalanceBackoffNum = 0; // reset
                                break;
                            }
                            YieldingThread()(getBackoffInterval());
                        }
                        while (!m_isInterrupted);
                    }
                    else if (m_isEmpty)
                    {
                        std::unique_lock<std::mutex> lock(m_notEmptyMutex);
                        //========================= BLOCK WHEN EMPTY =========================
                        // Wait for the queue to have at least one element
                        m_notEmptyCond.wait(lock, [this]() -> bool { return !m_isEmpty || m_isInterrupted; });
                    }
                    
                    if (m_isInterrupted)
                    {
                        break;
                    }
                    
                    if (!m_loadBalanceSharedIoQueues)
                    {
                        // Iterate to the next runnable task
                        task = grabWorkItem();
                        if (!task)
                        {
                            continue;
                        }
                    }

                    // set the current task
                    IQueue::TaskSetterGuard taskSetter(*this, task);
                    //========================= START TASK =========================
                    int rc = task->run(
                        m_taskStateConfiguration.getTaskStateHandler(), TaskType::IoTask,
                        m_taskStateConfiguration.getHandledTaskStates());
                    //========================== END TASK ==========================

                    if (rc == static_cast<int>(ITask::RetCode::Success))
                    {
                        if (task->getQueueId() == static_cast<int>(IQueue::QueueId::Any))
                        {
                            m_stats.incSharedQueueCompletedCount();
                        }
                        else
                        {
                            m_stats.incCompletedCount();
                        }
                    }
                    else
                    {
                        // IO task ended with error
                        if (task->getQueueId() == static_cast<int>(IQueue::QueueId::Any))
                        {
                            m_stats.incSharedQueueErrorCount();
                        }
                        else
                        {
                            m_stats.incErrorCount();
                        }

#ifdef __QUANTUM_PRINT_DEBUG
                        std::lock_guard<std::mutex> guard(Util::LogMutex());
                        if (rc == (int)ITask::RetCode::Exception)
                        {
                            std::cerr << "IO task exited with user exception." << std::endl;
                        }
                        else
                        {
                            std::cerr << "IO task exited with error : " << rc << std::endl;
                        }
#endif
                    }
                }
                catch ([[maybe_unused]] std::exception & ex)
                {
//                    UNUSED(ex);
#ifdef __QUANTUM_PRINT_DEBUG
                    std::lock_guard<std::mutex> guard(Util::LogMutex());
                    std::cerr << "Caught exception: " << ex.what() << std::endl;
#endif
                }
                catch (...)
                {
#ifdef __QUANTUM_PRINT_DEBUG
                    std::lock_guard<std::mutex> guard(Util::LogMutex());
                    std::cerr << "Caught unknown exception." << std::endl;
#endif
                }
            } // while
        }

        inline void IoQueue::enqueue(ITask::Ptr task)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!task)
            {
                return; // nothing to do
            }
            //========================= LOCKED SCOPE =========================
            SpinLock::Guard lock(m_spinlock);
            doEnqueue(task);
        }

        inline bool IoQueue::tryEnqueue(ITask::Ptr task)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!task)
            {
                return false; // nothing to do
            }
            //========================= LOCKED SCOPE =========================
            SpinLock::Guard lock(m_spinlock, lock::tryToLock);
            if (lock.ownsLock())
            {
                doEnqueue(task);
            }
            return lock.ownsLock();
        }

        inline void IoQueue::doEnqueue(ITask::Ptr task)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            bool isEmpty = m_queue.empty();
            if (task->isHighPriority())
            {
                m_stats.incHighPriorityCount();
                m_queue.emplace_front(std::static_pointer_cast<IoTask>(task));
            }
            else
            {
                m_queue.emplace_back(std::static_pointer_cast<IoTask>(task));
            }
            m_stats.incPostedCount();
            m_stats.incNumElements();
            if (!m_loadBalanceSharedIoQueues && isEmpty)
            {
                // signal on transition from 0 to 1 element only
                signalEmptyCondition(false);
            }
        }

        inline ITask::Ptr IoQueue::dequeue(std::atomic_bool & hint)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_loadBalanceSharedIoQueues)
            {
                //========================= LOCKED SCOPE =========================
                SpinLock::Guard lock(m_spinlock);
                return doDequeue(hint);
            }
            return doDequeue(hint);
        }

        inline ITask::Ptr IoQueue::tryDequeue(std::atomic_bool & hint)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            //========================= LOCKED SCOPE =========================
            SpinLock::Guard lock(m_spinlock, lock::tryToLock);
            if (lock.ownsLock())
            {
                return doDequeue(hint);
            }
            return nullptr;
        }

        inline ITask::Ptr IoQueue::doDequeue(std::atomic_bool & hint)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            hint = m_queue.empty();
            if (!hint)
            {
                ITask::Ptr task = m_queue.front();
                m_queue.pop_front();
                m_stats.decNumElements();
                return task;
            }
            return nullptr;
        }

        inline ITask::Ptr IoQueue::tryDequeueFromShared()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            static size_t index = 0;
            ITask::Ptr task;
            size_t size = 0;
            
            for (size_t i = 0; i < (*m_sharedIoQueues).size(); ++i)
            {
                IoQueue & queue = (*m_sharedIoQueues)[++index % (*m_sharedIoQueues).size()];
                size += queue.size();
                task = queue.tryDequeue(m_isIdle);
                if (task)
                {
                    return task;
                }
            }

            if (size)
            {
                // try again
                return tryDequeueFromShared();
            }
            return nullptr;
        }

        inline std::chrono::milliseconds IoQueue::getBackoffInterval()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_loadBalanceBackoffNum < m_loadBalancePollIntervalNumBackoffs)
            {
                ++m_loadBalanceBackoffNum;
            }
            if (m_loadBalancePollIntervalBackoffPolicy == Configuration::BackoffPolicy::Linear)
            {
                return m_loadBalancePollIntervalMs + (m_loadBalancePollIntervalMs * m_loadBalanceBackoffNum);
            }
            else
            {
                return m_loadBalancePollIntervalMs * static_cast<size_t>(std::exp2(m_loadBalanceBackoffNum));
            }
        }

        inline size_t IoQueue::size() const
        {
            // Add +1 to account for the currently executing task which is no longer on the queue.
#if defined(_GLIBCXX_USE_CXX11_ABI) && (_GLIBCXX_USE_CXX11_ABI == 0)
            return _isIdle ? _stats.numElements() : _stats.numElements() + 1;
#else
            return m_isIdle ? m_queue.size() : m_queue.size() + 1;
#endif
        }

        inline bool IoQueue::empty() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // If the queue is empty and there are no executing tasks
            return m_queue.empty() && m_isIdle;
        }

        inline void IoQueue::terminate()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            bool value {false};
            if (m_terminated.compare_exchange_strong(value, true) && m_sharedIoQueues)
            {
                {
                    std::unique_lock<std::mutex> lock(m_notEmptyMutex);
                    m_isInterrupted = true;
                }
                if (!m_loadBalanceSharedIoQueues)
                {
                    m_notEmptyCond.notify_all();
                }
                m_thread->join();
                m_queue.clear();
            }
        }

        inline IQueueStatistics & IoQueue::stats()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_stats;
        }

        inline SpinLock & IoQueue::getLock()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_spinlock;
        }

        inline void IoQueue::signalEmptyCondition(bool value)
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
        }

        inline ITask::Ptr IoQueue::grabWorkItem()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            static bool grabFromShared = false;
            ITask::Ptr task            = nullptr;
            grabFromShared             = !grabFromShared;

            if (grabFromShared)
            {
                //========================= LOCKED SCOPE (SHARED QUEUE) =========================
                SpinLock::Guard lock((*m_sharedIoQueues)[0].getLock());
                task = (*m_sharedIoQueues)[0].dequeue(m_isIdle);
                if (!task)
                {
                    //========================= LOCKED SCOPE =========================
                    SpinLock::Guard lock(m_spinlock);
                    task = dequeue(m_isIdle);
                    if (!task)
                    {
                        signalEmptyCondition(true);
                    }
                }
            }
            else
            {
                //========================= LOCKED SCOPE =========================
                SpinLock::Guard lock(m_spinlock);
                task = dequeue(m_isIdle);
                if (!task)
                {
                    //========================= LOCKED SCOPE (SHARED QUEUE) =========================
                    SpinLock::Guard lock((*m_sharedIoQueues)[0].getLock());
                    task = (*m_sharedIoQueues)[0].dequeue(m_isIdle);
                    if (!task)
                    {
                        signalEmptyCondition(true);
                    }
                }
            }
            return task;
        }

        inline ITask::Ptr IoQueue::grabWorkItemFromAll()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            static bool grabFromShared = false;
            ITask::Ptr task            = nullptr;
            grabFromShared             = !grabFromShared;

            if (grabFromShared)
            {
                task = tryDequeueFromShared();
                if (!task)
                {
                    task = dequeue(m_isIdle);
                }
            }
            else
            {
                task = dequeue(m_isIdle);
                if (!task)
                {
                    task = tryDequeueFromShared();
                }
            }
            return task;
        }

        inline bool IoQueue::isIdle() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_isIdle;
        }

        inline const std::shared_ptr<std::thread> & IoQueue::getThread() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_thread;
        }
}
