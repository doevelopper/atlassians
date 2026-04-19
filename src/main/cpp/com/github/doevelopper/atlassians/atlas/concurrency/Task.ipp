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
#include <com/github/doevelopper/atlassians/atlas/concurrency/Allocator.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Traits.hpp>

namespace Bloomberg::quantum
{
#ifndef __QUANTUM_USE_DEFAULT_ALLOCATOR
    #ifdef __QUANTUM_ALLOCATE_POOL_FROM_HEAP
        using TaskAllocator = HeapAllocator<Task>;
    #else
        using TaskAllocator = StackAllocator<Task, __QUANTUM_TASK_ALLOC_SIZE>;
    #endif
#else
        using TaskAllocator = StlAllocator<Task>;
#endif

        template <class RET, class FUNC, class... ARGS>
        Task::Task(
            std::false_type, std::shared_ptr<Context<RET>> ctx, int queueId, bool isHighPriority, ITask::Type type,
            FUNC && func, ARGS &&... args)
            : m_coroContext(ctx)
            , m_currentRunnableCoroutine(
                  Allocator<CoroStackAllocator>::instance(AllocatorTraits::defaultCoroPoolAllocSize()),
                  Util::bindCaller(ctx, std::forward<FUNC>(func), std::forward<ARGS>(args)...))
            , m_queueId(queueId)
            , m_isHighPriority(isHighPriority)
            , m_type(type)
            , m_taskId(CoroContextTag {})
            , m_terminated(false)
            , m_suspendedState(static_cast<int>(State::Suspended))
            , m_taskState(TaskState::Initialized)
        {
        }

        template <class RET, class FUNC, class... ARGS>
        Task::Task(
            std::true_type, std::shared_ptr<Context<RET>> ctx, int queueId, bool isHighPriority, ITask::Type type,
            FUNC && func, ARGS &&... args)
            : m_coroContext(ctx)
            , m_currentRunnableCoroutine(
                  Allocator<CoroStackAllocator>::instance(AllocatorTraits::defaultCoroPoolAllocSize()),
                  Util::bindCaller2(ctx, std::forward<FUNC>(func), std::forward<ARGS>(args)...))
            , m_queueId(queueId)
            , m_isHighPriority(isHighPriority)
            , m_type(type)
            , m_taskId(CoroContextTag {})
            , m_terminated(false)
            , m_suspendedState(static_cast<int>(State::Suspended))
            , m_taskState(TaskState::Initialized)
        {
        }

        inline Task::~Task()
        {
            terminate();
        }

        inline void Task::terminate()
        {
            bool value {false};
            if (m_terminated.compare_exchange_strong(value, true))
            {
                if (m_coroContext)
                    m_coroContext->terminate();
            }
        }

        inline int Task::run(
            const TaskStateHandler & stateHandler, TaskType taskHandledType, TaskState taskHandledStates)
        {
            SuspensionGuard guard(m_suspendedState);
            if (guard)
            {
                if (!m_currentRunnableCoroutine)
                {
                    return static_cast<int>(ITask::RetCode::NotCallable);
                }
                if (isBlocked())
                {
                    return static_cast<int>(ITask::RetCode::Blocked);
                }
                if (isSleeping(true))
                {
                    return static_cast<int>(ITask::RetCode::Sleeping);
                }

                if (m_taskState == TaskState::Initialized)
                {
                    handleTaskState(
                        stateHandler, m_taskId.id(), m_queueId, taskHandledType, taskHandledStates, TaskState::Started,
                        m_taskState);
                }
                else
                {
                    handleTaskState(
                        stateHandler, m_taskId.id(), m_queueId, taskHandledType, taskHandledStates, TaskState::Resumed,
                        m_taskState);
                }
                int rc = static_cast<int>(ITask::RetCode::Running);
                m_taskId.assignCurrentThread();
                try
                {
                    m_currentRunnableCoroutine(rc);
                }
                catch (...)
                {
                    handleTaskState(
                        stateHandler, m_taskId.id(), m_queueId, taskHandledType, taskHandledStates, TaskState::Stopped,
                        m_taskState);
                    throw;
                }

                switch (rc)
                {
                case static_cast<int>(ITask::RetCode::Blocked):
                case static_cast<int>(ITask::RetCode::Sleeping):
                case static_cast<int>(ITask::RetCode::AlreadyResumed):
                    // Do nothing
                    break;
                case static_cast<int>(ITask::RetCode::Running):
                    handleTaskState(
                        stateHandler, m_taskId.id(), m_queueId, taskHandledType, taskHandledStates, TaskState::Suspended,
                        m_taskState);
                    break;
                default:
                    handleTaskState(
                        stateHandler, m_taskId.id(), m_queueId, taskHandledType, taskHandledStates, TaskState::Stopped,
                        m_taskState);
                    break;
                }

                if (!m_currentRunnableCoroutine)
                {
                    guard.set(static_cast<int>(State::Terminated));
                }
                return rc;
            }
            return static_cast<int>(ITask::RetCode::AlreadyResumed);
        }

        inline void Task::setQueueId(int queueId)
        {
            m_queueId = queueId;
        }

        inline int Task::getQueueId() const
        {
            return m_queueId;
        }

        inline ITask::Type Task::getType() const
        {
            return m_type;
        }

        inline TaskId Task::getTaskId() const
        {
            return m_taskId;
        }

        inline ITaskContinuation::Ptr Task::getNextTask()
        {
            return m_nextSheduledTaskToRun;
        }

        inline void Task::setNextTask(ITaskContinuation::Ptr nextTask)
        {
            m_nextSheduledTaskToRun = nextTask;
        }

        inline ITaskContinuation::Ptr Task::getPrevTask()
        {
            return m_previousTaskInTheChain.lock();
        }

        inline void Task::setPrevTask(ITaskContinuation::Ptr prevTask)
        {
            m_previousTaskInTheChain = prevTask;
        }

        inline ITaskContinuation::Ptr Task::getFirstTask()
        {
            return (m_type == Type::First) ? shared_from_this() : getPrevTask()->getFirstTask();
        }

        inline ITaskContinuation::Ptr Task::getErrorHandlerOrFinalTask()
        {
            if ((m_type == Type::ErrorHandler) || (m_type == Type::Final))
            {
                return shared_from_this();
            }
            else if (m_nextSheduledTaskToRun)
            {
                ITaskContinuation::Ptr task = m_nextSheduledTaskToRun->getErrorHandlerOrFinalTask();
                if ((m_nextSheduledTaskToRun->getType() != Type::ErrorHandler) && (m_nextSheduledTaskToRun->getType() != Type::Final))
                {
                    m_nextSheduledTaskToRun->terminate();
                    m_nextSheduledTaskToRun.reset(); // release next task
                }
                return task;
            }
            return nullptr;
        }

        inline bool Task::isBlocked() const
        {
            return m_coroContext ? m_coroContext->isBlocked() : false; // coroutine is waiting on some signal
        }

        inline bool Task::isSleeping(bool updateTimer)
        {
            return m_coroContext ? m_coroContext->isSleeping(updateTimer) : false; // coroutine is sleeping
        }

        inline bool Task::isHighPriority() const
        {
            return m_isHighPriority;
        }

        inline bool Task::isSuspended() const
        {
            return m_suspendedState == static_cast<int>(State::Suspended);
        }

        inline ITask::LocalStorage & Task::getLocalStorage()
        {
            return m_localStorage;
        }

        inline ITaskAccessor::Ptr Task::getTaskAccessor() const
        {
            return m_coroContext;
        }

        inline void * Task::operator new(size_t)
        {
            return Allocator<TaskAllocator>::instance(AllocatorTraits::taskAllocSize()).allocate();
        }

        inline void Task::operator delete(void * p)
        {
            Allocator<TaskAllocator>::instance(AllocatorTraits::taskAllocSize()).deallocate(static_cast<Task *>(p));
        }

        inline void Task::deleter(Task * p)
        {
#ifndef __QUANTUM_USE_DEFAULT_ALLOCATOR
            Allocator<TaskAllocator>::instance(AllocatorTraits::taskAllocSize()).dispose(p);
#else
            delete p;
#endif
        }
}
