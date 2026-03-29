/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     08/06/24 W 20:19
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_TIMEFIXTURE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_TIMEFIXTURE_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::time
{
    class TimeFixture
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:
        /**
         * @using TimepointT
         * @brief Type alias for std::chrono::time_point<std::chrono::system_clock>.
         */
        using TimepointT = std::chrono::time_point<std::chrono::system_clock>;

        /**
         * @using Ptr
         * @brief Type alias for std::shared_ptr<TimeFixture>.
         */
        using Ptr = std::shared_ptr<TimeFixture>;

        TimeFixture() = delete;
        TimeFixture(const TimeFixture&) = default;
        TimeFixture(TimeFixture&&) = default;
        TimeFixture& operator=(const TimeFixture&) = default;
        TimeFixture& operator=(TimeFixture&&) = default;
        virtual ~TimeFixture() noexcept;

    /**
     * @brief Constructor.
     * @param name Name of the time fixture.
     * @param unit Unit of time (default: 0).
     */
    TimeFixture(const char *name, std::size_t unit = 0);

    /**
     * @brief Get the current time point.
     * @return Current time point.
     */
    TimepointT nowPoint() const;

    /**
     * @brief Get the start time point.
     * @return Start time point.
     */
    const TimepointT startPoint() const;

    /**
     * @brief Get the time in seconds.
     * @tparam T Type of the result (default: float).
     * @return Time in seconds.
     */
    template <typename T = float>
    const T seconds() const;

    /**
     * @brief Get the time in milliseconds.
     * @tparam T Type of the result (default: float).
     * @return Time in milliseconds.
     */
    template <typename T = float>
    const T milliSeconds() const;

    /**
     * @brief Get the time in microseconds.
     * @tparam T Type of the result (default: float).
     * @return Time in microseconds.
     */
    template <typename T = float>
    const T microSeconds() const;

    /**
     * @brief Get the time in nanoseconds.
     * @tparam T Type of the result (default: float).
     * @return Time in nanoseconds.
     */
    template <typename T = float>
    const T nanoSeconds() const;

    /**
     * @brief Get the time between two points in seconds.
     * @tparam T Type of the result (default: float).
     * @param endPoint End time point.
     * @return Time in seconds.
     */
    template <typename T = float>
    const T seconds(const TimepointT &endPoint) const;

    /**
     * @brief Get the time between two points in milliseconds.
     * @tparam T Type of the result (default: float).
     * @param endPoint End time point.
     * @return Time in milliseconds.
     */
    template <typename T = float>
    const T milliSeconds(const TimepointT &endPoint) const;

    /**
     * @brief Get the time between two points in microseconds.
     * @tparam T Type of the result (default: float).
     * @param endPoint End time point.
     * @return Time in microseconds.
     */
    template <typename T = float>
    const T microSseconds(const TimepointT &endPoint) const;

    /**
     * @brief Get the time between two points in nanoseconds.
     * @tparam T Type of the result (default: float).
     * @param endPoint End time point.
     * @return Time in nanoseconds.
     */
    template <typename T = float>
    const T nanoSeconds(const TimepointT &endPoint) const;

    protected:
    private:
        /**
         * @brief Static timer ID.
         */
        static unsigned m_timerId;

        /**
         * @brief Start time point.
         */
        std::chrono::time_point<std::chrono::system_clock> m_start;

        /**
         * @brief Name of the time fixture.
         */
        const char *m_name;

        /**
         * @brief Number of the time fixture.
         */
        const unsigned m_number;

        /**
         * @brief Unit of time.
         */
        std::size_t m_unit;

        /**
         * @brief Time information.
         */
        std::stringstream m_timeInformation;
    };

    /*!
    *  @using TimeFixturePtr
    * @brief Type alias for TimeFixture::Ptr.
    */
    using TimeFixturePtr     = TimeFixture::Ptr;
}

#endif
