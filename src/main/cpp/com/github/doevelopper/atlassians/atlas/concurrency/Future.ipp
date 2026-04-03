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
#include <com/github/doevelopper/atlassians/atlas/concurrency/Allocator.hpp>

namespace Bloomberg::quantum
{
#ifndef __QUANTUM_USE_DEFAULT_ALLOCATOR
    #ifdef __QUANTUM_ALLOCATE_POOL_FROM_HEAP
        using FutureAllocator = HeapAllocator<Future<int>>;
    #else
        using FutureAllocator = StackAllocator<Future<int>, __QUANTUM_FUTURE_ALLOC_SIZE>;
    #endif
#else
        using FutureAllocator = StlAllocator<Future<int>>;
#endif

        //==============================================================================================
        //                                class IThreadFuture
        //==============================================================================================
        template <class T>
        template <class V>
        NonBufferRetType<V> IThreadFuture<T>::get()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->get();
        }

        template <class T>
        template <class V>
        const NonBufferRetType<V> & IThreadFuture<T>::getRef() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<const Impl *>(this)->getRef();
        }

        template <class T>
        template <class V>
        BufferRetType<V> IThreadFuture<T>::pull(bool & isBufferClosed)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->pull(isBufferClosed);
        }

        //==============================================================================================
        //                                class ICoroFuture
        //==============================================================================================
        template <class T>
        template <class V>
        NonBufferRetType<V> ICoroFuture<T>::get(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->get(sync);
        }

        template <class T>
        template <class V>
        const NonBufferRetType<V> & ICoroFuture<T>::getRef(ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<const Impl *>(this)->getRef(sync);
        }

        template <class T>
        template <class V>
        BufferRetType<V> ICoroFuture<T>::pull(ICoroSync::Ptr sync, bool & isBufferClosed)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->pull(sync, isBufferClosed);
        }

        //==============================================================================================
        //                                class Future
        //==============================================================================================
        template <class T>
        Future<T>::Future(std::shared_ptr<SharedState<T>> sharedState)
            : m_sharedState(sharedState)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        template <class T>
        bool Future<T>::valid() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_sharedState != nullptr;
        }

        template <class T>
        template <class V>
        NonBufferRetType<V> Future<T>::get()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return get(nullptr);
        }

        template <class T>
        template <class V>
        const NonBufferRetType<V> & Future<T>::getRef() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return getRef(nullptr);
        }

        template <class T>
        void Future<T>::wait() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return wait(nullptr);
        }

        template <class T>
        std::future_status Future<T>::waitFor(std::chrono::milliseconds timeMs) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return waitFor(nullptr, timeMs);
        }

        template <class T>
        template <class V>
        NonBufferRetType<V> Future<T>::get(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_sharedState)
                ThrowFutureException(FutureState::NoState);
            return m_sharedState->get(sync);
        }

        template <class T>
        template <class V>
        const NonBufferRetType<V> & Future<T>::getRef(ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_sharedState)
                ThrowFutureException(FutureState::NoState);
            return m_sharedState->getRef(sync);
        }

        template <class T>
        void Future<T>::wait(ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_sharedState)
                ThrowFutureException(FutureState::NoState);
            return m_sharedState->wait(sync);
        }

        template <class T>
        std::future_status Future<T>::waitFor(ICoroSync::Ptr sync, std::chrono::milliseconds timeMs) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_sharedState)
                ThrowFutureException(FutureState::NoState);
            return m_sharedState->waitFor(sync, timeMs);
        }

        template <class T>
        template <class V>
        BufferRetType<V> Future<T>::pull(bool & isBufferClosed)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return pull(nullptr, isBufferClosed);
        }

        template <class T>
        template <class V>
        BufferRetType<V> Future<T>::pull(ICoroSync::Ptr sync, bool & isBufferClosed)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_sharedState)
                ThrowFutureException(FutureState::NoState);
            return m_sharedState->pull(sync, isBufferClosed);
        }

        template <class T>
        void * Future<T>::operator new(size_t)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return Allocator<FutureAllocator>::instance(AllocatorTraits::futureAllocSize()).allocate();
        }

        template <class T>
        void Future<T>::operator delete(void * p)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            Allocator<FutureAllocator>::instance(AllocatorTraits::futureAllocSize())
                .deallocate(static_cast<Future<int> *>(p));
        }

        template <class T>
        void Future<T>::deleter(Future<T> * p)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
#ifndef __QUANTUM_USE_DEFAULT_ALLOCATOR
            Allocator<FutureAllocator>::instance(AllocatorTraits::futureAllocSize())
                .dispose(reinterpret_cast<Future<int> *>(p));
#else
            delete p;
#endif
        }
}
