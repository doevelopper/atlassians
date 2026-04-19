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
#ifndef QUANUM_COROUTINE_POOL_ALLOCATOR
#define QUANUM_COROUTINE_POOL_ALLOCATOR

#include <cassert>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <boost/context/stack_context.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Spinlock.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================
        //                        struct CoroutinePoolAllocator
        //==============================================================================
        /*!
         * @struct CoroutinePoolAllocator.
         * @brief Provides fast (quasi zero-time) in-place allocation for coroutines.
         *      Coroutine stacks are pre-allocated from separate (i.e. non-contiguous)
         *      heap blocks and maintained in a reusable list.
         * @note This allocator is thread safe. For internal use only and is meant to
         *      replace the boost fixed size pool allocator which crashes.
         */
        template <typename STACK_TRAITS>
        struct CoroutinePoolAllocator
        {
            //------------------------------ Typedefs ----------------------------------
            typedef CoroutinePoolAllocator<STACK_TRAITS> this_type;
            typedef size_t size_type;
            typedef uint16_t index_type;
            typedef STACK_TRAITS traits;

            //------------------------------- Methods ----------------------------------
            explicit CoroutinePoolAllocator(index_type size);
            CoroutinePoolAllocator(const this_type &) = delete;
            CoroutinePoolAllocator(this_type &&) noexcept;
            CoroutinePoolAllocator & operator=(const this_type &) = delete;
            CoroutinePoolAllocator & operator=(this_type &&);
            virtual ~CoroutinePoolAllocator();

            // Accessors
            boost::context::stack_context allocate();
            void deallocate(const boost::context::stack_context & ctx);
            size_t allocatedBlocks() const;
            size_t allocatedHeapBlocks() const;
            bool isFull() const;
            bool isEmpty() const;

        private:

            struct Header
            {
                int _pos;
            };
            enum class ProtectMemPage
            {
                On,
                Off
            };
            int blockIndex(const boost::context::stack_context & ctx) const;
            bool isManaged(const boost::context::stack_context & ctx) const;
            Header * header(const boost::context::stack_context & ctx) const;
            Header * header(uint8_t * block) const;
            uint8_t * stackEnd(const boost::context::stack_context & ctx) const;
            void deallocateBlocks(size_t pos);
            uint8_t * allocateCoroutine(ProtectMemPage protect) const;
            int deallocateCoroutine(uint8_t *) const;

            //------------------------------- Members ----------------------------------
            index_type m_size;
            uint8_t ** m_blocks;
            index_type * _freeBlocks;
            ssize_t m_freeBlockIndex;
            size_t m_numHeapAllocatedBlocks;
            size_t m_stackSize;
            mutable SpinLock m_spinlock;
            LOG4CXX_DECLARE_STATIC_LOGGER
        };

        template <typename STACK_TRAITS>
        inline log4cxx::LoggerPtr CoroutinePoolAllocator<STACK_TRAITS>::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.CoroutinePoolAllocator" ) );

        template <typename STACK_TRAITS>
        struct CoroutinePoolAllocatorProxy
        {
            typedef std::false_type default_constructor;

            explicit CoroutinePoolAllocatorProxy(uint16_t size)
                : m_alloc(new CoroutinePoolAllocator<STACK_TRAITS>(size))
            {
                if (!m_alloc)
                {
                    throw std::bad_alloc();
                }
            }

            // Accessors
            boost::context::stack_context allocate()
            {
                return m_alloc->allocate();
            }

            void deallocate(const boost::context::stack_context & ctx)
            {
                return m_alloc->deallocate(ctx);
            }

            size_t allocatedBlocks() const
            {
                return m_alloc->allocatedBlocks();
            }

            size_t allocatedHeapBlocks() const
            {
                return m_alloc->allocatedHeapBlocks();
            }

            bool isFull() const
            {
                return m_alloc->isFull();
            }

            bool isEmpty() const
            {
                return m_alloc->isEmpty();
            }

        private:

            std::shared_ptr<CoroutinePoolAllocator<STACK_TRAITS>> m_alloc;
            LOG4CXX_DECLARE_STATIC_LOGGER
        };

    template <typename STACK_TRAITS>
    inline log4cxx::LoggerPtr CoroutinePoolAllocatorProxy<STACK_TRAITS>::logger =
        log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.CoroutinePoolAllocator" ) );
} // namespaces

#include <com/github/doevelopper/atlassians/atlas/concurrency/CoroutinePoolAllocator.ipp>

#endif // QUANUM_COROUTINE_POOL_ALLOCATOR
