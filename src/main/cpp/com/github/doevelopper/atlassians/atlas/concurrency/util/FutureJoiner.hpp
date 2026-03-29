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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_FUTURE_JOINER_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_FUTURE_JOINER_H

#include <type_traits>
#include <vector>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ICoroContext.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ICoroFuture.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IThreadContext.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IThreadFuture.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Stl.ipp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                      class FutureJoiner
        //==============================================================================================
        /*! @class FutureJoiner
         *  @brief Utility class that joins N futures into a single one.
         *  @details Instead of waiting for N futures to complete, the user can join them and wait
         *      on a single future which returns N values.
         * @tparam T The type returned by the future.
         */
        template <typename T>
        class FutureJoiner
        {
        public:

            /*!
             * @brief Join N thread futures.
             * @param[in] futures A vector of thread futures of type T.
             * @return A joined future to wait on.
             */
            template <
                class DISPATCHER,
                class = std::enable_if_t<std::is_same<typename DISPATCHER::ContextTag, ThreadContextTag>::value>>
            ThreadFuturePtr<std::vector<T>>
            operator()(DISPATCHER & dispatcher, std::vector<ThreadContextPtr<T>> && futures);

            template <
                class DISPATCHER,
                class = std::enable_if_t<std::is_same<typename DISPATCHER::ContextTag, ThreadContextTag>::value>>
            ThreadFuturePtr<std::vector<T>>
            operator()(DISPATCHER & dispatcher, std::vector<ThreadFuturePtr<T>> && futures);

            template <
                class DISPATCHER,
                class = std::enable_if_t<std::is_same<typename DISPATCHER::ContextTag, CoroContextTag>::value>>
            CoroContextPtr<std::vector<T>>
            operator()(DISPATCHER & dispatcher, std::vector<CoroContextPtr<T>> && futures);

            template <
                class DISPATCHER,
                class = std::enable_if_t<std::is_same<typename DISPATCHER::ContextTag, CoroContextTag>::value>>
            CoroContextPtr<std::vector<T>>
            operator()(DISPATCHER & dispatcher, std::vector<CoroFuturePtr<T>> && futures);

        private:

            template <template <class> class FUTURE, class DISPATCHER>
            ThreadFuturePtr<std::vector<T>>
            join(ThreadContextTag, DISPATCHER & dispatcher, std::vector<typename FUTURE<T>::Ptr> && futures);

            template <template <class> class FUTURE, class DISPATCHER>
            CoroContextPtr<std::vector<T>>
            join(CoroContextTag, DISPATCHER & dispatcher, std::vector<typename FUTURE<T>::Ptr> && futures);

            LOG4CXX_DECLARE_STATIC_LOGGER
        };
        template <typename RET>
        inline log4cxx::LoggerPtr FutureJoiner<RET>::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.FutureJoiner" ) );
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/util/FutureJoiner.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_FUTURE_JOINER_H
