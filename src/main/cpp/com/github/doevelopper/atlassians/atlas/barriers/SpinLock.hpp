
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_BARRIERS_SPINLOCK_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_BARRIERS_SPINLOCK_HPP

#include <atomic>
#include <cassert>
#include <thread>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace  com::github::doevelopper::night::owl::barriers
{
    /*!
     * @brief  This class implements a lock that will never cause an std::thread to block
     *      (i.e. sleep) if the lock is unavailable.
     *
     * @details   Instead the SpinLock will busy-wait
     *      for the lock to become available. The SpinLock is intended to enforce short
     *      periods of mutual exclusion where the lock is not held for very long. These
     *      locks are not recursive; if the same thread tries to lock a SpinLock while
     *      holding it, the thread will deadlock.
     *
     */
    class SpinLock
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
        Q_DISABLE_COPY_MOVE(SpinLock)
     public:
        SpinLock() noexcept;
        virtual ~SpinLock() noexcept;
        /*!
         * @brief Acquire the SpinLock; blocks the thread (by continuously polling the
         *      lock) until the lock has been acquired.
         */
        void lock() noexcept;
        /*!
         * @brief Try to acquire the SpinLock; does not block the thread and returns
         *      immediately.
         *
         * @return True if the lock was successfully acquired, false if it was already
         *      owned by some other thread.
         */
        bool try_lock() noexcept;
        /*!
         * @brief Release the SpinLock.  The caller must previously have acquired the
         *      SpinLock with a call to lock() or try_lock().
         */
        void unlock() noexcept;
    protected:
    private:
        std::atomic_flag    m_lock ;//= ATOMIC_FLAG_INIT;
        std::thread::id     m_owner ;//{ };
        int                 m_count ;//{ 0 };
    };

    /*!
     * @brief Define a type alias for an RAII SpinLock lock_guard for convenience.
     */
    using Lock = std::lock_guard<SpinLock>;

    /*!
     * @brief Define a type alias for a movable SpinLock unique_lock for convenience.
     */
    using UniqueLock = std::unique_lock<SpinLock>;

}
#endif
