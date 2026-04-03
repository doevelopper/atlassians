
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_TIMEOUTGUARD_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_TIMEOUTGUARD_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <atomic>
#include <thread>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::time
{

    class TimeoutGuard
    {
        using clock = std::chrono::system_clock;
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        TimeoutGuard() noexcept;
        TimeoutGuard(const TimeoutGuard &)             = delete;
        TimeoutGuard & operator=(const TimeoutGuard &) = delete;
        TimeoutGuard(TimeoutGuard &&)                  = delete;
        TimeoutGuard & operator=(TimeoutGuard &&)      = delete;
        virtual ~TimeoutGuard() noexcept;

        TimeoutGuard(clock::duration timeout, std::function<void(void)> alarm, clock::duration naptime);
        TimeoutGuard(clock::duration timeout, std::function<void(void)> alarm);

        void watch();
        void touch();

    private:

        void guard();

        clock::duration m_timeout;
        clock::duration m_naptime;

        std::function<void(void)> m_alarm;

        std::atomic_bool m_idle {true};
        std::atomic_bool m_live {true};

        std::atomic<clock::time_point> m_touched {clock::now()};
        ;

        std::thread m_guard_thread;
        std::mutex m_guard_mutex;
        std::condition_variable m_wakeup;
    };
}

#endif
