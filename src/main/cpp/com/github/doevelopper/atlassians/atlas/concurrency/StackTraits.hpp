/*
** Copyright 2018 Bloomberg Finance L.P.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_STACK_TRAITS_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_STACK_TRAITS_H

#include <boost/context/stack_traits.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                 struct StackTraits
        //==============================================================================================
        /*!
        * @struct StackTraits.
        * @brief Allows application-wide overrides for the coroutine stack traits which are used
        *       internally by boost::coroutines2.
        * @note See boost::context::stack_traits for details. Typically only the default size should be modified.
        */
        struct StackTraits
        {
            /*!
            * @brief Get/set if the environment defines a limit for the stack size.
            * @return Modifiable reference.
            */
            static bool & isUnbounded();

            /*!
            * @brief Get/set the page size.
            * @return Modifiable reference to the size in bytes.
            */
            static std::size_t & pageSize();

            /*!
            * @brief Get/set the default stack size, which may be platform specific.
            * @return Modifiable reference to the size in bytes.
            * @details If the stack is unbounded, the default boost implementation returns the max of {64kB,
            *       minimum_size()}.
            */
            static std::size_t & defaultSize();

            /*!
            * @brief Get/set the minimum stack size as defined by the environment.
            * @return Modifiable reference to the size in bytes.
            * @note Win32 4kB/Win64 8kB, defined by rlimit on POSIX.
            */
            static std::size_t & minimumSize();

            /*!
            * @brief Get/set the maximum stack size.
            * @return Modifiable reference to the size in bytes.
            * @note Only takes effect if isUnbounded() == false.
            */
            static std::size_t & maximumSize();

            LOG4CXX_DECLARE_STATIC_LOGGER
        };
        inline log4cxx::LoggerPtr StackTraits::logger =
        log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.StackTraits" ) );
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/StackTraits.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_STACK_TRAITS_H
