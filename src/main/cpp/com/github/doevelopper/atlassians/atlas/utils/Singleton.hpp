/*******************************************************************
* @file
 * @version  v0.0.
 * @date     02/06/24 W 17:02
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_UTILS_SINGLETON_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_UTILS_SINGLETON_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>
#include <boost/serialization/singleton.hpp>

namespace com::github::doevelopper::atlassians::atlas::utils
{
    template <typename T>
    class Singleton : boost::serialization::singleton<T>
    {
        Q_DISABLE_COPY_MOVE(Singleton)
        LOG4CXX_DECLARE_STATIC_LOGGER
    private:

        // Singleton(const Singleton &) noexcept             = default;
        // Singleton(Singleton &&) noexcept                  = default;
        // Singleton & operator=(const Singleton &) noexcept = default;
        // Singleton & operator=(Singleton &&) noexcept      = default;

    public:
        virtual ~Singleton() noexcept = delete; //delete should be public
        Singleton() noexcept = delete;

        // // using object_type = T;
        // static /*object_type*/  T& instance()
        // {
        //     std::lock_guard<std::mutex> lock(m_mutex);
        //     return boost::serialization::singleton<T>::get_mutable_instance();
        // }

        static T& instance()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (!m_instance) {
                m_instance = std::make_unique<T>();
            }
            return *m_instance;
        }
        // Befriend to allow your singleton class to have a private constructor
        using friend_type = boost::serialization::detail::singleton_wrapper<T>;

    protected:
    private:
        static std::unique_ptr<T> m_instance;
        static std::mutex m_mutex;
    };
    // Initialize static members
    template<typename T>
    std::unique_ptr<T> Singleton<T>::m_instance{nullptr};

    template<typename T>
    std::mutex Singleton<T>::m_mutex{};

    template < typename T >
    log4cxx::LoggerPtr Singleton< T >::logger =
        log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.utils.Singleton" ) );

    // template <typename T>
    // Singleton<T>::Singleton() noexcept
    // {
    //     LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // }

    // template <typename T>
    // Singleton<T>::~Singleton() noexcept
    // {
    //     LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // }
}

#endif
