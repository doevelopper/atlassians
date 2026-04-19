/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     18/06/24 W 22:22
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_POSIXTIMESTAMP_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_POSIXTIMESTAMP_HPP

#include <chrono>
#include <ctime>
#include <stdexcept>


#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::time
{

    /*!
     *  @brief We need absolute precision here so sadly can't use high-level C++ clocks...
     *  - Graphics frame timing needs support for at least the kernel clocks
     *    CLOCK_REALTIME and CLOCK_MONOTONIC, to be selected at runtime, whereas
     *    std::chrono does not support CLOCK_REALTIME or easily switching clocks.
     *  - mir::time::Timestamp is relative to the (wrong) epoch of steady_clock,
     *    so converting to/from mir::time::Timestamp would be dangerously
     *    inaccurate at best.
     */
    class PosixTimestamp
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:
        PosixTimestamp() noexcept;
        PosixTimestamp(const PosixTimestamp&) = default;
        PosixTimestamp(PosixTimestamp&&) = default;
        PosixTimestamp& operator=(const PosixTimestamp&) = default;
        PosixTimestamp& operator=(PosixTimestamp&&) = default;
        virtual ~PosixTimestamp() noexcept;

        PosixTimestamp(clockid_t clk, std::chrono::nanoseconds ns);
        PosixTimestamp(clockid_t clk, struct timespec const& ts);

        static PosixTimestamp now(clockid_t clock_id)
        {
            struct timespec ts{};
            clock_gettime(clock_id, &ts);
            return PosixTimestamp(clock_id, ts);
        }

        [[nodiscard]] clockid_t get_m_clock_id() const;
        void set_m_clock_id(const clockid_t m_clock_id);
        [[nodiscard]] const std::chrono::nanoseconds & get_m_nanoseconds() const;
        void set_m_nanoseconds(const std::chrono::nanoseconds & m_nanoseconds);
    protected:
    private:
        clockid_t m_clock_id;
        std::chrono::nanoseconds m_nanoseconds;
    };


    inline void assert_same_clock(PosixTimestamp const& a, PosixTimestamp const& b)
    {
        if (a.get_m_clock_id() != b.get_m_clock_id())
            throw std::logic_error("Can't compare different time domains");
    }

    inline bool operator==(PosixTimestamp const& a, PosixTimestamp const& b)
    {
        return a.get_m_clock_id() == b.get_m_clock_id() && a.get_m_nanoseconds() == b.get_m_nanoseconds();
    }

    inline PosixTimestamp operator-(PosixTimestamp const& a,
                                    std::chrono::nanoseconds b)
    {
        return PosixTimestamp(a.get_m_clock_id(), a.get_m_nanoseconds() - b);
    }

    inline std::chrono::nanoseconds operator-(PosixTimestamp const& a,
                                              PosixTimestamp const& b)
    {
        assert_same_clock(a, b);
        return a.get_m_nanoseconds() - b.get_m_nanoseconds();
    }

    inline PosixTimestamp operator+(PosixTimestamp const& a,
                                    std::chrono::nanoseconds b)
    {
        return PosixTimestamp(a.get_m_clock_id(), a.get_m_nanoseconds() + b);
    }

    inline std::chrono::nanoseconds operator%(PosixTimestamp const& a,
                                              std::chrono::nanoseconds b)
    {
        return std::chrono::nanoseconds(a.get_m_nanoseconds().count() % b.count());
    }

    inline bool operator>(PosixTimestamp const& a, PosixTimestamp const& b)
    {
        assert_same_clock(a, b);
        return a.get_m_nanoseconds() > b.get_m_nanoseconds();
    }

    inline bool operator<(PosixTimestamp const& a, PosixTimestamp const& b)
    {
        assert_same_clock(a, b);
        return a.get_m_nanoseconds() < b.get_m_nanoseconds();
    }

    inline bool operator>=(PosixTimestamp const& a, PosixTimestamp const& b)
    {
        assert_same_clock(a, b);
        return a.get_m_nanoseconds() >= b.get_m_nanoseconds();
    }

    inline bool operator<=(PosixTimestamp const& a, PosixTimestamp const& b)
    {
        assert_same_clock(a, b);
        return a.get_m_nanoseconds() <= b.get_m_nanoseconds();
    }

    inline void sleep_until(PosixTimestamp const& t)
    {
        long long ns = t.get_m_nanoseconds().count();
        struct timespec ts{};
        ts.tv_sec = ns / 1000000000LL;
        ts.tv_nsec = ns % 1000000000LL;
        while (EINTR == clock_nanosleep(t.get_m_clock_id(), TIMER_ABSTIME, &ts, NULL)) {}
    }
}

#endif
