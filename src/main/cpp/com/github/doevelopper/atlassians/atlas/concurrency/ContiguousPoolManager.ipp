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
// NOTE: DO NOT INCLUDE DIRECTLY

// ##############################################################################################
// #################################### IMPLEMENTATIONS #########################################
// ##############################################################################################

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <type_traits>

namespace Bloomberg::quantum
{
        template <typename T>
        ContiguousPoolManager<T>::ContiguousPoolManager()
            : m_control(std::make_shared<Control>())
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        template <typename T>
        ContiguousPoolManager<T>::ContiguousPoolManager(aligned_type * buffer, index_type size)
            : m_control(std::make_shared<Control>())
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            setBuffer(buffer, size);
        }

        template <typename T>
        template <typename U>
        ContiguousPoolManager<T>::ContiguousPoolManager(const ContiguousPoolManager<U> & other)
            : m_control(std::reinterpret_pointer_cast<Control>(other.m_control))
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_control || !m_control->_buffer)
            {
                throw std::invalid_argument("Invalid allocator");
            }
            // normalize size of buffer
            // index_type newSize        = std::min(m_control->_size, (index_type)resize<U, T>(m_control->_size));
            index_type newSize        = std::min(m_control->_size,static_cast<index_type>(resize<U, T>(m_control->_size)));
            m_control->_size           = newSize; // resize buffer
            m_control->_freeBlockIndex = newSize - 1;
        }

        template <typename T>
        template <typename U>
        ContiguousPoolManager<T>::ContiguousPoolManager(ContiguousPoolManager<U> && other)
            : m_control(std::move(other.m_control))
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_control || !m_control->_buffer)
            {
                throw std::invalid_argument("Invalid allocator");
            }
            // normalize size of buffer
            // index_type newSize        = std::min(m_control->_size, (index_type)resize<U, T>(m_control->_size));
            index_type newSize        = std::min(m_control->_size,static_cast<index_type>(resize<U, T>(m_control->_size)));
            m_control->_size           = newSize; // resize buffer
            m_control->_freeBlockIndex = newSize - 1;
        }

        template <typename T>
        void ContiguousPoolManager<T>::setBuffer(aligned_type * buffer, index_type size)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_control)
            {
                throw std::bad_alloc();
            }
            if (!buffer)
            {
                throw std::invalid_argument("Null buffer");
            }
            if (size == 0)
            {
                throw std::invalid_argument("Invalid allocator pool size of zero");
            }
            m_control->_size   = size;
            m_control->_buffer = buffer;
            if (m_control->_freeBlocks)
            {
                delete[] m_control->_freeBlocks;
            }
            m_control->_freeBlocks = new index_type[size];
            if (!m_control->_freeBlocks)
            {
                throw std::bad_alloc();
            }
            // build the free stack
            for (index_type i = 0; i < size; ++i)
            {
                m_control->_freeBlocks[i] = i;
            }
            m_control->_freeBlockIndex = size - 1;
        }

        template <typename T>
        typename ContiguousPoolManager<T>::pointer ContiguousPoolManager<T>::address(reference x) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return &x;
        }

        template <typename T>
        typename ContiguousPoolManager<T>::const_pointer ContiguousPoolManager<T>::address(const_reference x) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return &x;
        }

        template <typename T>
        typename ContiguousPoolManager<T>::size_type ContiguousPoolManager<T>::max_size() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return 1; // only 1 supported for now
        }

        template <typename T>
        template <typename... Args>
        void ContiguousPoolManager<T>::construct(T * p, Args &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            new ((void *)p) T(std::forward<Args>(args)...); // construct in-place
        }

        template <typename T>
        void ContiguousPoolManager<T>::destroy(pointer p)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (p != nullptr)
            {
                p->~T();
            }
        }

        template <typename T>
        typename ContiguousPoolManager<T>::pointer ContiguousPoolManager<T>::allocate(size_type n, const_pointer)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(bufferStart());
            {
                SpinLock::Guard lock(m_control->_spinlock);
                if (findContiguous(static_cast<index_type>(n)))
                {
                    m_control->_freeBlockIndex -= (n - 1);
                    return reinterpret_cast<pointer>(
                        &m_control->_buffer[m_control->_freeBlocks[m_control->_freeBlockIndex--]]);
                }
                // Use heap allocation
                ++m_control->_numHeapAllocatedBlocks;
            }
            return (pointer) new char[sizeof(value_type)];
        }

        template <typename T>
        void ContiguousPoolManager<T>::deallocate(pointer p, size_type n)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (p == nullptr)
            {
                return;
            }
            assert(bufferStart());
            if (isManaged(p))
            {
                // find index of the block and return the individual blocks to the free pool
                SpinLock::Guard lock(m_control->_spinlock);
                for (size_type i = 0; i < n; ++i)
                {
                    m_control->_freeBlocks[++m_control->_freeBlockIndex] = blockIndex(p + i);
                }
            }
            else
            {
                delete[] (char *)p;
                // delete[] static_cast<char *>(p);
                SpinLock::Guard lock(m_control->_spinlock);
                --m_control->_numHeapAllocatedBlocks;
                assert(m_control->_numHeapAllocatedBlocks >= 0);
            }
        }

        template <typename T>
        template <typename... Args>
        typename ContiguousPoolManager<T>::pointer ContiguousPoolManager<T>::create(Args &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            T * p = allocate();
            construct(p, std::forward<Args>(args)...);
            return p;
        }

        template <typename T>
        void ContiguousPoolManager<T>::dispose(pointer p)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            destroy(p);
            deallocate(p);
        }

        template <typename T>
        size_t ContiguousPoolManager<T>::allocatedBlocks() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_control->_size ? m_control->_size - m_control->_freeBlockIndex - 1 : 0;
        }

        template <typename T>
        size_t ContiguousPoolManager<T>::allocatedHeapBlocks() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_control->_numHeapAllocatedBlocks;
        }

        template <typename T>
        bool ContiguousPoolManager<T>::isFull() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_control->_freeBlockIndex == m_control->_size - 1;
        }

        template <typename T>
        bool ContiguousPoolManager<T>::isEmpty() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_control->_freeBlockIndex == -1;
        }

        template <typename T>
        typename ContiguousPoolManager<T>::index_type ContiguousPoolManager<T>::size() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_control->_size;
        }

        template <typename T>
        ContiguousPoolManager<T>::operator bool() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_control != nullptr;
        }

        template <typename T>
        typename ContiguousPoolManager<T>::pointer ContiguousPoolManager<T>::bufferStart()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return reinterpret_cast<pointer>(m_control->_buffer);
        }

        template <typename T>
        typename ContiguousPoolManager<T>::pointer ContiguousPoolManager<T>::bufferEnd()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return reinterpret_cast<pointer>(m_control->_buffer + m_control->_size);
        }

        template <typename T>
        bool ContiguousPoolManager<T>::isManaged(pointer p)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return (bufferStart() <= p) && (p < bufferEnd());
        }

        template <typename T>
        typename ContiguousPoolManager<T>::index_type ContiguousPoolManager<T>::blockIndex(pointer p)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<index_type>(reinterpret_cast<aligned_type *>(p) - m_control->_buffer);
        }

        template <typename T>
        bool ContiguousPoolManager<T>::findContiguous(index_type n)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if ((m_control->_freeBlockIndex + 1) < n)
            {
                return false;
            }
            bool found          = true;
            aligned_type * last = &m_control->_buffer[m_control->_freeBlocks[m_control->_freeBlockIndex]];
            for (ssize_t i = m_control->_freeBlockIndex - 1; i > m_control->_freeBlockIndex - n; --i)
            {
                aligned_type * first = &m_control->_buffer[m_control->_freeBlocks[i]];
                if ((last - first) != (m_control->_freeBlockIndex - i))
                {
                    return false;
                }
            }
            return found;
        }
}
