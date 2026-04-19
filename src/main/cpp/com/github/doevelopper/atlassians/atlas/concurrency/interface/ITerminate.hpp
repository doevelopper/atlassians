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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_ITERMINATE_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_ITERMINATE_H

#include <atomic>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ICoroSync.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                  interface ITerminate
        //==============================================================================================
        /// @interface ITerminate
        /// @brief Represents an object which can be terminated. This interface allows certain objects
        ///        to be explicitly disposed of before their destructors are called.
        struct ITerminate
        {
            /// @brief Virtual destructor. This function is explicitly left empty.
            virtual ~ITerminate();

            /// @brief Terminates the object.
            /// @note This method should contain all logic necessary to properly cleanup this instance.
            virtual void terminate() = 0;

            //==============================================================================================
            //                                 class ITerminate::Guard
            //==============================================================================================
            /// @class ITerminate::Guard
            /// @brief RAII-style mechanism for ensuring an object is terminated at the end of a scope.
            ///        Acquires an ITerminate interface on construction and terminates the object in the destructor.
            class Guard
            {
            public:

                /// @brief Constructor
                /// @param[in] object An ITerminate interface
                explicit Guard(ITerminate & object);

                /// @brief Destructor. Calls the terminate() method of the underlying object.
                ~Guard();

            private:

                ITerminate & _object;
                LOG4CXX_DECLARE_STATIC_LOGGER
            };

            LOG4CXX_DECLARE_STATIC_LOGGER
        };

        inline log4cxx::LoggerPtr ITerminate::Guard::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.ITerminate.Guard" ) );

        inline log4cxx::LoggerPtr ITerminate::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.ITerminate" ) );
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITerminate.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_ITERMINATE_H
