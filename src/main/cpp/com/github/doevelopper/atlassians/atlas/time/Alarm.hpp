/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     17/06/24 W 19:56
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_ALARM_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_ALARM_HPP

#include <chrono>
#include <memory>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::time
{
    /*!
     * @brief A one-shot, resettable handle to trigger a callback at a later time
     * @note All members of Alarm are threadsafe
     * @note All members of Alarm are safe to call from the Alarm's callback
     */
    class Alarm
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:
        enum State
        {
            pending,    /**< Will trigger the callback at some point in the future */
            cancelled,  /**< The callback has been cancelled before being triggered */
            triggered   /**< The callback has been called */
        };
        using Timestamp = std::chrono::steady_clock::time_point;
        using Duration = std::chrono::steady_clock::duration;

        Alarm() noexcept;
        Alarm(const Alarm&) = default;
        Alarm(Alarm&&) = default;
        Alarm& operator=(const Alarm&) = default;
        Alarm& operator=(Alarm&&) = default;
        /*!
         * @note Destruction of the Alarm guarantees that the callback will not subsequently be called
         */
        virtual ~Alarm() noexcept;

        /*!
         * @brief Cancels a pending alarm
         *
         * \note Has no effect if the Alarm is in the Triggered state.
         * \note cancel() is idempotent
         *
         * \return True iff the state of the Alarm is now Cancelled
         */
        virtual bool cancel() = 0;
        /*!
         * @brief Returns the time remaining until the alarm is triggered
         * @return The time remaining until the alarm is triggered
         *
         * \note If the Alarm is in the Triggered state, this will return 0.
         */
        virtual Duration remaining() const = 0;
        virtual State state() const = 0;

        /*!
         * @brief Reschedule the alarm
         * @param delay    Delay, in milliseconds, before the Alarm will be triggered
         * @return         True if this reschedule supersedes a previous not-yet-triggered timeout
         *
         * @note This cancels any previous timeout set.
         */
        virtual bool reschedule_in(std::chrono::milliseconds delay) = 0;

        /*!
         * @brief Reschedule the alarm
         * @param timeout  Time point when the alarm should be triggered
         * @return         True if this reschedule supersedes a previous not-yet-triggered timeout
         *
         * @note This cancels any previous timeout set.
         */
        virtual bool reschedule_for(Timestamp timeout) = 0;
    protected:
    private:
    };
}


#endif
