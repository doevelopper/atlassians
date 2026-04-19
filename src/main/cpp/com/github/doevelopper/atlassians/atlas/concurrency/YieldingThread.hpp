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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_YIELDING_THREAD_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_YIELDING_THREAD_H

#include <chrono>
#include <thread>
#include <com/github/doevelopper/atlassians/atlas/concurrency/ThreadTraits.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                               struct YieldingThreadDuration
        //==============================================================================================
        /*!
         * @struct YieldingThreadDuration.
         * @brief This class provides the same functionality as a coroutine yield when called from a thread context.
         * @note For internal use only.
         */
        template <class DURATION>
        struct YieldingThreadDuration
        {
            /*!
            * @brief Yields the current thread either via a busy wait loop or by sleeping it.
            *       Behavior is determined at compile time.
            * @param[in] time Time used for the sleep duration.
            */
            void operator()(DURATION time = defaultDuration())
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                if (time == DURATION(0))
                {
                    LOG4CXX_DEBUG(logger, "Busy wait");
                    // Busy wait
                    std::this_thread::yield();
                }
                else
                {
                    LOG4CXX_DEBUG(logger, "Sleep wait: " << std::string(typeid(time).name()));
                    // LOG4CXX_DEBUG(logger, "Sleep wait: " << std::string(std::chrono::duration_cast<std::chrono::milliseconds>(time)));
                    // Sleep wait
                    std::this_thread::sleep_for(time);
                }
            }

            static DURATION defaultDuration()
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                return std::chrono::duration_cast<DURATION>(ThreadTraits::yieldSleepIntervalMs())
                     + std::chrono::duration_cast<DURATION>(ThreadTraits::yieldSleepIntervalUs());
            }
            LOG4CXX_DECLARE_STATIC_LOGGER
        };
        template <class DURATION>
        inline log4cxx::LoggerPtr YieldingThreadDuration<DURATION>::logger =
        log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.YieldingThreadDuration" ) );
        using YieldingThread = YieldingThreadDuration<std::chrono::microseconds>;
}

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_YIELDING_THREAD_H
