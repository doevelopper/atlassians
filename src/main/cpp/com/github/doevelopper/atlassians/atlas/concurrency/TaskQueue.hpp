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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_TASK_QUEUE_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_TASK_QUEUE_H

#include <pthread.h>
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IQueue.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITaskContinuation.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITerminate.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Configuration.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/QueueStatistics.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Spinlock.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Task.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/TaskStateHandler.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/YieldingThread.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                 class TaskQueue
        //==============================================================================================
        /*!
        * @class TaskQueue.
        * @brief Thread queue for running coroutines.
        * @note For internal use only.
        */
        class TaskQueue : public IQueue
        {
        public:

            using TaskList     = std::list<Task::Ptr, ContiguousPoolManager<Task::Ptr>>;
            using TaskListIter = TaskList::iterator;

            TaskQueue();

            TaskQueue(const Configuration & configuration, std::shared_ptr<TaskQueue> sharedQueue);

            TaskQueue(const TaskQueue & other);

            TaskQueue(TaskQueue && other) = default;

            ~TaskQueue();

            void pinToCore(int coreId) final;

            void run() final;

            void enqueue(ITask::Ptr task) final;

            bool tryEnqueue(ITask::Ptr task) final;

            ITask::Ptr dequeue(std::atomic_bool & hint) final;

            ITask::Ptr tryDequeue(std::atomic_bool & hint) final;

            size_t size() const final;

            bool empty() const final;

            void terminate() final;

            IQueueStatistics & stats() final;

            SpinLock & getLock() final;

            void signalEmptyCondition(bool value) final;

            bool isIdle() const final;

            const std::shared_ptr<std::thread> & getThread() const final;

        private:

            struct WorkItem
            {
                WorkItem(TaskPtr task, TaskListIter iter, bool isBlocked, unsigned int blockedQueueRound);

                TaskPtr _task; // task pointer
                TaskListIter _iter; // task iterator
                bool _isBlocked; // true if the entire queue is blocked
                unsigned int _blockedQueueRound; // blocked queue round id
            };

            struct ProcessTaskResult
            {
                ProcessTaskResult(bool isBlocked, unsigned int blockedQueueRound);

                bool _isBlocked; // true if the entire queue is blocked
                unsigned int _blockedQueueRound; // blocked queue round id
            };

            // Coroutine result handlers
            bool handleNotCallable(const WorkItem & entry);
            bool handleAlreadyResumed(WorkItem & entry);
            bool handleRunning(WorkItem & entry);
            bool handleSuccess(const WorkItem & entry);
            bool handleBlocked(WorkItem & entry);
            bool handleSleeping(WorkItem & entry);
            bool handleError(const WorkItem & entry);
            bool handleException(const WorkItem & workItem, const std::exception * ex = nullptr);

            void onBlockedTask(WorkItem & entry);
            void onActiveTask(WorkItem & entry);

            bool isInterrupted();
            void signalSharedQueueEmptyCondition(bool value);
            ProcessTaskResult processTask();
            WorkItem grabWorkItem();
            void doEnqueue(ITask::Ptr task);
            ITask::Ptr doDequeue(std::atomic_bool & hint, TaskListIter iter);
            void acquireWaiting();
            void sleepOnBlockedQueue(const ProcessTaskResult & mainQueueResult);
            void sleepOnBlockedQueue(
                const ProcessTaskResult & mainQueueResult, const ProcessTaskResult & sharedQueueResult);

            QueueListAllocator m_alloc;
            std::shared_ptr<std::thread> m_thread;
            TaskList m_runQueue;
            TaskList m_waitQueue;
            TaskListIter m_queueIt;
            TaskListIter m_blockedIt;
            bool m_isBlocked;
            mutable SpinLock m_runQueueLock;
            mutable SpinLock m_waitQueueLock;
            std::mutex m_notEmptyMutex; // for accessing the condition variable
            std::condition_variable m_notEmptyCond;
            std::atomic_bool m_isEmpty;
            std::atomic_bool m_isSharedQueueEmpty;
            std::atomic_bool m_isInterrupted;
            std::atomic_bool m_isIdle;
            std::atomic_bool m_terminated;
            bool m_isAdvanced;
            QueueStatistics m_stats;
            std::shared_ptr<TaskQueue> m_sharedQueue;
            std::vector<TaskQueue *> m_helpers;
            unsigned int m_queueRound;
            unsigned int m_lastSleptQueueRound;
            unsigned int m_lastSleptSharedQueueRound;
            TaskStateConfiguration m_taskStateConfiguration;
            LOG4CXX_DECLARE_STATIC_LOGGER
        };
        inline log4cxx::LoggerPtr TaskQueue::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.TaskQueue" ) );
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/TaskQueue.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_TASK_QUEUE_H
