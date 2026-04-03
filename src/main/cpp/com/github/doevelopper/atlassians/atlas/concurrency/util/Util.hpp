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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_UTIL_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_UTIL_H

#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <tuple>
#include <utility>
#include <vector>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Capture.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IContext.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IPromise.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITask.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Traits.hpp>

namespace Bloomberg::quantum
{
        // fwd declarations
        template <class RET>
        class Context;
        template <class RET>
        class Promise;

        //==============================================================================================
        //                                      struct Util
        //==============================================================================================
        /// @struct Util.
        /// @brief Utility to bind a user callable function unto a coroutine or an IO task.
        /// @note For internal use only.
        struct Util
        {
            template <class RET, class FUNC, class... ARGS>
            static Function<int(Traits::Yield &)>
            bindCaller(std::shared_ptr<Context<RET>> ctx, FUNC && func0, ARGS &&... args0);

            template <class RET, class FUNC, class... ARGS>
            static Function<int(Traits::Yield &)>
            bindCaller2(std::shared_ptr<Context<RET>> ctx, FUNC && func0, ARGS &&... args0);

            template <class RET, class FUNC, class... ARGS>
            static Function<int()> bindIoCaller(std::shared_ptr<Promise<RET>> promise, FUNC && func0, ARGS &&... args0);

            template <class RET, class FUNC, class... ARGS>
            static Function<int()>
            bindIoCaller2(std::shared_ptr<Promise<RET>> promise, FUNC && func0, ARGS &&... args0);

            template <typename RET>
            static VoidContextPtr makeVoidContext(CoroContextPtr<RET> ctx);

            //------------------------------------------------------------------------------------------
            //                                      ForEach
            //------------------------------------------------------------------------------------------
            template <class RET, class INPUT_IT, class FUNC>
            static std::vector<RET> forEachCoro(VoidContextPtr ctx, INPUT_IT inputIt, size_t num, FUNC && func);

            template <class RET, class INPUT_IT, class FUNC>
            static std::vector<std::vector<RET>> forEachBatchCoro(
                VoidContextPtr ctx, INPUT_IT inputIt, size_t num, FUNC && func, size_t numCoroutineThreads);

            //------------------------------------------------------------------------------------------
            //                                      MapReduce
            //------------------------------------------------------------------------------------------
            template <class KEY, class MAPPED_TYPE, class REDUCED_TYPE, class INPUT_IT>
            static std::map<KEY, REDUCED_TYPE> mapReduceCoro(
                VoidContextPtr ctx, INPUT_IT inputIt, size_t num,
                const Functions::MapFunc<KEY, MAPPED_TYPE, INPUT_IT> & mapper,
                const Functions::ReduceFunc<KEY, MAPPED_TYPE, REDUCED_TYPE> & reducer);

            template <class KEY, class MAPPED_TYPE, class REDUCED_TYPE, class INPUT_IT>
            static std::map<KEY, REDUCED_TYPE> mapReduceBatchCoro(
                VoidContextPtr ctx, INPUT_IT inputIt, size_t num,
                const Functions::MapFunc<KEY, MAPPED_TYPE, INPUT_IT> & mapper,
                const Functions::ReduceFunc<KEY, MAPPED_TYPE, REDUCED_TYPE> & reducer);

#ifdef __QUANTUM_PRINT_DEBUG
            // Synchronize logging
            static std::mutex & LogMutex();
#endif
            LOG4CXX_DECLARE_STATIC_LOGGER
        };
        inline log4cxx::LoggerPtr Util::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.util.Util" ) );
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/util/Util.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_UTIL_H
