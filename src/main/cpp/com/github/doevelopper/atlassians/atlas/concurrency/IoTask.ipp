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
#include <exception>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Allocator.hpp>

namespace Bloomberg::quantum
{

#ifndef __QUANTUM_USE_DEFAULT_ALLOCATOR
    #ifdef __QUANTUM_ALLOCATE_POOL_FROM_HEAP
        using IoTaskAllocator = HeapAllocator<IoTask>;
    #else
        using IoTaskAllocator = StackAllocator<IoTask, __QUANTUM_IO_TASK_ALLOC_SIZE>;
    #endif
#else
        using IoTaskAllocator = StlAllocator<IoTask>;
#endif

        template <class RET, class FUNC, class... ARGS>
        IoTask::IoTask(
            std::true_type, std::shared_ptr<Promise<RET>> promise, int queueId, bool isHighPriority, FUNC && func,
            ARGS &&... args)
            : m_func(Util::bindIoCaller(promise, std::forward<FUNC>(func), std::forward<ARGS>(args)...))
            , m_terminated(false)
            , m_queueId(queueId)
            , m_isHighPriority(isHighPriority)
            , m_taskId(ThreadContextTag {})
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        template <class RET, class FUNC, class... ARGS>
        IoTask::IoTask(
            std::false_type, std::shared_ptr<Promise<RET>> promise, int queueId, bool isHighPriority, FUNC && func,
            ARGS &&... args)
            : m_func(Util::bindIoCaller2(promise, std::forward<FUNC>(func), std::forward<ARGS>(args)...))
            , m_terminated(false)
            , m_queueId(queueId)
            , m_isHighPriority(isHighPriority)
            , m_taskId(ThreadContextTag {})
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline IoTask::~IoTask()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            terminate();
        }

        inline void IoTask::terminate()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            bool value {false};
            if (m_terminated.compare_exchange_strong(value, true))
            {
                // not used
            }
        }

        inline int IoTask::run(
            const TaskStateHandler & stateHandler, TaskType taskHandledType, TaskState taskHandledStates)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_func)
            {
                TaskState taskState = TaskState::Initialized;
                handleTaskState(
                    stateHandler, m_taskId.id(), m_queueId, taskHandledType, taskHandledStates, TaskState::Started,
                    taskState);

                m_taskId.assignCurrentThread();
                int rc = 0;
                std::exception_ptr exception;
                try
                {
                    rc = m_func();
                }
                catch (...)
                {
                    exception = std::current_exception();
                }

                handleTaskState(
                    stateHandler, m_taskId.id(), m_queueId, taskHandledType, taskHandledStates, TaskState::Stopped,
                    taskState);
                if (exception)
                {
                    std::rethrow_exception(exception);
                }

                return rc;
            }
            return (int)ITask::RetCode::NotCallable;
        }

        inline void IoTask::setQueueId(int queueId)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_queueId = queueId;
        }

        inline int IoTask::getQueueId() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_queueId;
        }

        inline ITask::Type IoTask::getType() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return ITask::Type::IO;
        }

        inline TaskId IoTask::getTaskId() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_taskId;
        }

        inline bool IoTask::isBlocked() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return false;
        }

        inline bool IoTask::isSleeping(bool)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return false;
        }

        inline bool IoTask::isHighPriority() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_isHighPriority;
        }

        inline bool IoTask::isSuspended() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return false;
        }

        inline ITask::LocalStorage & IoTask::getLocalStorage()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_localStorage;
        }

        inline void * IoTask::operator new(size_t)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return Allocator<IoTaskAllocator>::instance(AllocatorTraits::ioTaskAllocSize()).allocate();
        }

        inline void IoTask::operator delete(void * p)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            Allocator<IoTaskAllocator>::instance(AllocatorTraits::ioTaskAllocSize())
                .deallocate(static_cast<IoTask *>(p));
        }

        inline void IoTask::deleter(IoTask * p)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
#ifndef __QUANTUM_USE_DEFAULT_ALLOCATOR
            Allocator<IoTaskAllocator>::instance(AllocatorTraits::ioTaskAllocSize()).dispose(p);
#else
            delete p;
#endif
        }
}
