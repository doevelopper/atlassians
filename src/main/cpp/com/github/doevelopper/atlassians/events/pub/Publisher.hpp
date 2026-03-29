/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     08/06/24 W 21:17
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_EVENTS_PUB_PUBLISHER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_EVENTS_PUB_PUBLISHER_HPP

#include <functional>
#include <list>
#include <memory>
#include <utility>
#include <type_traits>
#include <variant>

#include <com/github/doevelopper/atlassians/events/pub/Subscriber.hpp>

namespace com::github::doevelopper::atlassians::events::pub
{
    class Publisher
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:
        Publisher() noexcept;
        Publisher(const Publisher&) = default;
        Publisher(Publisher&&) = default;
        Publisher& operator=(const Publisher&) = default;
        Publisher& operator=(Publisher&&) = default;
        virtual ~Publisher() noexcept;

        /**
         * @brief Subscribe to type publications.
         *
         * Passing @c nullptr as argument or an already subscribed object has no
         * effect.
         *
         * @param subscriber Object to subscribe.
         * @param type Type to subscribe to.
         */
        void subscribe(Subscriber * const subscriber,
                       const std::type_info & type);
        /**
         * @brief Unsubscribe from type publications.
         *
         * Passing @c nullptr as argument or an unsubscribed object has no effect.
         *
         * @param subscriber Object to unsubscribe.
         * @param type Type to unsubscribe from.
         */
        void unsubscribe(Subscriber * const subscriber,
                         const std::type_info & type);
        /**
         * @brief Unsubscribe all from type.
         * @param type Type to unsubscribe from.
         */
        void unsubscribe(const std::type_info & type);
        /**
         * @brief Unsubscribe all.
         */
        void unsubscribe();

        /**
         * @brief Publish data to subscribers.
         * @param data Published data.
         */
        // void publish(const std::variant<> & data);
    protected:
    private:
    };
}

#endif
