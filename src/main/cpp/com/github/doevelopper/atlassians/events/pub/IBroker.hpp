/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     08/06/24 W 22:08
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_EVENTS_PUB_IBROKER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_EVENTS_PUB_IBROKER_HPP

#include <com/github/doevelopper/atlassians/events/pub/IPublisher.hpp>
#include <com/github/doevelopper/atlassians/events/pub/ISubscriber.hpp>

namespace com::github::doevelopper::atlassians::events::pub
{
    class IBroker
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:
        IBroker() noexcept;
        IBroker(const IBroker&) = default;
        IBroker(IBroker&&) = default;
        IBroker& operator=(const IBroker&) = default;
        IBroker& operator=(IBroker&&) = default;
        ~IBroker() noexcept;

        virtual void addSubscriber(ISubscriber * subscriber, std::string topic) = 0;
        virtual void registerToPublisher(ISubscriber* publisher) = 0;
        virtual void onPublish(std::string topic, int newData) = 0;

    protected:
    private:
    };
}

#endif
