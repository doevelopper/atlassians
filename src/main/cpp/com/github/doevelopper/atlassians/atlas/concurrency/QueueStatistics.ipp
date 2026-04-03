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
        inline QueueStatistics::QueueStatistics()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            reset();
        }

        inline QueueStatistics::QueueStatistics(const QueueStatistics & other)
            : m_numElements(other.numElements())
            , m_errorCount(other.m_errorCount)
            , m_sharedQueueErrorCount(other.m_sharedQueueErrorCount)
            , m_completedCount(other.m_completedCount)
            , m_sharedQueueCompletedCount(other.m_sharedQueueCompletedCount)
            , m_postedCount(other.m_postedCount)
            , m_highPriorityCount(other.m_highPriorityCount)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline void QueueStatistics::reset()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_numElements               = 0;
            m_errorCount                = 0;
            m_sharedQueueErrorCount     = 0;
            m_completedCount            = 0;
            m_sharedQueueCompletedCount = 0;
            m_postedCount               = 0;
            m_highPriorityCount         = 0;
        }

        inline size_t QueueStatistics::numElements() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_numElements;
        }

        inline void QueueStatistics::incNumElements()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            ++m_numElements;
        }

        inline void QueueStatistics::decNumElements()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            size_t oldValue = 1;
            size_t newValue = 0;
            while (!m_numElements.compare_exchange_weak(oldValue, newValue, std::memory_order_acq_rel))
            {
                if (oldValue == 0)
                {
                    break;
                }
                else
                {
                    newValue = oldValue - 1;
                }
            }
        }

        inline size_t QueueStatistics::errorCount() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_errorCount;
        }

        inline void QueueStatistics::incErrorCount()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            ++m_errorCount;
        }

        inline size_t QueueStatistics::sharedQueueErrorCount() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_sharedQueueErrorCount;
        }

        inline void QueueStatistics::incSharedQueueErrorCount()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            ++m_sharedQueueErrorCount;
        }

        inline size_t QueueStatistics::completedCount() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_completedCount;
        }

        inline void QueueStatistics::incCompletedCount()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            ++m_completedCount;
        }

        inline size_t QueueStatistics::sharedQueueCompletedCount() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_sharedQueueCompletedCount;
        }

        inline void QueueStatistics::incSharedQueueCompletedCount()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            ++m_sharedQueueCompletedCount;
        }

        inline size_t QueueStatistics::postedCount() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_postedCount;
        }

        inline void QueueStatistics::incPostedCount()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            ++m_postedCount;
        }

        inline size_t QueueStatistics::highPriorityCount() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_highPriorityCount;
        }

        inline void QueueStatistics::incHighPriorityCount()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            ++m_highPriorityCount;
        }

        inline void QueueStatistics::print(std::ostream & out) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            out << "Num active: " << m_numElements << std::endl;
            out << "Num completed: " << m_completedCount << std::endl;
            out << "Num shared completed: " << m_sharedQueueCompletedCount << std::endl;
            out << "Num errors: " << m_errorCount << std::endl;
            out << "Num shared errors: " << m_sharedQueueErrorCount << std::endl;
            out << "Num high-priority count: " << m_highPriorityCount << std::endl;
        }

        inline QueueStatistics & QueueStatistics::operator+=(const IQueueStatistics & rhs)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_numElements += rhs.numElements();
            m_errorCount += rhs.errorCount();
            m_sharedQueueErrorCount += rhs.sharedQueueErrorCount();
            m_completedCount += rhs.completedCount();
            m_sharedQueueCompletedCount += rhs.sharedQueueCompletedCount();
            m_postedCount += rhs.postedCount();
            m_highPriorityCount += rhs.highPriorityCount();
            return *this;
        }

        inline QueueStatistics operator+(QueueStatistics lhs, const IQueueStatistics & rhs)
        {
            lhs += rhs;
            return lhs;
        }

        inline std::ostream & operator<<(std::ostream & out, const IQueueStatistics & stats)
        {
            stats.print(out);
            return out;
        }
}
