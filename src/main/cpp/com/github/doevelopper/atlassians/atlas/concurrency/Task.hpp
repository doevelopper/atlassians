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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_TASK_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_TASK_H

#include <atomic>
#include <iostream>
#include <list>
#include <memory>
#include <thread>
#include <utility>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IContext.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IQueue.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITaskAccessor.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITaskContinuation.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITerminate.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/TaskStateHandler.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Traits.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/Util.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                 class Task
        //==============================================================================================
        /*!
        * @class Task.
        * @brief Runnable object representing a coroutine.
        * @note For internal use only.
        */
        class Task : public ITaskContinuation, public std::enable_shared_from_this<Task>
        {
        public:

            using Ptr     = std::shared_ptr<Task>;
            using WeakPtr = std::weak_ptr<Task>;

            enum class State : int
            {
                Running,
                Suspended,
                Terminated
            };

            template <class RET, class FUNC, class... ARGS>
            Task(
                std::false_type t, std::shared_ptr<Context<RET>> ctx, int queueId, bool isHighPriority,
                ITask::Type type, FUNC && func, ARGS &&... args);

            template <class RET, class FUNC, class... ARGS>
            Task(
                std::true_type t, std::shared_ptr<Context<RET>> ctx, int queueId, bool isHighPriority, ITask::Type type,
                FUNC && func, ARGS &&... args);

            Task(const Task & task)             = delete;
            Task(Task && task)                  = default;
            Task & operator=(const Task & task) = delete;
            Task & operator=(Task && task)      = default;

            ~Task();

            // ITerminate
            void terminate() final;

            // ITask
            int run(const TaskStateHandler & handler, TaskType handledTaskTypes, TaskState handledTaskStates) final;
            void setQueueId(int queueId) final;
            int getQueueId() const final;
            Type getType() const final;
            TaskId getTaskId() const final;
            bool isBlocked() const final;
            bool isSleeping(bool updateTimer = false) final;
            bool isHighPriority() const final;
            bool isSuspended() const final;

            // ITaskContinuation
            ITaskContinuation::Ptr getNextTask() final;
            void setNextTask(ITaskContinuation::Ptr nextTask) final;
            ITaskContinuation::Ptr getPrevTask() final;
            void setPrevTask(ITaskContinuation::Ptr prevTask) final;
            ITaskContinuation::Ptr getFirstTask() final;

            // Returns a final or error handler task in the chain and in the process frees all
            // the subsequent continuation tasks
            ITaskContinuation::Ptr getErrorHandlerOrFinalTask() final;

            // Local storage accessors
            LocalStorage & getLocalStorage() final;
            ITaskAccessor::Ptr getTaskAccessor() const;

            //===================================
            //           NEW / DELETE
            //===================================
            static void * operator new(size_t size);
            static void operator delete(void * p);
            static void deleter(Task * p);

        private:

            struct SuspensionGuard
            {
                SuspensionGuard(std::atomic_int & suspendedState)
                    : _isLocked(false)
                    , _suspendedState(suspendedState)
                {
                    int suspended = static_cast<int>(State::Suspended);
                    _isLocked     = _suspendedState.compare_exchange_strong(
                        suspended, static_cast<int>(State::Running), std::memory_order_acq_rel);
                }

                ~SuspensionGuard()
                {
                    if (_isLocked)
                    {
                        _suspendedState.store(static_cast<int>(State::Suspended), std::memory_order_acq_rel);
                    }
                }

                void set(int newState)
                {
                    _suspendedState.store(newState, std::memory_order_acq_rel);
                    _isLocked = false;
                }

                operator bool() const
                {
                    return _isLocked;
                }

                bool _isLocked;
                std::atomic_int & _suspendedState;
            };

            ITaskAccessor::Ptr m_coroContext; // holds execution context
            Traits::Coroutine m_currentRunnableCoroutine;
            int m_queueId;
            bool m_isHighPriority;
            ITaskContinuation::Ptr m_nextSheduledTaskToRun; // Task scheduled to run after current completes.
            ITaskContinuation::WeakPtr m_previousTaskInTheChain; // Previous task in the chain
            ITask::Type m_type;
            TaskId m_taskId;
            std::atomic_bool m_terminated;
            std::atomic_int m_suspendedState; // stores values of State
            ITask::LocalStorage m_localStorage; // local storage of the coroutine
            TaskState m_taskState; // task state

            LOG4CXX_DECLARE_STATIC_LOGGER
        };

        using TaskPtr     = Task::Ptr;
        using TaskWeakPtr = Task::WeakPtr;
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/Task.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_TASK_H
