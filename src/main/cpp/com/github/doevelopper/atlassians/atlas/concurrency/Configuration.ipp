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

#include <com/github/doevelopper/atlassians/atlas/concurrency/TaskStateHandler.hpp>

namespace Bloomberg::quantum
{
        inline const std::string & ConfigurationSchemaProvider::getJsonSchema()
        {
            static std::string schema = R"JSON(
    {
        "$schema" : "http://json-schema.org/draft-04/schema#",
        "$id" : "bloomberg:quantum.json",
        "title": "Quantum library settings",
        "type": "object",
        "properties": {
            "numCoroutineThreads": {
                "type": "number",
                "default": -1
            },
            "numIoThreads": {
                "type": "number",
                "default": 5
            },
            "pinToCores": {
                "type": "boolean",
                "default": false
            },
            "loadBalanceSharedIoQueues": {
                "type": "boolean",
                "default": false
            },
            "loadBalancePollIntervalMs": {
                "type": "number",
                "default": 100
            },
            "loadBalancePollIntervalBackoffPolicy": {
                "type": "string",
                "enum": [
                    "exponential",
                    "linear"
                ],
                "default": "linear"
            },
            "loadBalancePollIntervalNumBackoffs": {
                "type": "number",
                "default": 0
            },
            "coroQueueIdRangeForAnyLow": {
                "type": "number",
                "default": -1
            },
            "coroQueueIdRangeForAnyHigh": {
                "type": "number",
                "default": -1
            },
            "coroSharingForAny": {
                "type": "boolean",
                "default": false
            }
        },
        "additionalProperties": false,
        "required": []
    }
    )JSON";
            return schema;
        }

        inline const std::string & ConfigurationSchemaProvider::getJsonSchemaUri()
        {
            static std::string uri = "bloomberg:quantum.json";
            return uri;
        }

        inline Configuration & Configuration::setNumCoroutineThreads(int num)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_numCoroutineThreads = num;
            return *this;
        }

        inline Configuration & Configuration::setNumIoThreads(int num)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_numIoThreads = num;
            return *this;
        }

        inline Configuration & Configuration::setPinCoroutineThreadsToCores(bool value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_pinCoroutineThreadsToCores = value;
            return *this;
        }

        inline Configuration & Configuration::setLoadBalanceSharedIoQueues(bool value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_loadBalanceSharedIoQueues = value;
            return *this;
        }

        inline Configuration & Configuration::setLoadBalancePollIntervalMs(std::chrono::milliseconds interval)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_loadBalancePollIntervalMs = interval;
            return *this;
        }

        inline Configuration & Configuration::setLoadBalancePollIntervalBackoffPolicy(BackoffPolicy policy)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            _loadBalancePollIntervalBackoffPolicy = policy;
            return *this;
        }

        inline Configuration & Configuration::setLoadBalancePollIntervalNumBackoffs(size_t numBackoffs)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_loadBalancePollIntervalNumBackoffs = numBackoffs;
            return *this;
        }

        inline Configuration & Configuration::setCoroQueueIdRangeForAny(
            const std::pair<int, int> & coroQueueIdRangeForAny)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_coroQueueIdRangeForAny = coroQueueIdRangeForAny;
            return *this;
        }

        inline Configuration & Configuration::setCoroutineSharingForAny(bool sharing)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_coroutineSharingForAny = sharing;
            return *this;
        }

        inline Configuration & Configuration::setTaskStateConfiguration(
            const TaskStateConfiguration & TaskStateConfiguration)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_taskStateConfiguration = TaskStateConfiguration;
            return *this;
        }

        inline int Configuration::getNumCoroutineThreads() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_numCoroutineThreads;
        }

        inline int Configuration::getNumIoThreads() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_numIoThreads;
        }

        inline bool Configuration::getPinCoroutineThreadsToCores() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_pinCoroutineThreadsToCores;
        }

        inline bool Configuration::getLoadBalanceSharedIoQueues() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_loadBalanceSharedIoQueues;
        }

        inline std::chrono::milliseconds Configuration::getLoadBalancePollIntervalMs() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_loadBalancePollIntervalMs;
        }

        inline Configuration::BackoffPolicy Configuration::getLoadBalancePollIntervalBackoffPolicy() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return _loadBalancePollIntervalBackoffPolicy;
        }

        inline size_t Configuration::getLoadBalancePollIntervalNumBackoffs() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_loadBalancePollIntervalNumBackoffs;
        }

        inline const std::pair<int, int> & Configuration::getCoroQueueIdRangeForAny() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_coroQueueIdRangeForAny;
        }

        inline bool Configuration::getCoroutineSharingForAny() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_coroutineSharingForAny;
        }

        inline const TaskStateConfiguration & Configuration::getTaskStateConfiguration() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_taskStateConfiguration;
        }
}
