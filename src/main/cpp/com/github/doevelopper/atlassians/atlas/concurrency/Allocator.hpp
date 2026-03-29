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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_ALLOCATOR_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_ALLOCATOR_H

#include <memory>
#include <boost/coroutine2/all.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Stl.ipp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/AllocatorTraits.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/CoroutinePoolAllocator.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/HeapAllocator.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/StackAllocator.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/StackTraits.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                struct StackTraitsProxy
        //==============================================================================================
        struct StackTraitsProxy
        {
            static bool is_unbounded()
            {
                return StackTraits::isUnbounded();
            }

            static std::size_t page_size()
            {
                return StackTraits::pageSize();
            }

            static std::size_t default_size()
            {
                return StackTraits::defaultSize();
            }

            static std::size_t minimum_size()
            {
                return StackTraits::minimumSize();
            }

            static std::size_t maximum_size()
            {
                return StackTraits::maximumSize();
            }
        };

        //==============================================================================================
        //                                 struct StlAllocator
        //==============================================================================================
        template <typename T>
        struct StlAllocator : public std::allocator<T>
        {
            typedef std::true_type default_constructor;
        };

//==============================================================================================
//                                 struct BoostAllocator
//==============================================================================================
#if defined(__QUANTUM_BOOST_USE_SEGMENTED_STACKS) || defined(__QUANTUM_BOOST_USE_PROTECTED_STACKS)                     \
    || defined(__QUANTUM_BOOST_USE_FIXEDSIZE_STACKS)
        template <typename Traits>
        struct BoostAllocator :
    #if defined(__QUANTUM_BOOST_USE_SEGMENTED_STACKS)
            public boost::context::basic_segmented_stack<Traits>
    #elif defined(__QUANTUM_BOOST_USE_PROTECTED_STACKS)
            public boost::context::basic_protected_fixedsize_stack<Traits>
    #elif defined(__QUANTUM_BOOST_USE_FIXEDSIZE_STACKS)
            // Default for Boost
            public boost::context::basic_fixedsize_stack<Traits>
    #endif
        {
            typedef std::true_type default_constructor;
        };
        using CoroStackAllocator = BoostAllocator<StackTraitsProxy>;
#else
        using CoroStackAllocator = CoroutinePoolAllocatorProxy<StackTraitsProxy>;
#endif

        //==============================================================================================
        //                               struct Allocator (singleton)
        //==============================================================================================
        template <typename AllocType>
        struct Allocator
        {
            template <typename A = AllocType>
            static AllocType & instance(std::enable_if_t<!A::default_constructor::value, uint16_t> size)
            {
                static AllocType allocator(size);
                return allocator;
            }

            template <typename A = AllocType>
            static AllocType & instance(std::enable_if_t<A::default_constructor::value, uint16_t> = 0)
            {
                static AllocType allocator;
                return allocator;
            }
        };
}

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_ALLOCATOR_H
