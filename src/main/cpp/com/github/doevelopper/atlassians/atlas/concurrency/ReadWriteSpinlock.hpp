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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_READ_WRITE_SPINLOCK_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_READ_WRITE_SPINLOCK_H

#include <atomic>
#include <mutex>
#include <com/github/doevelopper/atlassians/atlas/concurrency/SpinlockTraits.hpp>

namespace Bloomberg::quantum
{
        class ReadWriteSpinLock
        {
        public:

            /*!
            * @brief Spinlock is in unlocked state
            */
            ReadWriteSpinLock() = default;

            /*!
            * @brief Copy constructor.
            */
            ReadWriteSpinLock(const ReadWriteSpinLock &) = delete;

            /*!
            * @brief Move constructor.
            */
            ReadWriteSpinLock(ReadWriteSpinLock &&) = default;

            /*!
            * @brief Copy assignment operator.
            */
            ReadWriteSpinLock & operator=(const ReadWriteSpinLock &) = delete;

            /*!
            * @brief Move assignment operator.
            */
            ReadWriteSpinLock & operator=(ReadWriteSpinLock &&) = default;

            /*!
            * @brief Lock this object as a reader (shared with other readers)
            */
            void lockRead();

            /*!
            * @brief Lock this object as a writer (exclusive)
            */
            void lockWrite();

            /*!
            * @brief Attempts to lock this object as a reader (shared with other readers). This operation never
            *   blocks.
            * @return True if the lock operation succeeded, false otherwise.
            */
            bool tryLockRead();

            /*!
            * @brief Attempts to lock this object as a writer (exclusive). This operation never blocks.
            * @return True if the lock operation succeeded, false otherwise.
            */
            bool tryLockWrite();

            /*!
            * @brief Unlocks the reader lock.
            * @warning Locking this object as a writer and incorrectly unlocking it as a reader results in undefined
            *      behavior.
            */
            void unlockRead();

            /*!
            * @brief Unlocks the writer lock.
            * @warning Locking this object as a reader and incorrectly unlocking it as a writer results in undefined
            *       behavior.
            */
            void unlockWrite();

            /*!
            * @brief Atomically upgrades a Reader to a Writer.
            * @warning Calling then while not owning the read lock results in undefined behavior.
            */
            void upgradeToWrite();

            /*!
            * @brief Attempts to upgrade a reader lock to a writer lock. This operation never blocks.
            * @param pendingUpdate Use this overload when calling this function in a loop.
            *      PendingUpdate must be initialized to 'false'.
            * @return True if the lock operation succeeded, false otherwise.
            */
            bool tryUpgradeToWrite();
            bool tryUpgradeToWrite(bool & pendingUpdate);

            /*!
            * @brief Determines if this object is either read or write locked.
            * @return True if locked, false otherwise.
            */
            bool isLocked() const;

            /*!
            * @brief Determines if this object is read locked.
            * @return True if locked, false otherwise.
            */
            bool isReadLocked() const;

            /*!
            * @brief Determines if this object is write locked.
            * @return True if locked, false otherwise.
            */
            bool isWriteLocked() const;

            /*!
            * @brief Returns the number of readers holding the lock.
            * @return The number of readers.
            * @note: This is not an atomic operation
            */
            int numReaders() const;

            /*!
            * @brief Returns the number of pending upgraded writers.
            * @return The number of writers.
            */
            int numPendingWriters() const;

            class Guard
            {
            public:

                /*!
                 * @brief Construct this object and lock the passed-in spinlock as a reader.
                 * @param[in] lock ReadWriteSpinLock which protects a scope during the lifetime of the Guard.
                 * @param[in] acquire Determines the type of ownership.
                 * @note Blocks the current thread until the spinlock is acquired.
                 */
                Guard(ReadWriteSpinLock & lock, LockTraits::AcquireRead acquire);
                Guard(ReadWriteSpinLock & lock, LockTraits::AcquireWrite acquire);

                /*!
                 * @brief Construct this object and tries to lock the passed-in spinlock as a reader.
                 * @param[in] lock ReadWriteSpinLock which protects a scope during the lifetime of the Guard.
                 * @param[in] acquire Determines the type of ownership.
                 * @param[in] tryLock Tag. Not used.
                 * @note Attempts to lock the spinlock. Does not block.
                 */
                Guard(ReadWriteSpinLock & lock, LockTraits::AcquireRead acquire, LockTraits::TryToLock tryLock);
                Guard(ReadWriteSpinLock & lock, LockTraits::AcquireWrite acquire, LockTraits::TryToLock tryLock);

                /*!
                 * @brief Construct this object and depending on the flag may assume ownership.
                 * @param[in] lock ReadWriteSpinLock which protects a scope during the lifetime of the Guard.
                 * @param[in] adoptLock If supplied, assumes the current 'locked' state of the lock.
                 * @param[in] deferLock If supplied, assumes the lock is 'unlocked' and does not lock it.
                 */
                Guard(ReadWriteSpinLock & lock, LockTraits::AdoptLock adoptLock);
                Guard(ReadWriteSpinLock & lock, LockTraits::DeferLock deferLock);

                /*!
                 * @brief Destroy this object and unlock the underlying spinlock.
                 */
                ~Guard();

                /*!
                 * @brief Acquire the underlying spinlock.
                 * @note Blocks.
                 */
                void lockRead();
                void lockWrite();

                /*!
                 * @brief Try to acquire the underlying spinlock.
                 * @return True if spinlock is locked, false otherwise.
                 * @note Does not block.
                 */
                bool tryLockRead();
                bool tryLockWrite();

                /*!
                 * @brief Upgrade this reader to a writer atomically.
                 * @note Blocks until upgrade is performed.
                 * @note The lock must already be owned prior to invoking this function.
                 */
                void upgradeToWrite();

                /*!
                 * @brief Try to upgrade this reader to a writer.
                 * @return True is succeeded.
                 * @note Does not block.
                 * @note The lock must already be owned prior to invoking this function.
                 */
                bool tryUpgradeToWrite();

                /*!
                 * @brief Indicates if this object owns the underlying spinlock.
                 * @return True if ownership is acquired.
                 */
                bool ownsLock() const;
                bool ownsReadLock() const;
                bool ownsWriteLock() const;

                /*!
                 * @brief Unlocks the underlying spinlock
                 */
                void unlock();

            private:

                ReadWriteSpinLock & m_spinlock;
                bool m_ownsLock {false};
                bool m_isUpgraded {false};
                LOG4CXX_DECLARE_STATIC_LOGGER
            };

        private:

            alignas(128) std::atomic_uint32_t m_count {0};
            LOG4CXX_DECLARE_STATIC_LOGGER
        };

    inline log4cxx::LoggerPtr ReadWriteSpinLock::logger =
    log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.ReadWriteSpinLock" ) );
    inline log4cxx::LoggerPtr ReadWriteSpinLock::Guard::logger =
    log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.ReadWriteSpinLock.Guard" ) );
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/ReadWriteSpinlock.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_READ_WRITE_SPINLOCK_H
