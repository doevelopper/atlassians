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
#include <cstring>
#include <type_traits>

#if defined(_WIN32) && !defined(__CYGWIN__)
// TODO: Windows headers for memory mapping and page protection
#else
    #include <sys/mman.h>
#endif

#if defined(BOOST_USE_VALGRIND)
    #include <valgrind/valgrind.h>
#endif

namespace Bloomberg::quantum
{
        template <typename STACK_TRAITS>
        CoroutinePoolAllocator<STACK_TRAITS>::CoroutinePoolAllocator(index_type size)
            : m_size(size)
            , m_blocks(nullptr)
            , _freeBlocks(nullptr)
            , m_freeBlockIndex(size - 1)
            , // point to the last element
            m_numHeapAllocatedBlocks(0)
            , m_stackSize(std::min(std::max(traits::default_size(), traits::minimum_size()), traits::maximum_size()))
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if ((m_size == 0) || (m_stackSize == 0))
            {
                throw std::invalid_argument("Invalid coroutine allocator size of zero");
            }
            // Make sure the stack is a multiple of the system page size
            size_t remainder = m_stackSize % traits::page_size();
            if (remainder > 0)
            {
                // extend to the next page
                m_stackSize += (traits::page_size() - remainder);
            }
            _freeBlocks = new index_type[size];
            if (!_freeBlocks)
            {
                throw std::bad_alloc();
            }
            m_blocks = new uint8_t *[size];
            if (!m_blocks)
            {
                delete[] _freeBlocks;
                throw std::bad_alloc();
            }
            // pre-allocate all the coroutine stack blocks and protect the last stack page to
            // track coroutine stack overflows.
            for (size_t i = 0; i < size; ++i)
            {
                m_blocks[i] = allocateCoroutine(ProtectMemPage::On);
                if (!m_blocks[i])
                {
                    deallocateBlocks(i);
                    throw std::bad_alloc();
                }
                // set the block position
                header(m_blocks[i])->_pos = i;
            }
            // initialize the free block list
            std::iota(_freeBlocks, _freeBlocks + size, 0);
        }

        template <typename STACK_TRAITS>
        CoroutinePoolAllocator<STACK_TRAITS>::CoroutinePoolAllocator(
            CoroutinePoolAllocator<STACK_TRAITS> && other) noexcept
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            *this = other;
        }

        template <typename STACK_TRAITS>
        CoroutinePoolAllocator<STACK_TRAITS> &
        CoroutinePoolAllocator<STACK_TRAITS>::operator=(CoroutinePoolAllocator<STACK_TRAITS> && other)
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_size                   = other.m_size;
            m_blocks                 = other.m_blocks;
            _freeBlocks             = other._freeBlocks;
            m_freeBlockIndex         = other.m_freeBlockIndex;
            m_numHeapAllocatedBlocks = other.m_numHeapAllocatedBlocks;

            // Reset other
            other.m_blocks                 = nullptr;
            other._freeBlocks             = nullptr;
            other.m_freeBlockIndex         = -1;
            other.m_numHeapAllocatedBlocks = 0;
        }

        template <typename STACK_TRAITS>
        CoroutinePoolAllocator<STACK_TRAITS>::~CoroutinePoolAllocator()
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            deallocateBlocks(m_size);
        }

        template <typename STACK_TRAITS>
        void CoroutinePoolAllocator<STACK_TRAITS>::deallocateBlocks(size_t pos)
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            for (size_t j = 0; j < pos; ++j)
            {
                deallocateCoroutine(m_blocks[j]);
            }
            delete[] m_blocks;
            delete[] _freeBlocks;
        }

        template <typename STACK_TRAITS>
        uint8_t * CoroutinePoolAllocator<STACK_TRAITS>::allocateCoroutine(ProtectMemPage protect) const
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
#if defined(_WIN32) && !defined(__CYGWIN__)
            return new uint8_t[_stackSize];
#else
            uint8_t * block = (uint8_t *)mmap(
                nullptr, m_stackSize, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE,
                -1, // invalid fd
                0); // no offset
            if (block == MAP_FAILED)
            {
                return nullptr;
            }
            // Add protection to the lowest page
            if ((protect == ProtectMemPage::On) && mprotect(block, traits::page_size(), PROT_NONE) != 0)
            {
                munmap(block, m_stackSize); // free region
                return nullptr;
            }
            return block;
#endif
        }

        template <typename STACK_TRAITS>
        int CoroutinePoolAllocator<STACK_TRAITS>::deallocateCoroutine(uint8_t * block) const
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            assert(block);
#if defined(_WIN32) && !defined(__CYGWIN__)
            delete[] block;
            return 0;
#else
            return munmap(block, m_stackSize);
#endif
        }

        template <typename STACK_TRAITS>
        boost::context::stack_context CoroutinePoolAllocator<STACK_TRAITS>::allocate()
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            uint8_t * block = nullptr;
            {
                SpinLock::Guard lock(m_spinlock);
                if (!isEmpty())
                {
                    block = m_blocks[_freeBlocks[m_freeBlockIndex--]];
                }
            }
            if (!block)
            {
                // Do not protect last memory page for performance reasons
                block = allocateCoroutine(ProtectMemPage::Off);
                if (!block)
                {
                    throw std::bad_alloc();
                }
                header(block)->_pos = -1; // mark position as non-managed
                SpinLock::Guard lock(m_spinlock);
                ++m_numHeapAllocatedBlocks;
            }
            // populate stack context
            boost::context::stack_context ctx;
            ctx.size = m_stackSize - sizeof(Header);
            ctx.sp   = block + ctx.size;
#if defined(BOOST_USE_VALGRIND)
            ctx.valgrind_stack_id = VALGRIND_STACK_REGISTER(ctx.sp, block);
#endif
            return ctx;
        }

        template <typename STACK_TRAITS>
        void CoroutinePoolAllocator<STACK_TRAITS>::deallocate(const boost::context::stack_context & ctx)
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!ctx.sp)
            {
                return;
            }
#if defined(BOOST_USE_VALGRIND)
            VALGRIND_STACK_DEREGISTER(ctx.valgrind_stack_id);
#endif
            int bi = blockIndex(ctx);
            assert(bi >= -1 && bi < m_size); // guard against coroutine stack overflow or corruption
            if (isManaged(ctx))
            {
                // find index of the block
                SpinLock::Guard lock(m_spinlock);
                _freeBlocks[++m_freeBlockIndex] = bi;
            }
            else
            {
                // Unlink coroutine stack
                {
                    SpinLock::Guard lock(m_spinlock);
                    --m_numHeapAllocatedBlocks;
//                    assert(m_numHeapAllocatedBlocks >= 0);
                }
                if (deallocateCoroutine(stackEnd(ctx)) != 0)
                {
                    throw std::runtime_error("Bad de-allocation");
                }
            }
        }

        template <typename STACK_TRAITS>
        size_t CoroutinePoolAllocator<STACK_TRAITS>::allocatedBlocks() const
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_size - m_freeBlockIndex - 1;
        }

        template <typename STACK_TRAITS>
        size_t CoroutinePoolAllocator<STACK_TRAITS>::allocatedHeapBlocks() const
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_numHeapAllocatedBlocks;
        }

        template <typename STACK_TRAITS>
        bool CoroutinePoolAllocator<STACK_TRAITS>::isFull() const
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_freeBlockIndex == m_size - 1;
        }

        template <typename STACK_TRAITS>
        bool CoroutinePoolAllocator<STACK_TRAITS>::isEmpty() const
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_freeBlockIndex == -1;
        }

        template <typename STACK_TRAITS>
        typename CoroutinePoolAllocator<STACK_TRAITS>::Header *
        CoroutinePoolAllocator<STACK_TRAITS>::header(const boost::context::stack_context & ctx) const
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return reinterpret_cast<Header *>(ctx.sp);
        }

        template <typename STACK_TRAITS>
        typename CoroutinePoolAllocator<STACK_TRAITS>::Header *
        CoroutinePoolAllocator<STACK_TRAITS>::header(uint8_t * block) const
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return reinterpret_cast<Header *>(block + m_stackSize - sizeof(Header));
        }

        template <typename STACK_TRAITS>
        uint8_t * CoroutinePoolAllocator<STACK_TRAITS>::stackEnd(const boost::context::stack_context & ctx) const
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<uint8_t *>(ctx.sp) - ctx.size;
        }

        template <typename STACK_TRAITS>
        bool CoroutinePoolAllocator<STACK_TRAITS>::isManaged(const boost::context::stack_context & ctx) const
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return blockIndex(ctx) != -1;
        }

        template <typename STACK_TRAITS>
        int CoroutinePoolAllocator<STACK_TRAITS>::blockIndex(const boost::context::stack_context & ctx) const
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return header(ctx)->_pos;
        }
}
