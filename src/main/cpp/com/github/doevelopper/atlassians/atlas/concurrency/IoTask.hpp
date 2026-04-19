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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_IO_TASK_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_IO_TASK_H

#include <functional>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITask.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Capture.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Promise.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/Util.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                 class IoTask
        //==============================================================================================
        /*!
         * @class IoTask.
         * @brief Long running or blocking task running in the IO thread pool.
         * @note For internal use only.
         */
        class IoTask : public ITask
        {
        public:

            using Ptr     = std::shared_ptr<IoTask>;
            using WeakPtr = std::weak_ptr<IoTask>;

            template <class RET, class FUNC, class... ARGS>
            IoTask(
                std::true_type, std::shared_ptr<Promise<RET>> promise, int queueId, bool isHighPriority, FUNC && func,
                ARGS &&... args);

            template <class RET, class FUNC, class... ARGS>
            IoTask(
                std::false_type, std::shared_ptr<Promise<RET>> promise, int queueId, bool isHighPriority, FUNC && func,
                ARGS &&... args);

            IoTask(const IoTask & task)             = delete;
            IoTask(IoTask && task)                  = default;
            IoTask & operator=(const IoTask & task) = delete;
            IoTask & operator=(IoTask && task)      = default;

            ~IoTask();

            // ITerminate
            void terminate() final;

            // ITask
            int run(const TaskStateHandler & handler, TaskType handledTaskTypes, TaskState handledTaskStates) final;
            void setQueueId(int queueId) final;
            int getQueueId() const final;
            Type getType() const final;
            TaskId getTaskId() const final;
            // Always return false
            bool isBlocked() const final;
            bool isSleeping(bool updateTimer = false) final;
            bool isHighPriority() const final;
            bool isSuspended() const final;
            ITask::LocalStorage & getLocalStorage() final;

            //===================================
            //           NEW / DELETE
            //===================================
            static void * operator new(size_t size);
            static void operator delete(void * p);
            static void deleter(IoTask * p);

        private:

            Function<int()> m_func; // the current runnable io function
            std::atomic_bool m_terminated;
            int m_queueId;
            bool m_isHighPriority;
            TaskId m_taskId;
            ITask::LocalStorage m_localStorage; // local storage of the IO task
            LOG4CXX_DECLARE_STATIC_LOGGER
        };
        inline log4cxx::LoggerPtr IoTask::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.IoTask" ) );
        using IoTaskPtr     = IoTask::Ptr;
        using IoTaskWeakPtr = IoTask::WeakPtr;
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/IoTask.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_IO_TASK_H
