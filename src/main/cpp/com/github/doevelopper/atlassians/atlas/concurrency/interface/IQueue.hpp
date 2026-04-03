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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_IQUEUE_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_IQUEUE_H

#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IQueueStatistics.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITask.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITerminate.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Allocator.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Spinlock.hpp>
#ifndef __GLIBC__
    #include <pthread.h>
#endif
#include <string.h>
#include <thread>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                  interface IQueue
        //==============================================================================================
        /// @interface IQueue
        /// @brief Interface to a task queue. For internal use only.
        struct IQueue : public ITerminate
        {
            // Typedefs and enum definitions
            using Ptr = std::shared_ptr<IQueue>;
            enum class QueueType : std::uint8_t
            {
                Coro,
                IO,
                All
            };
            enum class QueueId : std::int8_t
            {
                Any  = -1,
                Same = -2,
                All  = -3
            };

            // Interface methods
            virtual void pinToCore(int coreId) = 0;

            virtual void run() = 0;

            virtual void enqueue(ITask::Ptr task) = 0;

            virtual bool tryEnqueue(ITask::Ptr task) = 0;

            virtual ITask::Ptr dequeue(std::atomic_bool & hint) = 0;

            virtual ITask::Ptr tryDequeue(std::atomic_bool & hint) = 0;

            virtual size_t size() const = 0;

            virtual bool empty() const = 0;

            virtual IQueueStatistics & stats() = 0;

            virtual SpinLock & getLock() = 0;

            virtual void signalEmptyCondition(bool value) = 0;

            virtual bool isIdle() const = 0;

            virtual const std::shared_ptr<std::thread> & getThread() const = 0;

            static void setThreadName(
                QueueType type, std::thread::native_handle_type threadHandle, int queueId, bool shared, bool any);

            static ITask::Ptr getCurrentTask();

            static void setCurrentTask(ITask::Ptr task);

        protected:

            struct TaskSetterGuard
            {
                TaskSetterGuard(IQueue & taskQueue, ITask::Ptr task);
                ~TaskSetterGuard();
                IQueue & _taskQueue;
            };

        private:

            static ITask::Ptr & currentTask();
            LOG4CXX_DECLARE_STATIC_LOGGER
        };

        inline log4cxx::LoggerPtr IQueue::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.interface.IQueue" ) );
        using IQueuePtr = IQueue::Ptr;

        inline void IQueue::setThreadName(
            QueueType type, std::thread::native_handle_type threadHandle, int queueId, bool shared, bool any)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            int idx       = 0;
            char name[16] = {0};
            memcpy(name + idx, "quantum:", 8);
            idx += 8;
            if (type == QueueType::Coro)
            {
                memcpy(name + idx, "co:", 3);
                idx += 3;
                if (shared)
                {
                    memcpy(name + idx, "s:", 2);
                    idx += 2;
                }
                else if (any)
                {
                    memcpy(name + idx, "a:", 2);
                    idx += 2;
                }
            }
            else
            {
                memcpy(name + idx, "io:", 3);
                idx += 3;
            }
            // last 2 digits of the queueId
            name[idx + 1] = '0' + queueId % 10;
            queueId /= 10;
            name[idx] = '0' + queueId % 10;

#if (__GLIBC__ >= 2) && (__GLIBC_MINOR__ >= 12)
            pthread_setname_np(threadHandle, name);
#endif
        }

#ifndef __QUANTUM_USE_DEFAULT_ALLOCATOR
    #ifdef __QUANTUM_ALLOCATE_POOL_FROM_HEAP
        using QueueListAllocator = HeapAllocator<ITask::Ptr>;
    #else
        using QueueListAllocator = StackAllocator<ITask::Ptr, __QUANTUM_QUEUE_LIST_ALLOC_SIZE>;
    #endif
#else
        using QueueListAllocator = StlAllocator<ITask::Ptr>;
#endif

        inline ITask::Ptr & IQueue::currentTask()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            static thread_local ITask::Ptr currentTask;
            return currentTask;
        }

        inline ITask::Ptr IQueue::getCurrentTask()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return currentTask();
        }

        inline void IQueue::setCurrentTask(ITask::Ptr task)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            currentTask() = std::move(task);
        }

        inline IQueue::TaskSetterGuard::TaskSetterGuard(IQueue & taskQueue, ITask::Ptr task)
            : _taskQueue(taskQueue)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            _taskQueue.setCurrentTask(std::move(task));
        }

        inline IQueue::TaskSetterGuard::~TaskSetterGuard()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            _taskQueue.setCurrentTask(nullptr);
        }
}

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_IQUEUE_H
