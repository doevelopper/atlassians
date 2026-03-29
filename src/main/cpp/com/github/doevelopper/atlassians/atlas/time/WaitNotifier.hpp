#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_WAITNOTIFIER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_WAITNOTIFIER_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::time
{
    class WaitNotifier
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:
        WaitNotifier() noexcept;
        WaitNotifier(const WaitNotifier&) = default;
        WaitNotifier(WaitNotifier&&) = default;
        WaitNotifier& operator=(const WaitNotifier&) = default;
        WaitNotifier& operator=(WaitNotifier&&) = default;
        virtual ~WaitNotifier() noexcept;

        // Wait for condition var notify
        void wait();

        // Wait for deserted time
        void wait_until(const std::chrono::system_clock::time_point &tm_point);

        // Wait for some duration time
        void wait_for(const std::chrono::system_clock::duration &tm_duration);

        // Notify once
        void notify();

    protected:
    private:
        bool m_notified{false};
        std::mutex m_mutex;
        std::condition_variable m_cv;
    };
}
#endif
