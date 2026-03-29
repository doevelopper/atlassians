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
        //==============================================================================================
        //                                class IThreadPromise
        //==============================================================================================
        template <template <class> class PROMISE, class T>
        template <class V, class>
        int IThreadPromise<PROMISE, T>::set(V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->set(std::forward<V>(value));
        }

        template <template <class> class PROMISE, class T>
        template <class V, class>
        void IThreadPromise<PROMISE, T>::push(V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            static_cast<Impl *>(this)->push(std::forward<V>(value));
        }

        template <template <class> class PROMISE, class T>
        template <class V, class>
        int IThreadPromise<PROMISE, T>::closeBuffer()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->closeBuffer();
        }

        //==============================================================================================
        //                                class ICoroPromise
        //==============================================================================================
        template <template <class> class PROMISE, class T>
        template <class V, class>
        int ICoroPromise<PROMISE, T>::set(ICoroSync::Ptr sync, V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->set(sync, std::forward<V>(value));
        }

        template <template <class> class PROMISE, class T>
        template <class V, class>
        void ICoroPromise<PROMISE, T>::push(ICoroSync::Ptr sync, V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            static_cast<Impl *>(this)->push(sync, std::forward<V>(value));
        }

        template <template <class> class PROMISE, class T>
        template <class V, class>
        int ICoroPromise<PROMISE, T>::closeBuffer()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->closeBuffer();
        }

//==============================================================================================
//                                class Promise
//==============================================================================================
#ifndef __QUANTUM_USE_DEFAULT_ALLOCATOR
    #ifdef __QUANTUM_ALLOCATE_POOL_FROM_HEAP
        using PromiseAllocator = HeapAllocator<Promise<int>>;
    #else
        using PromiseAllocator = StackAllocator<Promise<int>, __QUANTUM_PROMISE_ALLOC_SIZE>;
    #endif
#else
        using PromiseAllocator = StlAllocator<Promise<int>>;
#endif

        template <class T>
        template <class... ARGS>
        Promise<T>::Promise(ARGS &&... args)
            : IThreadPromise<Promise, T>(this)
            , ICoroPromise<Promise, T>(this)
            , m_sharedState(new SharedState<T>(std::forward<ARGS>(args)...))
            , m_terminated(false)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        template <class T>
        Promise<T>::~Promise()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            terminate();
        }

        template <class T>
        void Promise<T>::terminate()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            bool value {false};
            if (m_terminated.compare_exchange_strong(value, true))
            {
                if (m_sharedState)
                    m_sharedState->breakPromise(local::context());
            }
        }

        template <class T>
        bool Promise<T>::valid() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_sharedState != nullptr;
        }

        template <class T>
        int Promise<T>::setException(std::exception_ptr ex)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return setException(nullptr, ex);
        }

        template <class T>
        int Promise<T>::setException(ICoroSync::Ptr sync, std::exception_ptr ex)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_sharedState)
                ThrowFutureException(FutureState::NoState);
            return m_sharedState->setException(sync, ex);
        }

        template <class T>
        IThreadFutureBase::Ptr Promise<T>::getIThreadFutureBase() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_sharedState)
                ThrowFutureException(FutureState::NoState);
            return FuturePtr<T>(new Future<T>(m_sharedState), Future<T>::deleter);
        }

        template <class T>
        ICoroFutureBase::Ptr Promise<T>::getICoroFutureBase() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_sharedState)
                ThrowFutureException(FutureState::NoState);
            return FuturePtr<T>(new Future<T>(m_sharedState), Future<T>::deleter);
        }

        template <class T>
        template <class V, class>
        int Promise<T>::set(V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return set(nullptr, std::forward<V>(value));
        }

        template <class T>
        ThreadFuturePtr<T> Promise<T>::getIThreadFuture() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_sharedState)
                ThrowFutureException(FutureState::NoState);
            return FuturePtr<T>(new Future<T>(m_sharedState), Future<T>::deleter);
        }

        template <class T>
        template <class V, class>
        int Promise<T>::set(ICoroSync::Ptr sync, V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_sharedState)
                ThrowFutureException(FutureState::NoState);
            return m_sharedState->set(sync, std::forward<V>(value));
        }

        template <class T>
        CoroFuturePtr<T> Promise<T>::getICoroFuture() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_sharedState)
                ThrowFutureException(FutureState::NoState);
            return FuturePtr<T>(new Future<T>(m_sharedState), Future<T>::deleter);
        }

        template <class T>
        template <class V, class>
        void Promise<T>::push(V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            push(nullptr, std::forward<V>(value));
        }

        template <class T>
        template <class V, class>
        void Promise<T>::push(ICoroSync::Ptr sync, V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_sharedState)
                ThrowFutureException(FutureState::NoState);
            m_sharedState->push(sync, std::forward<V>(value));
        }

        template <class T>
        template <class V, class>
        int Promise<T>::closeBuffer()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return closeBuffer(nullptr);
        }

        template <class T>
        template <class V, class>
        int Promise<T>::closeBuffer(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_sharedState)
                ThrowFutureException(FutureState::NoState);
            return m_sharedState->closeBuffer(sync);
        }

        template <class T>
        void * Promise<T>::operator new(size_t)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return Allocator<PromiseAllocator>::instance(AllocatorTraits::promiseAllocSize()).allocate();
        }

        template <class T>
        void Promise<T>::operator delete(void * p)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            Allocator<PromiseAllocator>::instance(AllocatorTraits::promiseAllocSize())
                .deallocate(static_cast<Promise<int> *>(p));
        }

        template <class T>
        void Promise<T>::deleter(Promise<T> * p)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
#ifndef __QUANTUM_USE_DEFAULT_ALLOCATOR
            Allocator<PromiseAllocator>::instance(AllocatorTraits::promiseAllocSize())
                .dispose(reinterpret_cast<Promise<int> *>(p));
#else
            delete p;
#endif
        }
}
