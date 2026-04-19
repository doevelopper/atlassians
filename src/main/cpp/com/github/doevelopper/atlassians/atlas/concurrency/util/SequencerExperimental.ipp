/*
** Copyright 2021 Bloomberg Finance L.P.
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

#include <functional>
#include <stdexcept>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Stl.ipp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Local.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Promise.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Traits.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/DrainGuard.hpp>

namespace Bloomberg::quantum::experimental
{
            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::Sequencer(
                Dispatcher & dispatcher,
                const typename Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::Configuration & configuration)
                : m_dispatcher(dispatcher)
                , m_drain(false)
                , m_pendingTaskQueueMap(
                      configuration.getBucketCount(), configuration.getHash(), configuration.getKeyEqual(),
                      configuration.getAllocator())
                , m_exceptionCallback(configuration.getExceptionCallback())
                , m_taskStats(std::make_shared<SequenceKeyStatisticsWriter>())
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            bool Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::addPendingTask(
                const std::shared_ptr<SequencerTask<SequenceKey>> & task)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                bool empty = m_universalTaskQueue._tasks.empty();
                m_universalTaskQueue._tasks.push_back(task);
                if (not empty)
                {
                    ++task->_pendingKeyCount;
                }
                m_universalTaskQueue._stats->incrementPostedTaskCount();
                m_universalTaskQueue._stats->incrementPendingTaskCount();

                return empty;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            bool Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::addPendingTask(
                const SequenceKey & key, const std::shared_ptr<SequencerTask<SequenceKey>> & task)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                typename PendingTaskQueueMap::iterator it = m_pendingTaskQueueMap.find(key);
                if (it == m_pendingTaskQueueMap.end())
                {
                    it = m_pendingTaskQueueMap.emplace(key, SequencerKeyData<SequenceKey>()).first;
                    LOG4CXX_DEBUG(logger, "Add each universal task to this queue before the task.");
                    // add each universal task to this queue before the task
                    bool first = true;
                    for (auto & universalTask : m_universalTaskQueue._tasks)
                    {
                        it->second._tasks.push_back(universalTask);
                        LOG4CXX_DEBUG(logger, "Increment the pending count of the universal task unless it's first in the queue.");
                        // increment the pending count of the universal task unless it's first in the queue
                        if (not first)
                        {
                            ++universalTask->_pendingKeyCount;
                        }
                        first = false;
                    }
                }
                bool empty = it->second._tasks.empty();
                it->second._tasks.push_back(task);
                if (not empty)
                {
                    ++task->_pendingKeyCount;
                }
                if (not task->_universal)
                {
                    task->_keyData.push_back(&it->second);
                    it->second._stats->incrementPostedTaskCount();
                    it->second._stats->incrementPendingTaskCount();
                }
                return empty;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            std::shared_ptr<SequencerTask<SequenceKey>>
            Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::removeCompleted(
                SequencerKeyData<SequenceKey> & entry, const std::shared_ptr<SequencerTask<SequenceKey>> & task)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                // Regular tasks tasks:
                // * entry._tasks.empty() means that there's a bug somewhere: when we intend to delete a pending task,
                // the task queue must not be empty
                // * entry._tasks.front() != task means that there's a bug somewhere:
                // only completed tasks are removed from the queue, and such tasks must sit at the head of the queue
                // Universal tasks: because we do not track the queues where universal tasks are enqueued,
                // entry._tasks.empty() or entry._tasks.front() != task just means that the task was never enqueued
                // to this queue.
                if (entry._tasks.empty() or entry._tasks.front() != task)
                {
                    LOG4CXX_ERROR(logger, "bug somewhere: when we intend to delete a pending task.");
                    return nullptr;
                }

                entry._tasks.pop_front();
                if (entry._tasks.empty())
                {
                    LOG4CXX_ERROR(logger, "No more tasks in the queue.");
                    // no more tasks in the queue
                    return nullptr;
                }
                std::shared_ptr<SequencerTask<SequenceKey>> next = entry._tasks.front();
                // decrementing _pendingKeyCount because the task next is now the head of the queue.
                // As a result, the next task has one less pending dependent to wait for
                return 0 == --next->_pendingKeyCount ? next : nullptr;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::removeCompletedAndScheduleNext(
                VoidContextPtr ctx, const std::shared_ptr<SequencerTask<SequenceKey>> & task)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                Mutex::Guard lock(ctx, m_mutex);
                if (task->_universal)
                {
                    LOG4CXX_DEBUG(logger, "Remove the task from all key queues.");
                    // remove the task from all key queues
                    for (auto & item : m_pendingTaskQueueMap)
                    {
                        if (auto nextTask = removeCompleted(item.second, task))
                        {
                            scheduleTask(nextTask);
                        }
                    }

                    LOG4CXX_DEBUG(logger, "Remove the task from the universal queue.");
                    // remove the task from the universal queue
                    if (auto nextTask = removeCompleted(m_universalTaskQueue, task))
                    {
                        scheduleTask(nextTask);
                    }
                }
                else
                {
                    LOG4CXX_DEBUG(logger, "Remove the task from its key queues only.");
                    // remove the task from its key queues only
                    for (SequencerKeyData<SequenceKey> * data : task->_keyData)
                    {
                        if (auto nextTask = removeCompleted(*data, task))
                        {
                            scheduleTask(nextTask);
                        }
                    }
                }
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            int Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::executePending(
                VoidContextPtr ctx, Sequencer * sequencer, std::shared_ptr<SequencerTask<SequenceKey>> task)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                // passing Sequencer pointer into the static executePending instead of
                // making executePending non-static because executePending is passed to
                // dispatcher as a coroutine function, but dispatcher.post(...) doesn't support
                // results of std::bind(...)
                int rc = -1;
                try
                {
                    rc = task->_func(ctx);
                }
                //    catch(const boost::coroutines2::detail::forced_unwind&)
                catch (const boost::context::detail::forced_unwind &) // 1.83.0
                {
                    LOG4CXX_ERROR(logger, "Quantum context switch.");
                    // quantum context switch
                    throw;
                }
                catch (...)
                {
                    if (sequencer->m_exceptionCallback)
                    {
                        sequencer->m_exceptionCallback(std::current_exception(), task->_opaque);
                    }
                }
                LOG4CXX_DEBUG(logger, "Remove the task from the pending queues + schedule next tasks.");
                // remove the task from the pending queues + schedule next tasks
                sequencer->removeCompletedAndScheduleNext(ctx, task);
                return rc;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::scheduleTask(
                const std::shared_ptr<SequencerTask<SequenceKey>> & task)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                if (task->_universal)
                {
                    m_universalTaskQueue._stats->decrementPendingTaskCount();
                }
                else
                {
                    for (SequencerKeyData<SequenceKey> * keyData : task->_keyData)
                    {
                        keyData->_stats->decrementPendingTaskCount();
                    }
                }
                m_taskStats->decrementPendingTaskCount();

                m_dispatcher.post(
                    task->_queueId, task->_isHighPriority,
                    &Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::executePending, this,
                    std::shared_ptr<SequencerTask<SequenceKey>>(task));
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            template <class FUNC, class... ARGS>
            void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueue(
                const SequenceKey & sequenceKey, FUNC && func, ARGS &&... args)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                enqueueSingle(
                    nullptr, static_cast<int>(IQueue::QueueId::Any), false, sequenceKey, std::forward<FUNC>(func),
                    std::forward<ARGS>(args)...);
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            template <class FUNC, class... ARGS>
            void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueue(
                void * opaque, int queueId, bool isHighPriority, const SequenceKey & sequenceKey, FUNC && func,
                ARGS &&... args)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                enqueueSingle(
                    opaque, queueId, isHighPriority, sequenceKey, std::forward<FUNC>(func),
                    std::forward<ARGS>(args)...);
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            template <class FUNC, class... ARGS>
            void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueueSingle(
                void * opaque, int queueId, bool isHighPriority, const SequenceKey & sequenceKey, FUNC && func,
                ARGS &&... args)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                if (m_drain)
                {
                    throw std::runtime_error("Sequencer is disabled");
                }

                if (queueId < static_cast<int>(IQueue::QueueId::Any))
                {
                    throw std::out_of_range(std::string {"Invalid IO queue id: "} + std::to_string(queueId));
                }

                auto task = std::make_shared<SequencerTask<SequenceKey>>(
                    makeCapture<int>(std::forward<FUNC>(func), std::forward<ARGS>(args)...), false, opaque, queueId,
                    isHighPriority);

                Mutex::Guard lock(local::context(), m_mutex);

                m_taskStats->incrementPostedTaskCount();
                m_taskStats->incrementPendingTaskCount();

                if (addPendingTask(sequenceKey, task))
                {
                    scheduleTask(task);
                }
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            template <class FUNC, class... ARGS>
            void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueue(
                const std::vector<SequenceKey> & sequenceKeys, FUNC && func, ARGS &&... args)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                enqueueMultiple(
                    nullptr, static_cast<int>(IQueue::QueueId::Any), false, sequenceKeys, std::forward<FUNC>(func),
                    std::forward<ARGS>(args)...);
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            template <class FUNC, class... ARGS>
            void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueue(
                void * opaque, int queueId, bool isHighPriority, const std::vector<SequenceKey> & sequenceKeys,
                FUNC && func, ARGS &&... args)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                enqueueMultiple(
                    opaque, queueId, isHighPriority, sequenceKeys, std::forward<FUNC>(func),
                    std::forward<ARGS>(args)...);
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            template <class FUNC, class... ARGS>
            void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueueMultiple(
                void * opaque, int queueId, bool isHighPriority, const std::vector<SequenceKey> & sequenceKeys,
                FUNC && func, ARGS &&... args)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                if (m_drain)
                {
                    throw std::runtime_error("Sequencer is disabled");
                }
                if (queueId < static_cast<int>(IQueue::QueueId::Any))
                {
                    throw std::out_of_range(std::string {"Invalid IO queue id: "} + std::to_string(queueId));
                }

                auto task = std::make_shared<SequencerTask<SequenceKey>>(
                    makeCapture<int>(std::forward<FUNC>(func), std::forward<ARGS>(args)...), false, opaque, queueId,
                    isHighPriority);

                Mutex::Guard lock(local::context(), m_mutex);

                m_taskStats->incrementPostedTaskCount();
                m_taskStats->incrementPendingTaskCount();

                bool canSchedule = true;
                for (const SequenceKey & sequenceKey : sequenceKeys)
                {
                    if (not addPendingTask(sequenceKey, task))
                    {
                        canSchedule = false;
                    }
                }
                if (canSchedule)
                {
                    scheduleTask(task);
                }
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            template <class FUNC, class... ARGS>
            void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueueAll(FUNC && func, ARGS &&... args)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                enqueueAllImpl(
                    nullptr, static_cast<int>(IQueue::QueueId::Any), false, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            template <class FUNC, class... ARGS>
            void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueueAll(
                void * opaque, int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                enqueueAllImpl(opaque, queueId, isHighPriority, std::move(func), std::forward<ARGS>(args)...);
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            template <class FUNC, class... ARGS>
            void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueueAllImpl(
                void * opaque, int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                if (m_drain)
                {
                    throw std::runtime_error("Sequencer is disabled");
                }
                if (queueId < static_cast<int>(IQueue::QueueId::Any))
                {
                    throw std::out_of_range(std::string {"Invalid IO queue id: "} + std::to_string(queueId));
                }

                auto task = std::make_shared<SequencerTask<SequenceKey>>(
                    makeCapture<int>(std::forward<FUNC>(func), std::forward<ARGS>(args)...), true, opaque, queueId,
                    isHighPriority);

                Mutex::Guard lock(local::context(), m_mutex);

                m_taskStats->incrementPostedTaskCount();
                m_taskStats->incrementPendingTaskCount();

                bool canSchedule = addPendingTask(task);
                for (const auto & pendingItem : m_pendingTaskQueueMap)
                {
                    if (not addPendingTask(pendingItem.first, task))
                    {
                        canSchedule = false;
                    }
                }
                if (canSchedule)
                {
                    scheduleTask(task);
                }
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            size_t Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::trimSequenceKeys()
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                Mutex::Guard lock(local::context(), m_mutex);
                for (typename PendingTaskQueueMap::iterator it = m_pendingTaskQueueMap.begin();
                     it != m_pendingTaskQueueMap.end();)
                {
                    if (it->second._tasks.empty())
                        it = m_pendingTaskQueueMap.erase(it);
                    else
                        ++it;
                }
                return m_pendingTaskQueueMap.size();
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            SequenceKeyStatistics
            Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::getStatistics(const SequenceKey & sequenceKey)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                Mutex::Guard lock(local::context(), m_mutex);
                typename PendingTaskQueueMap::const_iterator it = m_pendingTaskQueueMap.find(sequenceKey);
                if (it == m_pendingTaskQueueMap.end())
                    return SequenceKeyStatistics();
                return *it->second._stats;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            SequenceKeyStatistics Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::getStatistics()
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                Mutex::Guard lock(local::context(), m_mutex);
                return *m_universalTaskQueue._stats;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            SequenceKeyStatistics Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::getTaskStatistics()
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                return *m_taskStats;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            size_t Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::getSequenceKeyCount()
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                Mutex::Guard lock(local::context(), m_mutex);
                return m_pendingTaskQueueMap.size();
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            bool
            Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::drain(std::chrono::milliseconds timeout, bool isFinal)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                std::shared_ptr<Promise<int>> promise = std::make_shared<Promise<int>>();
                ThreadFuturePtr<int> future           = promise->getIThreadFuture();
                LOG4CXX_DEBUG(logger, "Enqueue a universal task and wait");
                // enqueue a universal task and wait
                enqueueAll([promise](VoidContextPtr ctx) -> int { return promise->set(ctx, 0); });

                DrainGuard guard(m_drain, !isFinal);
                return future->waitFor(timeout) == std::future_status::ready;
            }
}
