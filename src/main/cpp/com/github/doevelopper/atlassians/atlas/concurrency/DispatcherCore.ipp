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

namespace Bloomberg::quantum
{
        inline DispatcherCore::DispatcherCore(const Configuration & config)
            : m_sharedCoroAnyQueue(
                config.getCoroutineSharingForAny() ? std::make_shared<TaskQueue>(config, nullptr) : nullptr)
            , m_sharedIoQueues((config.getNumIoThreads() <= 0) ? 1 : config.getNumIoThreads(), IoQueue(config, nullptr))
            , m_ioQueues(
                  (config.getNumIoThreads() <= 0) ? 1 : config.getNumIoThreads(), IoQueue(config, &m_sharedIoQueues))
            , m_loadBalanceSharedIoQueues(false)
            , m_terminated(false)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            const int coroCount = (config.getNumCoroutineThreads() == -1) ? std::thread::hardware_concurrency()
                                : (config.getNumCoroutineThreads() == 0)  ? 1
                                                                          : config.getNumCoroutineThreads();
            
            m_coroQueueIdRangeForAny = std::make_pair(0, coroCount - 1);

            const auto & coroQueueIdRangeForAny = config.getCoroQueueIdRangeForAny();
            // set the range to the default if the configured one is invalid or empty
            if (coroQueueIdRangeForAny.first <= coroQueueIdRangeForAny.second && coroQueueIdRangeForAny.first >= 0
                && coroQueueIdRangeForAny.second < (int)coroCount)
            {
                m_coroQueueIdRangeForAny = coroQueueIdRangeForAny;
            }

            // set thread name for shared queue
            if (m_sharedCoroAnyQueue)
            {
                IQueue::setThreadName(
                    IQueue::QueueType::Coro, m_sharedCoroAnyQueue->getThread()->native_handle(), 0, true, false);
            }

            // start the coro threads
            m_coroQueues.reserve(coroCount);
            for (int coroId = 0; coroId < coroCount; ++coroId)
            {
                bool hasSharedQueue =
                    (coroId >= m_coroQueueIdRangeForAny.first && coroId <= m_coroQueueIdRangeForAny.second);
                m_coroQueues.emplace_back(config, hasSharedQueue ? m_sharedCoroAnyQueue : nullptr);
                // set thread name for coro queues
                IQueue::setThreadName(
                    IQueue::QueueType::Coro, m_coroQueues.back().getThread()->native_handle(), coroId, false,
                    hasSharedQueue);
            }

            // set thread names for io queues
            for (size_t ioId = 0; ioId < m_ioQueues.size(); ++ioId)
            {
                IQueue::setThreadName(
                    IQueue::QueueType::IO, m_ioQueues[ioId].getThread()->native_handle(), ioId, false, false);
            }

            // pin to cores
            if (config.getPinCoroutineThreadsToCores())
            {
                unsigned int cores = std::thread::hardware_concurrency();
                for (size_t i = 0; i < m_coroQueues.size(); ++i)
                {
                    m_coroQueues[i].pinToCore(i % cores);
                }
            }
        }

        inline DispatcherCore::~DispatcherCore()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            terminate();
        }

        inline void DispatcherCore::terminate()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            bool value {false};
            if (m_terminated.compare_exchange_strong(value, true))
            {
                for (auto && queue : m_coroQueues)
                {
                    queue.terminate();
                }
                if (m_sharedCoroAnyQueue)
                {
                    m_sharedCoroAnyQueue->terminate();
                }
                for (auto && queue : m_ioQueues)
                {
                    queue.terminate();
                }
                for (auto && queue : m_sharedIoQueues)
                {
                    queue.terminate();
                }
            }
        }

        inline size_t DispatcherCore::size(IQueue::QueueType type, int queueId) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (type == IQueue::QueueType::All)
            {
                if (queueId != static_cast<int>(IQueue::QueueId::All))
                {
                    throw std::invalid_argument("Cannot specify queue id when type is 'All'");
                }
                return coroSize(static_cast<int>(IQueue::QueueId::All)) + ioSize(static_cast<int>(IQueue::QueueId::All));
            }
            else if (type == IQueue::QueueType::Coro)
            {
                return coroSize(queueId);
            }
            return ioSize(queueId);
        }

        inline bool DispatcherCore::empty(IQueue::QueueType type, int queueId) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (type == IQueue::QueueType::All)
            {
                if (queueId != static_cast<int>(IQueue::QueueId::All))
                {
                    throw std::invalid_argument("Cannot specify queue id when type is 'All'");
                }
                return coroEmpty(static_cast<int>(IQueue::QueueId::All)) && ioEmpty(static_cast<int>(IQueue::QueueId::All));
            }
            else if (type == IQueue::QueueType::Coro)
            {
                return coroEmpty(queueId);
            }
            return ioEmpty(queueId);
        }

        inline size_t DispatcherCore::coroSize(int queueId) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (queueId == static_cast<int>(IQueue::QueueId::All))
            {
                size_t size = 0;
                for (auto && queue : m_coroQueues)
                {
                    size += queue.size();
                }
                if (m_sharedCoroAnyQueue)
                {
                    size += m_sharedCoroAnyQueue->size();
                }
                return size;
            }
            else if ((queueId >= static_cast<int>(m_coroQueues.size())) || (queueId < 0))
            {
                throw std::out_of_range(std::string {"Invalid coroutine queue id: "} + std::to_string(queueId));
            }
            return m_coroQueues.at(queueId).size();
        }

        inline bool DispatcherCore::coroEmpty(int queueId) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (queueId == static_cast<int>(IQueue::QueueId::All))
            {
                for (auto && queue : m_coroQueues)
                {
                    if (!queue.empty())
                        return false;
                }
                if (m_sharedCoroAnyQueue && !m_sharedCoroAnyQueue->empty())
                {
                    return false;
                }
                return true;
            }
            else if ((queueId >= static_cast<int>(m_coroQueues.size())) || (queueId < 0))
            {
                throw std::out_of_range(std::string {"Invalid coroutine queue id: "} + std::to_string(queueId));
            }
            return m_coroQueues.at(queueId).empty();
        }

        inline size_t DispatcherCore::ioSize(int queueId) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (queueId == static_cast<int>(IQueue::QueueId::All))
            {
                size_t size = 0;
                for (auto && queue : m_ioQueues)
                {
                    size += queue.size();
                }
                for (auto && queue : m_sharedIoQueues)
                {
                    size += queue.size();
                }
                return size;
            }
            else if (queueId == static_cast<int>(IQueue::QueueId::Any))
            {
                size_t size = 0;
                for (auto && queue : m_sharedIoQueues)
                {
                    size += queue.size();
                }
                return size;
            }
            return m_ioQueues.at(queueId).size();
        }

        inline bool DispatcherCore::ioEmpty(int queueId) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (queueId == static_cast<int>(IQueue::QueueId::All))
            {
                for (auto && queue : m_sharedIoQueues)
                {
                    if (!queue.empty())
                    {
                        return false;
                    }
                }
                for (auto && queue : m_ioQueues)
                {
                    if (!queue.empty())
                    {
                        return false;
                    }
                }
                return true;
            }
            else if (queueId == static_cast<int>(IQueue::QueueId::Any))
            {
                for (auto && queue : m_sharedIoQueues)
                {
                    if (!queue.empty())
                    {
                        return false;
                    }
                }
                return true;
            }
            return m_ioQueues.at(queueId).empty();
        }

        inline QueueStatistics DispatcherCore::stats(IQueue::QueueType type, int queueId)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (type == IQueue::QueueType::All)
            {
                if (queueId != static_cast<int>(IQueue::QueueId::All))
                {
                    throw std::invalid_argument("Cannot specify queue id when queue type is 'All'");
                }
                return coroStats(static_cast<int>(IQueue::QueueId::All)) + ioStats(static_cast<int>(IQueue::QueueId::All));
            }
            else if (type == IQueue::QueueType::Coro)
            {
                return coroStats(queueId);
            }
            return ioStats(queueId);
        }

        inline QueueStatistics DispatcherCore::coroStats(int queueId)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (queueId == static_cast<int>(IQueue::QueueId::All))
            {
                QueueStatistics stats;
                for (auto && queue : m_coroQueues)
                {
                    stats += queue.stats();
                }
                if (m_sharedCoroAnyQueue)
                {
                    stats += m_sharedCoroAnyQueue->stats();
                }
                return stats;
            }
            else
            {
                if ((queueId >= static_cast<int>(m_coroQueues.size())) || (queueId < 0))
                {
                    throw std::out_of_range(std::string {"Invalid coroutine queue id: "} + std::to_string(queueId));
                }
                return static_cast<const QueueStatistics &>(m_coroQueues.at(queueId).stats());
            }
        }

        inline QueueStatistics DispatcherCore::ioStats(int queueId)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (queueId == static_cast<int>(IQueue::QueueId::All))
            {
                QueueStatistics stats;
                for (auto && queue : m_ioQueues)
                {
                    stats += queue.stats();
                }
                for (auto && queue : m_sharedIoQueues)
                {
                    stats += queue.stats();
                }
                return stats;
            }
            else if (queueId == static_cast<int>(IQueue::QueueId::Any))
            {
                QueueStatistics stats;
                for (auto && queue : m_sharedIoQueues)
                {
                    stats += queue.stats();
                }
                return stats;
            }
            else
            {
                if ((queueId >= static_cast<int>(m_ioQueues.size())) || (queueId < 0))
                {
                    throw std::invalid_argument(std::string {"Invalid IO queue id: "} + std::to_string(queueId));
                }
                return static_cast<const QueueStatistics &>(m_ioQueues.at(queueId).stats());
            }
        }

        inline void DispatcherCore::resetStats()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            for (auto && queue : m_coroQueues)
            {
                queue.stats().reset();
            }
            if (m_sharedCoroAnyQueue)
            {
                m_sharedCoroAnyQueue->stats().reset();
            }
            for (auto && queue : m_sharedIoQueues)
            {
                queue.stats().reset();
            }
            for (auto && queue : m_ioQueues)
            {
                queue.stats().reset();
            }
        }

        inline void DispatcherCore::post(Task::Ptr task)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!task)
            {
                return;
            }

            if (task->getQueueId() == static_cast<int>(IQueue::QueueId::Any))
            {
                if (m_sharedCoroAnyQueue)
                {
                    m_sharedCoroAnyQueue->enqueue(task);
                    return;
                }

                size_t index = 0;

                // Insert into the shortest queue or the first empty queue found
                size_t numTasks = std::numeric_limits<size_t>::max();
                for (size_t i = static_cast<size_t>(m_coroQueueIdRangeForAny.first); i <= static_cast<size_t>(m_coroQueueIdRangeForAny.second); ++i)
                {
                    size_t queueSize = m_coroQueues[i].size();
                    if (queueSize < numTasks)
                    {
                        numTasks = queueSize;
                        index    = i;
                    }
                    if (numTasks == 0)
                    {
                        break; // reached an empty queue
                    }
                }

                task->setQueueId(index); // overwrite the queueId with the selected one
            }
            else
            {
                if (task->getQueueId() >= static_cast<int>(m_coroQueues.size()))
                {
                    throw std::out_of_range(
                        std::string {"Invalid coroutine queue id: "} + std::to_string(task->getQueueId()));
                }
            }
            
            m_coroQueues.at(task->getQueueId()).enqueue(task);
        }

        inline void DispatcherCore::postAsyncIo(IoTask::Ptr task)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!task)
            {
                return;
            }

            if (task->getQueueId() == static_cast<int>(IQueue::QueueId::Any))
            {
                if (m_loadBalanceSharedIoQueues)
                {
                    static size_t index = 0;
                    // loop until we can find an queue that won't block
                    while (1)
                    {
                        if (m_sharedIoQueues.at(++index % m_sharedIoQueues.size()).tryEnqueue(task))
                        {
                            break;
                        }
                    }
                }
                else
                {
                    // insert the task into the shared queue
                    m_sharedIoQueues[0].enqueue(task);

                    // Signal all threads there is work to do
                    for (auto && queue : m_ioQueues)
                    {
                        queue.signalEmptyCondition(false);
                    }
                }
            }
            else
            {
                if (task->getQueueId() >= static_cast<int>(m_ioQueues.size()))
                {
                    throw std::out_of_range(std::string {"Invalid IO queue id: "} + std::to_string(task->getQueueId()));
                }

                // Run on specific queue
                m_ioQueues.at(task->getQueueId()).enqueue(task);
            }
        }

        inline int DispatcherCore::getNumCoroutineThreads() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_coroQueues.size();
        }

        inline int DispatcherCore::getNumIoThreads() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_ioQueues.size();
        }

        inline const std::pair<int, int> & DispatcherCore::getCoroQueueIdRangeForAny() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_coroQueueIdRangeForAny;
        }
}
