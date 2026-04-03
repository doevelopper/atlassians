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
        thread_local static std::atomic_int s_threadSignal {-1}; // thread specific (non-coroutine)

        inline ConditionVariable::ConditionVariable()
            : m_destroyed(false)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline ConditionVariable::~ConditionVariable()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            Mutex::Guard lock(local::context(), m_thisLock);
            m_destroyed = true;
        }

        inline void ConditionVariable::notifyOne()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            notifyOne(nullptr);
        }

        inline void ConditionVariable::notifyOne(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // LOCKED OR UNLOCKED SCOPE
            Mutex::Guard lock(sync, m_thisLock);
            if (m_waiters.empty())
            {
                return;
            }
            (*m_waiters.front()) = 1;
            m_waiters.pop_front();
        }

        inline void ConditionVariable::notifyAll()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            notifyAll(nullptr);
        }

        inline void ConditionVariable::notifyAll(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // LOCKED OR UNLOCKED SCOPE
            Mutex::Guard lock(sync, m_thisLock);
            for (auto && waiter : m_waiters)
            {
                (*waiter) = 1;
            }
            m_waiters.clear();
        }

        inline void ConditionVariable::wait(Mutex & mutex)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            wait(nullptr, mutex);
        }

        inline void ConditionVariable::wait(ICoroSync::Ptr sync, Mutex & mutex)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            waitImpl(sync, mutex);
        }

        template <class PREDICATE>
        void ConditionVariable::wait(Mutex & mutex, PREDICATE predicate)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            wait(nullptr, mutex, std::move(predicate));
        }

        template <class PREDICATE>
        void ConditionVariable::wait(ICoroSync::Ptr sync, Mutex & mutex, PREDICATE predicate)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            waitImpl(sync, mutex, std::move(predicate));
        }

        template <class REP, class PERIOD>
        bool ConditionVariable::waitFor(Mutex & mutex, const std::chrono::duration<REP, PERIOD> & time)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return waitFor(nullptr, mutex, time);
        }

        template <class REP, class PERIOD>
        bool
        ConditionVariable::waitFor(ICoroSync::Ptr sync, Mutex & mutex, const std::chrono::duration<REP, PERIOD> & time)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (time == std::chrono::milliseconds(-1))
            {
                waitImpl(sync, mutex);
                return true;
            }
            return waitForImpl(sync, mutex, time);
        }

        template <class REP, class PERIOD, class PREDICATE>
        bool
        ConditionVariable::waitFor(Mutex & mutex, const std::chrono::duration<REP, PERIOD> & time, PREDICATE predicate)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return waitFor(nullptr, mutex, time, predicate);
        }

        template <class REP, class PERIOD, class PREDICATE>
        bool ConditionVariable::waitFor(
            ICoroSync::Ptr sync, Mutex & mutex, const std::chrono::duration<REP, PERIOD> & time, PREDICATE predicate)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (time == std::chrono::milliseconds(-1))
            {
                waitImpl(sync, mutex, predicate);
                return true;
            }
            return waitForImpl(sync, mutex, time, std::move(predicate));
        }

        inline void ConditionVariable::waitImpl(ICoroSync::Ptr sync, Mutex & mutex)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            std::atomic_int & signal = sync ? sync->signal() : s_threadSignal;
            { //========= LOCKED SCOPE =========
                Mutex::Guard lock(sync, m_thisLock);
                if (m_destroyed)
                {
                    // don't release 'mutex' which is locked at this point
                    return;
                }
                signal = 0; // clear signal flag
                m_waiters.push_back(&signal);
            }
            //========= UNLOCKED SCOPE =========
            Mutex::ReverseGuard unlock(sync, mutex);
            while ((signal == 0) && !m_destroyed)
            {
                // wait for signal
                yield(sync);
            }
            signal = -1; // reset
        }

        template <class PREDICATE>
        void ConditionVariable::waitImpl(ICoroSync::Ptr sync, Mutex & mutex, PREDICATE predicate)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // see: https://en.cppreference.com/w/cpp/thread/condition_variable/wait
            while (!predicate() && !m_destroyed)
            {
                waitImpl(sync, mutex);
            }
        }

        template <class REP, class PERIOD>
        bool ConditionVariable::waitForImpl(
            ICoroSync::Ptr sync, Mutex & mutex, const std::chrono::duration<REP, PERIOD> & time)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (time < std::chrono::milliseconds::zero())
            {
                // invalid time setting
                throw std::invalid_argument("Timeout cannot be negative");
            }
            std::atomic_int & signal = sync ? sync->signal() : s_threadSignal;
            { //========= LOCKED SCOPE =========
                Mutex::Guard lock(sync, m_thisLock);
                if (m_destroyed)
                {
                    // don't release 'mutex' which is locked at this point
                    return true;
                }
                if (time == std::chrono::duration<REP, PERIOD>::zero())
                {
                    // immediate timeout.
                    // reset flag if necessary and return w/o releasing mutex.
                    int expected = 1;
                    signal.compare_exchange_strong(expected, -1);
                    return (expected == 1);
                }
                signal = 0; // clear signal flag
                m_waiters.push_back(&signal);
            }
            //========= UNLOCKED SCOPE =========
            Mutex::ReverseGuard unlock(sync, mutex);
            auto start   = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration<REP, PERIOD>::zero();

            // wait until signalled or times out
            while ((signal == 0) && !m_destroyed)
            {
                // wait for signal
                yield(sync);
                elapsed = std::chrono::duration_cast<std::chrono::duration<REP, PERIOD>>(
                    std::chrono::steady_clock::now() - start);
                if (elapsed >= time)
                {
                    break; // expired
                }
            }
            bool rc = (signal == 1);
            signal  = -1; // reset signal flag
            return rc;
        }

        template <class REP, class PERIOD, class PREDICATE>
        bool ConditionVariable::waitForImpl(
            ICoroSync::Ptr sync, Mutex & mutex, const std::chrono::duration<REP, PERIOD> & time, PREDICATE predicate)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // see: https://en.cppreference.com/w/cpp/thread/condition_variable/wait_until
            while (!predicate() && !m_destroyed)
            {
                if (!waitForImpl(sync, mutex, time))
                {
                    // timeout
                    return predicate();
                }
            }
            return true;
        }
}
