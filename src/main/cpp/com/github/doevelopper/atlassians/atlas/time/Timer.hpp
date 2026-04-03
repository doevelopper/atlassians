
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_TIMER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_TIMER_HPP

#include <chrono>
#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

#include <cstdint>
#include <signal.h>
#include <time.h>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::time
{
    using TimerClock = std::chrono::steady_clock;
    using HiResClock = std::chrono::high_resolution_clock;
    using TimerCallbackCancel = std::function< void () >;
    using TimerCallbackFunction = std::function< void () >;
    using TimePoint = std::chrono::time_point< TimerClock >;
    using HiResTimePoint = std::chrono::high_resolution_clock::time_point;
    using TimePrint = std::function< std::string(std::string, std::string) >;
    using DurationMs = std::chrono::duration<float, std::milli>;
    using DurationUs = std::chrono::duration<float, std::micro>;
    using DurationNs = std::chrono::duration<float, std::nano>;
    using MilliSecs = std::chrono::milliseconds;
    using Duration = std::chrono::duration<double>;

//  using Timeouts = std::multimap<timerClock::time_point, std::shared_ptr<timerCallbackFunction>>;

    template < typename T >

    class SDLC_API_EXPORT Timer
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

        enum class TimerType : std::uint8_t
        {
            SINGLE_SHOOT,  // timer which Signals only ones
            MULTI_SHOOT    // Periodic timer
        };

        class ScopeTimer
        {
            LOG4CXX_DECLARE_STATIC_LOGGER

            std::string s;
            std::chrono::high_resolution_clock::time_point t0;
            std::chrono::high_resolution_clock::time_point start;

        public:
            ScopeTimer();
            virtual ~ScopeTimer();
            template<typename ...Args>
            ScopeTimer (Args&& ...args);
        };

    public:

        Timer();
        Timer(const Timer &) = default;
        Timer(Timer &&) = default;
        Timer &operator=(const Timer &) = default;
        Timer &operator=(Timer &&) = default;

        /*!
         * @brief Construct a Timer with a explanatory message.
         * @param handler Class to which timer needs to notify the during timeout.
         */
        Timer(T *handler);

        /*!
         * @brief Destroy the Timer object
         *
         */
        virtual ~Timer();

        /*!
         * @brief Gets the internal timer ID
         */
        timer_t getID()
        {
            return (m_TimerID);
        };

        T *getHandler();
        /*!
         * @brief
         *
         * @param msec
         */
        void start(std::uint64_t msec);

        /*!
         * @brief
         *
         */
        void Start();

        /*!
         * @brief
         *
         */
        void Stop();

        /*!
         * @brief Set the Interval object
         *
         * @param msec
         */
        void setInterval(std::uint64_t msec);

        /*!
         * @brief Get the Interval object
         *
         * @return std::uint64_t
         */
        std::uint64_t getInterval();

        TimerType getTimerType();
        /*!
         * @brief Set the Timer Type object
         *
         * @param type
         */
        void setTimerType(TimerType type);

        /*!
         * @brief
         *
         * @return std::chrono::time_point<Clock, MilliSecs>
         */
        std::chrono::time_point<TimerClock, MilliSecs> timestamp();
    protected:

    private:

        /*!
         * @brief
         *
         * @param msec
         */
        void StartTimer(std::uint64_t msec);
        /*!
         * @brief
         *
         */
        void StopTimer();
        /*!
         * @brief
         *
         * @param sigval
         */
        static void TimerThreadFunc(union sigval arg);

        T *m_handler;
        timer_t m_TimerID;
        TimerType m_TimerType;
        std::uint64_t m_interval;

        // Timeouts m_timeouts;
        std::mutex m_mutex;
        std::condition_variable m_cv;
        std::unique_ptr< std::thread > m_thread;
        static const std::uint64_t MS_TO_NS_FACTOR;
    };

}  // namespace cfs::osal
#endif
