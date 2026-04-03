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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_DISPATCHER_CORE_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_DISPATCHER_CORE_H

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Configuration.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/IoQueue.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/TaskQueue.hpp>
#if defined(_WIN32) && !defined(__CYGWIN__)
    #include <winbase.h>
#else
    #include <pthread.h>
#endif

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                 class DispatcherCore
        //==============================================================================================
        /*!
        *  @class DispatcherCore
        *  @brief Class implementing the dispatching logic unto worker threads. Used for both coroutines
        *      and IO tasks.
        *  @note For internal use only.
        */
        class DispatcherCore : public ITerminate
        {
        public:

            friend class Dispatcher;

            ~DispatcherCore();

            void terminate() final;

            size_t size(IQueue::QueueType type, int queueId) const;

            bool empty(IQueue::QueueType type, int queueId) const;

            QueueStatistics stats(IQueue::QueueType type, int queueId);

            void resetStats();

            void post(Task::Ptr task);

            void postAsyncIo(IoTask::Ptr task);

            int getNumCoroutineThreads() const;

            int getNumIoThreads() const;

            const std::pair<int, int> & getCoroQueueIdRangeForAny() const;

        private:

            DispatcherCore(const Configuration & config);

            size_t coroSize(int queueId) const;

            size_t ioSize(int queueId) const;

            bool coroEmpty(int queueId) const;

            bool ioEmpty(int queueId) const;

            QueueStatistics coroStats(int queueId);

            QueueStatistics ioStats(int queueId);

            // Members
            std::shared_ptr<TaskQueue> m_sharedCoroAnyQueue; // shared coro queue for Any
            std::vector<TaskQueue> m_coroQueues; // coroutine queues
            std::vector<IoQueue> m_sharedIoQueues; // shared IO task queues (hold tasks posted to 'Any' IO queue)
            std::vector<IoQueue> m_ioQueues; // dedicated IO task queues
            bool m_loadBalanceSharedIoQueues; // tasks posted to 'Any' IO queue are load balanced
            std::atomic_bool m_terminated;
            std::pair<int, int> m_coroQueueIdRangeForAny; // range of coroutine queueIds covered by 'Any'

            LOG4CXX_DECLARE_STATIC_LOGGER
        };
    inline log4cxx::LoggerPtr DispatcherCore::logger =
        log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.DispatcherCore" ) );
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/DispatcherCore.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_DISPATCHER_CORE_H
