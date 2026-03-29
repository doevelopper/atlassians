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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_HEAP_ALLOCATOR_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_HEAP_ALLOCATOR_H

#include <com/github/doevelopper/atlassians/atlas/concurrency/ContiguousPoolManager.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================
        //                            struct HeapAllocator
        //==============================================================================
        /*!
         * @struct HeapAllocator.
         * @brief Provides a heap-based object pool to the underlying ContiguousPoolManager.
         *      The default buffer size is 1000.
         * @tparam T The type to allocate.
         * @note This allocator is thread safe. For internal use only.
         */
        template <typename T>
        struct HeapAllocator : public ContiguousPoolManager<T>
        {
            //------------------------------ Typedefs ----------------------------------
            typedef HeapAllocator<T> this_type;
            typedef T value_type;
            typedef value_type * pointer;
            typedef const value_type * const_pointer;
            typedef value_type & reference;
            typedef const value_type & const_reference;
            typedef size_t size_type;
            typedef uint16_t index_type;
            typedef std::ptrdiff_t difference_type;
            typedef std::true_type propagate_on_container_move_assignment;
            typedef std::false_type propagate_on_container_copy_assignment;
            typedef std::true_type propagate_on_container_swap;
            typedef std::false_type is_always_equal;
            typedef std::false_type default_constructor;
            typedef std::aligned_storage<sizeof(value_type), alignof(value_type)> storage_type;
            typedef typename storage_type::type aligned_type;

            template <typename U>
            struct rebind
            {
                typedef HeapAllocator<U> other;
            };

            //------------------------------- Methods ----------------------------------
            HeapAllocator(index_type size)
                : m_size(size)
                , m_buffer(new aligned_type[size])
            {
                if (!m_buffer)
                {
                    throw std::bad_alloc();
                }
                this->setBuffer(m_buffer, m_size);
            }

            HeapAllocator(const this_type & other)
                : HeapAllocator(other.m_size)
            {
            }

            HeapAllocator(this_type && other)             = default;
            HeapAllocator & operator=(const this_type &)  = delete;
            HeapAllocator & operator=(this_type && other) = delete;

            ~HeapAllocator()
            {
                delete[] m_buffer;
            }

            // Rebound types
            template <typename U>
            HeapAllocator(const HeapAllocator<U> & other)
                : HeapAllocator(other.size())
            {
            }

            template <typename U>
            HeapAllocator(HeapAllocator<U> && other)
                : ContiguousPoolManager<T>(std::move(other))
                // , m_size((index_type)resize<U, T>(other.m_size))
                , m_size(static_cast<index_type>(resize<U, T>(other.m_size)))
                , m_buffer(other.m_buffer)
            {
                other.m_size   = 0;
                other.m_buffer = nullptr;
            }

            template <typename U>
            HeapAllocator & operator=(const HeapAllocator<U> &) = delete;
            template <typename U>
            HeapAllocator & operator=(HeapAllocator<U> &&) = delete;

            static HeapAllocator select_on_container_copy_construction(const HeapAllocator & other)
            {
                return HeapAllocator(other.size());
            }

            bool operator==(const this_type &) const
            {
                return true;
            }

            bool operator!=(const this_type &) const
            {
                return false;
            }

            index_type size() const
            {
                return m_size;
            }

        private:

            //------------------------------- Members ----------------------------------
            index_type m_size;
            aligned_type * m_buffer;
        };
}

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_HEAP_ALLOCATOR_H
