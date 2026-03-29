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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_ITASK_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_ITASK_H

#include <limits>
#include <memory>
#include <unordered_map>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITerminate.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/TaskId.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/TaskStateHandler.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Traits.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                  interface ITask
        //==============================================================================================
        /// @interface ITask
        /// @brief Interface to a task. For internal use only.
        struct ITask : public ITerminate
        {
            using Ptr          = std::shared_ptr<ITask>;
            using WeakPtr      = std::weak_ptr<ITask>;
            using LocalStorage = std::unordered_map<std::string, void *>;

            enum class Type : std::int8_t
            {
                Standalone,
                First,
                Continuation,
                ErrorHandler,
                Final,
                Termination,
                IO
            };

            enum class RetCode : std::int8_t
            {
                Success        = 0, ///< Coroutine finished successfully
                Running        = std::numeric_limits<std::int8_t>::max(), ///< Coroutine is still active
                AlreadyResumed = static_cast<int>(Running - 1), ///< Coroutine is running on a different thread
                Exception      = static_cast<int>(Running - 2), ///< Coroutine ended in an exception
                NotCallable    = static_cast<int>(Running - 3), ///< Coroutine cannot be called
                Blocked        = static_cast<int>(Running - 4), ///< Coroutine is blocked
                Sleeping       = static_cast<int>(Running - 5), ///< Coroutine is sleeping
                Max            = static_cast<int>(Running - 10), ///< Value of the max reserved return code
            };

            ~ITask() = default;

            virtual int run(
                const TaskStateHandler & stateHandler, TaskType handledTaskTypes, TaskState handledTaskStates) = 0;

            virtual void setQueueId(int queueId) = 0;

            virtual int getQueueId() const = 0;

            virtual TaskId getTaskId() const = 0;

            virtual Type getType() const = 0;

            virtual bool isBlocked() const = 0;

            virtual bool isSleeping(bool updateTimer) = 0;

            virtual bool isHighPriority() const = 0;

            virtual bool isSuspended() const = 0;

            virtual LocalStorage & getLocalStorage() = 0;
            LOG4CXX_DECLARE_STATIC_LOGGER
        };
        inline log4cxx::LoggerPtr ITask::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.ITask" ) );
        using ITaskPtr     = ITask::Ptr;
        using ITaskWeakPtr = ITask::WeakPtr;
}

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_ITASK_H
