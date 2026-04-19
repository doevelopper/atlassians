/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     18/06/24 W 21:30
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_BARRIERS_SYNCHRONISED_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_BARRIERS_SYNCHRONISED_HPP

#include <mutex>

#include <com/github/doevelopper/atlassians/atlas/barriers/RecursiveReadWriteMutex.hpp>

namespace com::github::doevelopper::atlassians::atlas::barriers
{
    /*!
     * @brief An object that enforces unique access to the data it contains
     *
     *      This behaves like a mutex which owns the data it guards, and
     *      can give out a smart-pointer-esque handle to lock and access it.
     *
     * @tparam T  The type of data contained
     */
    template<typename T>
    class Synchronised
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:

        /**
         * Smart-pointer-esque accessor for the protected data.
         *
         * Ensures exclusive access to the referenced data.
         *
         * \note Instances of Locked must not outlive the Synchronised
         *       they are derived from.
         */
        template<typename U>
        class LockedImpl
        {
        public:
            LockedImpl(LockedImpl&& from) noexcept
                : m_value{from.m_value},
                  m_lock{std::move(from.m_lock)}
            {
                from.m_value = nullptr;
            }

            ~LockedImpl() = default;

            auto operator*() const -> U&
            {
                return *m_value;
            }

            auto operator->() const -> U*
            {
                return m_value;
            }

            /**
             * Relinquish access to the data
             *
             * This prevents further access to the contained data through
             * this handle, and allows other code to acquire access.
             */
            void drop()
            {
                m_value = nullptr;
                m_lock.unlock();
            }

            /**
             * Allows waiting for a condition variable
             *
             * The protected data may be accessed both in the predicate and after this method completes.
             */
            template<typename Cv, typename Predicate>
            void wait(Cv& cv, Predicate stop_waiting)
            {
                cv.wait(m_lock, stop_waiting);
            }

        private:
            friend class Synchronised;
            LockedImpl(std::unique_lock<std::mutex>&& lock, U& value)
                : m_value{&value},
                  m_lock{std::move(lock)}
            {
            }

            U* m_value;
            std::unique_lock<std::mutex> m_lock;
        };

        Synchronised() noexcept;
        Synchronised(const Synchronised&) = default;
        Synchronised(Synchronised&&) = default;
        Synchronised& operator=(const Synchronised&) = default;
        Synchronised& operator=(Synchronised&&) = default;
        virtual ~Synchronised() noexcept;

        /*!
         * @brief Smart-pointer-esque accessor for the protected data.
         *
         *  Ensures exclusive access to the referenced data.
         *
         * @note Instances of Locked must not outlive the Synchronised
         *       they are derived from.
         */
        using Locked = LockedImpl<T>;
        /*!
         * @brief Smart-pointer-esque accessor for the protected data.
         *
         *      Provides const access to the protected data, with the guarantee
         * that no changes to the data can be made while this handle
         * is live.
         *
         * @note Instances of Locked must not outlive the Synchronised
         *       they are derived from.
         */
        using LockedView = LockedImpl<T const>;
        /*!
         * @brief Lock the data and return an accessor.
         *
         * @return A smart-pointer-esque accessor for the contained data.
         *         While code has access to a live Locked instance it is
         *         guaranteed to have unique access to the contained data.
         */
        auto lock() -> Locked
        {
            return LockedImpl<T>{std::unique_lock{m_mutex}, m_value};
        }

        /*!
        * @brief Lock the data and return an accessor.
        *
        * @return A smart-pointer-esque accessor for the contained data.
        *         While code has access to a live Locked instance it is
        *         guaranteed to have unique access to the contained data.
        */
        auto lock() const -> LockedView
        {
            return LockedImpl<T const>{std::unique_lock{m_mutex}, m_value};
        }
    protected:
    private:
        std::mutex mutable m_mutex;
        T m_value;
    };
}


#endif
