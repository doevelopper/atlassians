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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_FUNCTIONS_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_FUNCTIONS_H

#include <functional>
#include <iterator>
#include <utility>
#include <vector>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Traits.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                    struct Functions
        //==============================================================================================
        /*!
         * @struct Functions
         * @brief Contains typedefs for various functions.
         */
        struct Functions
        {
            template <class RET, class INPUT_IT>
            using ForEachFunc =
                std::function<RET(VoidContextPtr, const typename std::iterator_traits<INPUT_IT>::value_type &)>;

            template <class KEY, class MAPPED_TYPE, class INPUT_IT>
            using MapFunc = std::function<std::vector<std::pair<KEY, MAPPED_TYPE>>(
                VoidContextPtr, const typename std::iterator_traits<INPUT_IT>::value_type &)>;

            template <class KEY, class MAPPED_TYPE, class REDUCED_TYPE>
            using ReduceFunc = std::function<std::pair<KEY, REDUCED_TYPE>(
                VoidContextPtr, std::pair<KEY, std::vector<MAPPED_TYPE>> &&)>;
        };
}

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_FUNCTIONS_H
