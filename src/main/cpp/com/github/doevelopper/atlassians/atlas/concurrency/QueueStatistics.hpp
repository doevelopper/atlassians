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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_QUEUE_STATISTICS_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_QUEUE_STATISTICS_H

#include <atomic>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IQueueStatistics.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                       class QueueStatistics
        //==============================================================================================
        /*!
         * @class QueueStatistics.
         * @brief Provides various counters related to queues and task execution.
         * @note See IQueueStatistics for detailed description.
         */
        class QueueStatistics : public IQueueStatistics
        {
            friend class TaskQueue;
            friend class IOQueue;

        public:

            QueueStatistics();
            QueueStatistics(const QueueStatistics &);

            //===================================
            //         IQUEUESTATISTICS
            //===================================
            void reset() final;

            size_t numElements() const final;

            void incNumElements() final;

            void decNumElements() final;

            size_t errorCount() const final;

            void incErrorCount() final;

            size_t sharedQueueErrorCount() const final;

            void incSharedQueueErrorCount() final;

            size_t completedCount() const final;

            void incCompletedCount() final;

            size_t sharedQueueCompletedCount() const final;

            void incSharedQueueCompletedCount() final;

            size_t postedCount() const final;

            void incPostedCount() final;

            size_t highPriorityCount() const final;

            void incHighPriorityCount() final;

            void print(std::ostream & out) const final;

            QueueStatistics & operator+=(const IQueueStatistics & rhs);

            friend QueueStatistics operator+(QueueStatistics lhs, const IQueueStatistics & rhs);

        private:

            std::atomic_size_t m_numElements;
            size_t m_errorCount;
            size_t m_sharedQueueErrorCount;
            size_t m_completedCount;
            size_t m_sharedQueueCompletedCount;
            size_t m_postedCount;
            size_t m_highPriorityCount;

            LOG4CXX_DECLARE_STATIC_LOGGER
        };
    inline log4cxx::LoggerPtr QueueStatistics::logger =
        log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.QueueStatistics" ) );
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/QueueStatistics.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_QUEUE_STATISTICS_H
