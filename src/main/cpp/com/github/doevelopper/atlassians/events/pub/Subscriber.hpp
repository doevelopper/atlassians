/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     08/06/24 W 21:17
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_EVENTS_PUB_SUBSCRIBER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_EVENTS_PUB_SUBSCRIBER_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::events::pub
{
    class Subscriber
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:
        Subscriber() noexcept;
        Subscriber(const Subscriber&) = default;
        Subscriber(Subscriber&&) = default;
        Subscriber& operator=(const Subscriber&) = default;
        Subscriber& operator=(Subscriber&&) = default;
        virtual ~Subscriber() noexcept;
    protected:
    private:
    };
}

#endif
