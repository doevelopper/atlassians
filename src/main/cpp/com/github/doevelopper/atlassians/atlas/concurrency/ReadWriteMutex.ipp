/*
** Copyright 2020 Bloomberg Finance L.P.
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
#include <cassert>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Local.hpp>

namespace Bloomberg::quantum
{
        void yield(ICoroSync::Ptr sync);

        //==============================================================================================
        //                                 ReadWriteMutex
        //==============================================================================================
        inline void ReadWriteMutex::lockRead()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // Application must use the other lockRead() overload if we are running inside a coroutine
            assert(!local::context());
            lockRead(nullptr);
        }

        inline void ReadWriteMutex::lockRead(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            while (!tryLockRead())
            {
                yield(sync);
            }
        }

        inline void ReadWriteMutex::lockWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // Application must use the other lockWrite() overload if we are running inside a coroutine
            assert(!local::context());
            lockWrite(nullptr);
        }

        inline void ReadWriteMutex::lockWrite(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            while (!tryLockWrite())
            {
                yield(sync);
            }
        }

        inline bool ReadWriteMutex::tryLockRead()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_spinlock.tryLockRead();
        }

        inline bool ReadWriteMutex::tryLockWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            bool rc = m_spinlock.tryLockWrite();
            if (rc)
            {
                // mutex is write-locked
                m_taskId = local::taskId();
                // task id must be valid
                assert(m_taskId != TaskId {});
            }
            return rc;
        }

        inline void ReadWriteMutex::upgradeToWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // Application must use the other upgradeToWrite() overload if we are running inside a coroutine
            assert(!local::context());
            upgradeToWrite(nullptr);
        }

        inline void ReadWriteMutex::upgradeToWrite(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            bool pendingUpgrade = false;
            while (!tryUpgradeToWriteImpl(&pendingUpgrade))
            {
                yield(sync);
            }
        }

        inline bool ReadWriteMutex::tryUpgradeToWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return tryUpgradeToWriteImpl(nullptr);
        }

        inline bool ReadWriteMutex::tryUpgradeToWriteImpl(bool * pendingUpgrade)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            bool rc;
            if (pendingUpgrade)
            {
                rc = m_spinlock.tryUpgradeToWrite(*pendingUpgrade);
            }
            else
            {
                rc = m_spinlock.tryUpgradeToWrite();
            }
            if (rc)
            {
                m_taskId = local::taskId();
            }
            return rc;
        }

        inline void ReadWriteMutex::unlockRead()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_spinlock.unlockRead();
        }

        inline void ReadWriteMutex::unlockWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(m_taskId == local::taskId());
            m_taskId = TaskId {}; // reset the task id
            m_spinlock.unlockWrite();
        }

        inline bool ReadWriteMutex::isLocked() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_spinlock.isLocked();
        }

        inline bool ReadWriteMutex::isReadLocked() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_spinlock.isReadLocked();
        }

        inline bool ReadWriteMutex::isWriteLocked() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_spinlock.isWriteLocked();
        }

        inline int ReadWriteMutex::numReaders() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_spinlock.numReaders();
        }

        inline int ReadWriteMutex::numPendingWriters() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_spinlock.numPendingWriters();
        }

        //==============================================================================================
        //                                 ReadWriteMutex::Guard
        //==============================================================================================
        inline ReadWriteMutex::Guard::Guard(ReadWriteMutex & lock, LockTraits::AcquireRead acquire)
            : ReadWriteMutex::Guard::Guard(nullptr, lock, acquire)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // Application must use the other constructor overload if we are running inside a coroutine
            assert(!local::context());
        }

        inline ReadWriteMutex::Guard::Guard(ReadWriteMutex & lock, LockTraits::AcquireWrite acquire)
            : ReadWriteMutex::Guard::Guard(nullptr, lock, acquire)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // Application must use the other constructor overload if we are running inside a coroutine
            assert(!local::context());
        }

        inline ReadWriteMutex::Guard::Guard(ICoroSync::Ptr sync, ReadWriteMutex & lock, LockTraits::AcquireRead)
            : m_mutex(&lock)
            , m_ownsLock(true)
            , m_isUpgraded(false)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_mutex->lockRead(sync);
        }

        inline ReadWriteMutex::Guard::Guard(ICoroSync::Ptr sync, ReadWriteMutex & lock, LockTraits::AcquireWrite)
            : m_mutex(&lock)
            , m_ownsLock(true)
            , m_isUpgraded(true)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_mutex->lockWrite(sync);
        }

        inline ReadWriteMutex::Guard::Guard(ReadWriteMutex & lock, LockTraits::AcquireRead, LockTraits::TryToLock)
            : m_mutex(&lock)
            , m_ownsLock(m_mutex->tryLockRead())
            , m_isUpgraded(false)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline ReadWriteMutex::Guard::Guard(ReadWriteMutex & lock, LockTraits::AcquireWrite, LockTraits::TryToLock)
            : m_mutex(&lock)
            , m_ownsLock(m_mutex->tryLockWrite())
            , m_isUpgraded(m_ownsLock)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline ReadWriteMutex::Guard::Guard(ReadWriteMutex & lock, LockTraits::AdoptLock)
            : m_mutex(&lock)
            , m_ownsLock(lock.isLocked())
            , m_isUpgraded(lock.isWriteLocked())
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline ReadWriteMutex::Guard::Guard(ReadWriteMutex & lock, LockTraits::DeferLock)
            : m_mutex(&lock)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        inline ReadWriteMutex::Guard::~Guard()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (ownsLock())
            {
                unlock();
            }
        }

        inline void ReadWriteMutex::Guard::lockRead()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // Application must use the other lock() overload if we are running inside a coroutine
            assert(!local::context());
            lockRead(nullptr);
        }

        inline void ReadWriteMutex::Guard::lockWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // Application must use the other lock() overload if we are running inside a coroutine
            assert(!local::context());
            lockWrite(nullptr);
        }

        inline void ReadWriteMutex::Guard::lockRead(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(m_mutex && !ownsLock());
            m_mutex->lockRead(sync);
            m_ownsLock   = true;
            m_isUpgraded = false;
        }

        inline void ReadWriteMutex::Guard::lockWrite(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(m_mutex && !ownsLock());
            m_mutex->lockWrite(sync);
            m_ownsLock = m_isUpgraded = true;
        }

        inline bool ReadWriteMutex::Guard::tryLockRead()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(m_mutex && !ownsLock());
            m_ownsLock = m_mutex->tryLockRead();
            return m_ownsLock;
        }

        inline bool ReadWriteMutex::Guard::tryLockWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(m_mutex && !ownsLock());
            m_ownsLock = m_isUpgraded = m_mutex->tryLockWrite();
            return m_ownsLock;
        }

        inline void ReadWriteMutex::Guard::upgradeToWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // Application must use the other upgradeToWrite() overload if we are running inside a coroutine
            assert(!local::context());
            upgradeToWrite(nullptr);
        }

        inline void ReadWriteMutex::Guard::upgradeToWrite(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(m_mutex && ownsReadLock());
            m_mutex->upgradeToWrite(sync);
            m_isUpgraded = true;
        }

        inline bool ReadWriteMutex::Guard::tryUpgradeToWrite()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(m_mutex && ownsReadLock());
            m_isUpgraded = m_mutex->tryUpgradeToWrite();
            return m_isUpgraded;
        }

        inline void ReadWriteMutex::Guard::unlock()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(m_mutex && ownsLock());
            if (ownsReadLock())
            {
                m_mutex->unlockRead();
            }
            else
            {
                m_mutex->unlockWrite();
            }
            m_ownsLock = m_isUpgraded = false;
        }

        inline void ReadWriteMutex::Guard::release()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_ownsLock = m_isUpgraded = false;
            m_mutex                  = nullptr;
        }

        inline bool ReadWriteMutex::Guard::ownsLock() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_ownsLock;
        }

        inline bool ReadWriteMutex::Guard::ownsReadLock() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_ownsLock && !m_isUpgraded;
        }

        inline bool ReadWriteMutex::Guard::ownsWriteLock() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_ownsLock && m_isUpgraded;
        }
}
