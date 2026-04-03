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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_IO_QUEUE_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_IO_QUEUE_H

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <list>
#include <thread>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IQueue.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITask.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITerminate.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Configuration.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/IoTask.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/QueueStatistics.hpp>

namespace Bloomberg::quantum
{
#ifndef __QUANTUM_USE_DEFAULT_ALLOCATOR
    #ifdef __QUANTUM_ALLOCATE_POOL_FROM_HEAP
        using IoQueueListAllocator = HeapAllocator<IoTask::Ptr>;
    #else
        using IoQueueListAllocator = StackAllocator<IoTask::Ptr, __QUANTUM_IO_QUEUE_LIST_ALLOC_SIZE>;
    #endif
#else
        using IoQueueListAllocator = StlAllocator<IoTask::Ptr>;
#endif

        //==============================================================================================
        //                                 class IoQueue
        //==============================================================================================
        /*!
         * @class IoQueue
         * @brief Thread queue for executing IO tasks.
         * @note For internal use only.
         */
        class IoQueue : public IQueue
        {
        public:

            using TaskList     = std::list<IoTask::Ptr, IoQueueListAllocator>;
            using TaskListIter = TaskList::iterator;

            IoQueue();

            IoQueue(const Configuration & config, std::vector<IoQueue> * sharedIoQueues);

            IoQueue(const IoQueue & other);

            IoQueue(IoQueue && other) = default;

            ~IoQueue();

            void terminate() final;

            void pinToCore(int coreId) final;

            void run() final;

            void enqueue(ITask::Ptr task) final;

            bool tryEnqueue(ITask::Ptr task) final;

            ITask::Ptr dequeue(std::atomic_bool & hint) final;

            ITask::Ptr tryDequeue(std::atomic_bool & hint) final;

            size_t size() const final;

            bool empty() const final;

            IQueueStatistics & stats() final;

            SpinLock & getLock() final;

            void signalEmptyCondition(bool value) final;

            bool isIdle() const final;

            const std::shared_ptr<std::thread> & getThread() const final;

        private:

            ITask::Ptr grabWorkItem();
            ITask::Ptr grabWorkItemFromAll();
            void doEnqueue(ITask::Ptr task);
            ITask::Ptr doDequeue(std::atomic_bool & hint);
            ITask::Ptr tryDequeueFromShared();
            std::chrono::milliseconds getBackoffInterval();

            // async IO queue
            std::vector<IoQueue> * m_sharedIoQueues;
            bool m_loadBalanceSharedIoQueues;
            std::chrono::milliseconds m_loadBalancePollIntervalMs;
            Configuration::BackoffPolicy m_loadBalancePollIntervalBackoffPolicy;
            size_t m_loadBalancePollIntervalNumBackoffs;
            size_t m_loadBalanceBackoffNum;
            std::shared_ptr<std::thread> m_thread;
            TaskList m_queue;
            mutable SpinLock m_spinlock;
            std::mutex m_notEmptyMutex; // for accessing the condition variable
            std::condition_variable m_notEmptyCond;
            std::atomic_bool m_isEmpty;
            std::atomic_bool m_isInterrupted;
            std::atomic_bool m_isIdle;
            std::atomic_bool m_terminated;
            QueueStatistics m_stats;
            TaskStateConfiguration m_taskStateConfiguration;
            LOG4CXX_DECLARE_STATIC_LOGGER
        };
        inline log4cxx::LoggerPtr IoQueue::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.IoQueue" ) );
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/IoQueue.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_IO_QUEUE_H
