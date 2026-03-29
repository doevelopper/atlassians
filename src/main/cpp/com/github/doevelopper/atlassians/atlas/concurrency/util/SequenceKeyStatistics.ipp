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

namespace Bloomberg::quantum
{
        inline SequenceKeyStatistics::SequenceKeyStatistics(const SequenceKeyStatistics & that)
            : m_postedTaskCount(that.m_postedTaskCount)
            , m_pendingTaskCount(that.m_pendingTaskCount.load())
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline SequenceKeyStatistics::SequenceKeyStatistics(SequenceKeyStatistics && that)
            : m_postedTaskCount(std::move(that.m_postedTaskCount))
            , m_pendingTaskCount(that.m_pendingTaskCount.load())
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline SequenceKeyStatistics & SequenceKeyStatistics::operator=(SequenceKeyStatistics && that)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_postedTaskCount  = std::move(that.m_postedTaskCount);
            m_pendingTaskCount = that.m_pendingTaskCount.load();
            return *this;
        }

        inline SequenceKeyStatistics & SequenceKeyStatistics::operator=(const SequenceKeyStatistics & that)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_postedTaskCount  = that.m_postedTaskCount;
            m_pendingTaskCount = m_pendingTaskCount.load();
            return *this;
        }

        inline size_t SequenceKeyStatistics::getPostedTaskCount() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_postedTaskCount;
        }

        inline size_t SequenceKeyStatistics::getPendingTaskCount() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_pendingTaskCount;
        }

        inline void SequenceKeyStatisticsWriter::incrementPostedTaskCount()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            ++m_postedTaskCount;
        }

        inline void SequenceKeyStatisticsWriter::incrementPendingTaskCount()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            ++m_pendingTaskCount;
        }

        inline void SequenceKeyStatisticsWriter::decrementPendingTaskCount()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            --m_pendingTaskCount;
        }
}
