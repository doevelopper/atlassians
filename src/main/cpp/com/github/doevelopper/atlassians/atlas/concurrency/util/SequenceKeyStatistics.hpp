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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_SEQUENCE_KEY_STATISTICS_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_SEQUENCE_KEY_STATISTICS_H

#include <atomic>
#include <memory>
#include <tuple>
#include <vector>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                  class SequenceKeyStatistics
        //==============================================================================================
        /*!
        * @class SequenceKeyStatistics.
        * @brief Implementation of a statistics collection for a SequenceKey in Sequencer
        */
        class SequenceKeyStatistics
        {
        public:

            /*!
             * @brief Constructor.
             */
            SequenceKeyStatistics() = default;

            /*!
             * @brief Constructor.
             */
            SequenceKeyStatistics(const SequenceKeyStatistics & that);

            /*!
             * @brief Constructor.
             */
            SequenceKeyStatistics(SequenceKeyStatistics && that);

            /*!
             * @brief Assignment operator.
             */
            SequenceKeyStatistics & operator=(const SequenceKeyStatistics & that);

            /*!
             * @brief Assignment operator.
             */
            SequenceKeyStatistics & operator=(SequenceKeyStatistics && that);

            /*!
             * @brief Gets the total number of tasks associated with the key that have been posted to the Sequencer
             *      since the sequencer started tracking the key
             * @return the number of tasks
             */
            size_t getPostedTaskCount() const;

            /*!
             * @brief Gets the total number of pending tasks associated with the key
             * @remark A task is pending if the dispatcher has not started it yet
             * @return the number of tasks
             */
            size_t getPendingTaskCount() const;

        protected:

            /*!
             * @brief Number of posted tasks associated with the sequence key
             */
            size_t m_postedTaskCount {0};
            /*!
             * @brief Number of pending tasks associated with the sequence key
             */
            std::atomic<size_t> m_pendingTaskCount {0};

            LOG4CXX_DECLARE_STATIC_LOGGER
        };
        inline log4cxx::LoggerPtr SequenceKeyStatistics::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.util.SequenceKeyStatistics" ) );
        //==============================================================================================
        //                                      class SequenceKeyStatisticsWriter
        //==============================================================================================

        /*!
         * @class SequenceKeyStatistics.
         * @brief Implementation of a writer for the SequenceKeyStatistics
         */
        class SequenceKeyStatisticsWriter : public SequenceKeyStatistics
        {
        public:

            /*!
             *  @brief Increments the total number of tasks associated with the key that have been posted to the
             *     Sequencer since the sequencer started tracking the key
             */
            void incrementPostedTaskCount();

            /*!
             * @brief Increments the total number of pending tasks associated with the key
             */
            void incrementPendingTaskCount();

            /*!
             * @brief Increments the total number of pending tasks associated with the key
             */
            void decrementPendingTaskCount();
            LOG4CXX_DECLARE_STATIC_LOGGER
        };
        inline log4cxx::LoggerPtr SequenceKeyStatisticsWriter::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.util.SequenceKeyStatisticsWriter" ) );

}

#include <com/github/doevelopper/atlassians/atlas/concurrency/util/SequenceKeyStatistics.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_SEQUENCE_KEY_STATISTICS_H
