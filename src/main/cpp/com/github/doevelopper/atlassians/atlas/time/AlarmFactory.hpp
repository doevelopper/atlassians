/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     17/06/24 W 20:05
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_ALARMFACTORY_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_ALARMFACTORY_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::time
{
    class LockableCallback;
    class Alarm;
    class AlarmFactory
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:

        AlarmFactory(const AlarmFactory&) = delete;
        AlarmFactory(AlarmFactory&&) = default;
        AlarmFactory& operator=(const AlarmFactory&) = delete;
        AlarmFactory& operator=(AlarmFactory&&) = default;
        virtual ~AlarmFactory() noexcept;

        /*!
         * @brief Create an Alarm that will not fire until scheduled
         *
         * @param callback  Function to call when the Alarm signals
         * @return A handle to an Alarm that can later be scheduled
         */
        virtual std::unique_ptr<Alarm> create_alarm(std::function<void()> const& callback) = 0;

        /*!
         * @brief Create an Alarm that will not fire until scheduled
         *
         * A LockableCallback allows the user to preserve lock ordering
         * in situations where Alarm methods need to be called under external lock
         * and the callback implementation needs to run code protected by the same
         * lock. An alarm implementation may have internal locks of its own, which
         * maybe acquired during callback dispatching; to preserve lock ordering
         * LockableCallback::lock is invoked during callback dispatch before
         * any internal locks are acquired.
         *
         * @param callback Function to call when the Alarm signals
         * @return A handle to an Alarm that can later be scheduled
         */
        virtual std::unique_ptr<Alarm> create_alarm(std::unique_ptr<LockableCallback> callback) = 0;
    protected:
        AlarmFactory() noexcept;
    private:
    };
}

#endif
