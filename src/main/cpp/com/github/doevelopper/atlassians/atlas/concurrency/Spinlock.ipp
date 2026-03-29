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
#include <chrono>
#include <random>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/SpinlockUtil.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================
        //                                   SpinLock
        //==============================================================================
//        inline SpinLock::SpinLock(SpinLock&& o) : _flag(o._flag.load())
//        {
//        }

//        inline
//        SpinLock& SpinLock::operator=(SpinLock&& o)
//        {
//            if(this != &o)
//            {
//                _flag.store(o._flag.load());
//            }
//            return *this;
//        }
        inline void SpinLock::lock()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            SpinLockUtil::lockWrite(m_flag, LockTraits::Attempt::Unlimited);
        }

        inline bool SpinLock::tryLock()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return SpinLockUtil::lockWrite(m_flag, LockTraits::Attempt::Once);
        }

        inline void SpinLock::unlock()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return SpinLockUtil::unlockWrite(m_flag);
        }

        inline bool SpinLock::isLocked() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return SpinLockUtil::isLocked(m_flag);
        }

        //==============================================================================
        //                            SpinLock::Guard
        //==============================================================================
        inline SpinLock::Guard::Guard(SpinLock & lock)
            : m_spinlock(lock)
            , m_ownsLock(true)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_spinlock.lock();
        }

        inline SpinLock::Guard::Guard(SpinLock & lock, LockTraits::TryToLock)
            : m_spinlock(lock)
            , m_ownsLock(m_spinlock.tryLock())
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline SpinLock::Guard::Guard(SpinLock & lock, LockTraits::AdoptLock)
            : m_spinlock(lock)
            , m_ownsLock(lock.isLocked())
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline SpinLock::Guard::Guard(SpinLock & lock, LockTraits::DeferLock)
            : m_spinlock(lock)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline SpinLock::Guard::~Guard()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_ownsLock)
            {
                m_spinlock.unlock();
            }
        }

        inline bool SpinLock::Guard::tryLock()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(!m_ownsLock);
            m_ownsLock = m_spinlock.tryLock();
            return m_ownsLock;
        }

        inline void SpinLock::Guard::lock()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(!m_ownsLock);
            m_spinlock.lock();
            m_ownsLock = true;
        }

        inline bool SpinLock::Guard::ownsLock() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_ownsLock;
        }

        inline void SpinLock::Guard::unlock()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(m_ownsLock);
            m_spinlock.unlock();
            m_ownsLock = false;
        }

        inline SpinLock::ReverseGuard::ReverseGuard(SpinLock & lock)
            : m_spinlock(lock)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(m_spinlock.isLocked());
            m_spinlock.unlock();
        }

        inline SpinLock::ReverseGuard::~ReverseGuard()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(!m_spinlock.isLocked());
            m_spinlock.lock();
        }
}
