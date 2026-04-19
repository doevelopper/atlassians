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
/*
The atomic 32-bit counter is divided into two halves, the high 16 bits representing the number
of pending writer upgrades and the low 16 bit representing the lock state as following:
   0 (unlocked)
   -1 (write-locked)
   >0 (read-locked and the number represents the number of readers)

When a reader is upgraded to a writer, the number of readers is decremented by 1 and the number
of pending writers is incremented by 1 if the upgrade can't happen immediately. If however
there's only a single reader, the upgrade occurs immediately and the number of readers (i.e. 1)
gets converted directly to a writer (i.e. -1).

State transitions:
H|L represent the high (pending upgrades) and low (reader/writer owners) portions of
the 32-bit counter.

Reader Lock
1) 0|L -> 0|L+1 where L>=0

Reader Unlock
1) H|L -> H|L-1

Reader Upgrade
1) H|1 -> H|-1 (direct upgrade)
2) H|L -> H+1|L-1 then goto Writer (2)

Reader blocks when
1) H|-1
2) H|L -> where H>0

Writer Lock
1) H|0 -> H|-1 where H>=0 (regular writer)
2) H|0 -> H-1|-1 where H>0 (upgraded writer)

Writer Unlock
1) H|-1 -> H|0

Writer blocks when
1) H|-1
2) H|L where L>0
*/

#include <com/github/doevelopper/atlassians/atlas/concurrency/util/SpinlockUtil.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                ReadWriteSpinLock
        //==============================================================================================
        inline void ReadWriteSpinLock::lockRead()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            SpinLockUtil::lockRead(m_count, LockTraits::Attempt::Unlimited);
        }

        inline void ReadWriteSpinLock::lockWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            SpinLockUtil::lockWrite(m_count, LockTraits::Attempt::Unlimited);
        }

        inline bool ReadWriteSpinLock::tryLockRead()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return SpinLockUtil::lockRead(m_count, LockTraits::Attempt::Once);
        }

        inline bool ReadWriteSpinLock::tryLockWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return SpinLockUtil::lockWrite(m_count, LockTraits::Attempt::Once);
        }

        inline void ReadWriteSpinLock::unlockRead()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            SpinLockUtil::unlockRead(m_count);
        }

        inline void ReadWriteSpinLock::unlockWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            SpinLockUtil::unlockWrite(m_count);
        }

        inline void ReadWriteSpinLock::upgradeToWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            SpinLockUtil::upgradeToWrite(m_count, LockTraits::Attempt::Unlimited);
        }

        inline bool ReadWriteSpinLock::tryUpgradeToWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return SpinLockUtil::upgradeToWrite(m_count, LockTraits::Attempt::Once);
        }

        inline bool ReadWriteSpinLock::tryUpgradeToWrite(bool & pendingUpgrade)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return SpinLockUtil::upgradeToWrite(m_count, pendingUpgrade, LockTraits::Attempt::Reentrant);
        }

        inline bool ReadWriteSpinLock::isLocked() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return SpinLockUtil::isLocked(m_count);
        }

        inline bool ReadWriteSpinLock::isReadLocked() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return isLocked() && !isWriteLocked();
        }

        inline bool ReadWriteSpinLock::isWriteLocked() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return SpinLockUtil::isWriteLocked(m_count);
        }

        inline int ReadWriteSpinLock::numReaders() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return SpinLockUtil::numReaders(m_count);
        }

        inline int ReadWriteSpinLock::numPendingWriters() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return SpinLockUtil::numPendingWriters(m_count);
        }

        //==============================================================================================
        //                                ReadWriteSpinLock::Guard
        //==============================================================================================
        inline ReadWriteSpinLock::Guard::Guard(ReadWriteSpinLock & lock, LockTraits::AcquireRead)
            : m_spinlock(lock)
            , m_ownsLock(true)
            , m_isUpgraded(false)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_spinlock.lockRead();
        }

        inline ReadWriteSpinLock::Guard::Guard(ReadWriteSpinLock & lock, LockTraits::AcquireWrite)
            : m_spinlock(lock)
            , m_ownsLock(true)
            , m_isUpgraded(true)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_spinlock.lockWrite();
        }

        inline ReadWriteSpinLock::Guard::Guard(ReadWriteSpinLock & lock, LockTraits::AcquireRead, LockTraits::TryToLock)
            : m_spinlock(lock)
            , m_ownsLock(m_spinlock.tryLockRead())
            , m_isUpgraded(false)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline ReadWriteSpinLock::Guard::Guard(
            ReadWriteSpinLock & lock, LockTraits::AcquireWrite, LockTraits::TryToLock)
            : m_spinlock(lock)
            , m_ownsLock(m_spinlock.tryLockWrite())
            , m_isUpgraded(m_ownsLock)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline ReadWriteSpinLock::Guard::Guard(ReadWriteSpinLock & lock, LockTraits::AdoptLock)
            : m_spinlock(lock)
            , m_ownsLock(lock.isLocked())
            , m_isUpgraded(lock.isWriteLocked())
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline ReadWriteSpinLock::Guard::Guard(ReadWriteSpinLock & lock, LockTraits::DeferLock)
            : m_spinlock(lock)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline ReadWriteSpinLock::Guard::~Guard()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (ownsLock())
            {
                unlock();
            }
        }

        inline void ReadWriteSpinLock::Guard::lockRead()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(!ownsLock());
            m_spinlock.lockRead();
            m_ownsLock   = true;
            m_isUpgraded = false;
        }

        inline void ReadWriteSpinLock::Guard::lockWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(!ownsLock());
            m_spinlock.lockWrite();
            m_ownsLock = m_isUpgraded = true;
        }

        inline bool ReadWriteSpinLock::Guard::tryLockRead()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(!ownsLock());
            m_ownsLock   = m_spinlock.tryLockRead();
            m_isUpgraded = false;
            return m_ownsLock;
        }

        inline bool ReadWriteSpinLock::Guard::tryLockWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(!ownsLock());
            m_ownsLock = m_isUpgraded = m_spinlock.tryLockWrite();
            return m_ownsLock;
        }

        inline void ReadWriteSpinLock::Guard::upgradeToWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(ownsLock());
            m_spinlock.upgradeToWrite();
            m_isUpgraded = true;
        }

        inline bool ReadWriteSpinLock::Guard::tryUpgradeToWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(ownsLock());
            m_isUpgraded = m_spinlock.tryUpgradeToWrite();
            return m_isUpgraded;
        }

        inline bool ReadWriteSpinLock::Guard::ownsLock() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_ownsLock;
        }

        inline bool ReadWriteSpinLock::Guard::ownsReadLock() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_ownsLock && !m_isUpgraded;
        }

        inline bool ReadWriteSpinLock::Guard::ownsWriteLock() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_ownsLock && m_isUpgraded;
        }

        inline void ReadWriteSpinLock::Guard::unlock()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(ownsLock());
            if (ownsReadLock())
            {
                m_spinlock.unlockRead();
            }
            else
            {
                m_spinlock.unlockWrite();
            }
            m_ownsLock = m_isUpgraded = false;
        }
}
