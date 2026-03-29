/*
** Copyright 2022 Bloomberg Finance L.P.
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

#include <com/github/doevelopper/atlassians/atlas/concurrency/Auxiliary.hpp>

namespace Bloomberg::quantum
{
        inline TaskStateConfiguration & TaskStateConfiguration::setTaskStateHandler(const TaskStateHandler & handler)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_handlerm = handler;
            return *this;
        }

        inline TaskStateConfiguration & TaskStateConfiguration::setHandledTaskStates(TaskState states)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_handledStates = states;
            return *this;
        }

        inline TaskStateConfiguration & TaskStateConfiguration::setHandledTaskTypes(TaskType types)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_handledTypes = types;
            return *this;
        }

        inline const TaskStateHandler & TaskStateConfiguration::getTaskStateHandler() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_handlerm;
        }

        inline TaskState TaskStateConfiguration::getHandledTaskStates() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_handledStates;
        }

        inline TaskType TaskStateConfiguration::getHandledTaskTypes() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_handledTypes;
        }

        inline bool isValidTaskStateOrded(TaskState currentState, TaskState nextState)
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            switch (nextState)
            {
            case TaskState::Started:
                return TaskState::Initialized == currentState;

            case TaskState::Suspended:
                return TaskState::Started == currentState || TaskState::Resumed == currentState;

            case TaskState::Resumed:
                return TaskState::Suspended == currentState;

            case TaskState::Stopped:
                return TaskState::Started == currentState || TaskState::Resumed == currentState;

            case TaskState::Initialized:
            case TaskState::None:
                return false;

            default:
                return false;
            }
        }

        inline void handleTaskState(
            const TaskStateHandler & stateHandler, size_t taskId, int queueId, TaskType handledType,
            TaskState handledStates, TaskState nextState, TaskState & currentState)
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (not isValidTaskStateOrded(currentState, nextState))
            {
#ifdef __QUANTUM_PRINT_DEBUG
                std::lock_guard<std::mutex> guard(Util::LogMutex());
                std::cerr << "Invalid task state order"
                          << ", task id: " << taskId
                          << ", next task state: " << std::to_string(static_cast<int>(nextState))
                          << ", current task state: " << std::to_string(static_cast<int>(currentState)) << std::endl;
                assert(false);
#endif
                return;
            }
            currentState = nextState;

            if (not stateHandler)
            {
                return;
            }

            if (not isIntersection(handledStates, nextState))
            {
                return;
            }

            stateHandler(taskId, queueId, handledType, nextState);
        }
}
