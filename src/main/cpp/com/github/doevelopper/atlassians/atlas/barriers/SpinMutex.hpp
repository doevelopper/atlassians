
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_BARRIERS_SPINGUARD_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_BARRIERS_SPINGUARD_HPP

#include <com/github/doevelopper/night/owl/barriers/SpinLock.hpp>
#include <atomic>

namespace  com::github::doevelopper::night::owl::barriers
{
    class SpinMutex
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
     public:
        SpinMutex() noexcept;
        virtual ~SpinMutex() noexcept;


        /*!
         * @brief Try locking the mutex, and return whether it succeeded.
         *
         * @details If the mutex is already locked, this method will return immediately
         *       without blocking. To block the calling thread until the mutex can
         *       be acquired, use `lock()` instead.
         *
         * @returns
         *     True if the mutex has been acquired and is now owned by the calling
         *     thread, and false otherwise.
         */

        [[nodiscard]] bool try_lock() noexcept;

        /*!
         * @brief Blocks until the calling thread acquires the mutex.
         *
         * @details  This method will busy-wait until it can acquire the mutex. There is
         *       no back off policy for yielding after a certain number of attempts
         *       have been made.
         *
         * @note When this method returns, the calling thread has acquired the mutex.
         *
         * @note  The behavior is undefined if this method is called while the calling
         */
        void lock() noexcept;

        /*!
         * @brief Unlocks the mutex.
         *    The behavior is undefined if the mutex was not owned by the calling thread.
         */
        void unlock() noexcept;
    protected:
    private:
        std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
        Q_DISABLE_COPY_MOVE(SpinMutex)
    };
}
#endif

