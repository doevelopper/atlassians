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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_PROMISE_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_PROMISE_H

#include <atomic>
#include <memory>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IPromise.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Future.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/SharedState.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                class Promise
        //==============================================================================================
        /*!
         * @class Promise
         * @brief Class representing a promised value.
         * @tparam T Type of value returned by the promise.
         * @note See IPromiseBase, IThreadPromise and ICoroPromise interfaces for usage details.
         */
        template <class T>
        class Promise : public IPromiseBase, public IThreadPromise<Promise, T>, public ICoroPromise<Promise, T>
        {
        public:

            using Ptr = std::shared_ptr<Promise<T>>;

            // Constructor
            template <class... ARGS>
            Promise(ARGS &&... args);

            // Move constructor
            Promise(Promise<T> && other)
                : IThreadPromise<Promise, T>(this)
                , ICoroPromise<Promise, T>(this)
                , m_sharedState(std::move(other.m_sharedState))
                , m_terminated(other.m_terminated.load())
            {
            }

            // Move assignment
            Promise & operator=(Promise<T> && other)
            {
                if (this != &other)
                {
                    m_sharedState = std::move(other.m_sharedState);
                    m_terminated  = other.m_terminated.load();
                }
                return *this;
            }

            // Destructor
            ~Promise();

            // Future interface accessors
            IThreadFutureBase::Ptr getIThreadFutureBase() const final;
            ICoroFutureBase::Ptr getICoroFutureBase() const final;
            ThreadFuturePtr<T> getIThreadFuture() const;
            CoroFuturePtr<T> getICoroFuture() const;

            // ITerminate
            void terminate() final;

            // IPromiseBase
            bool valid() const final;
            int setException(std::exception_ptr ex) final;
            int setException(ICoroSync::Ptr sync, std::exception_ptr ex) final;

            // IThreadPromise
            template <class V, class = NonBufferType<T, V>>
            int set(V && value);

            template <class V, class = BufferType<T, V>>
            void push(V && value);

            // ICoroPromise
            template <class V, class = NonBufferType<T, V>>
            int set(ICoroSync::Ptr sync, V && value);

            template <class V, class = BufferType<T, V>>
            void push(ICoroSync::Ptr sync, V && value);

            template <class V = T, class = BufferRetType<V>>
            int closeBuffer();

            template <class V = T, class = BufferRetType<V>>
            int closeBuffer(ICoroSync::Ptr sync);

            //===================================
            //           NEW / DELETE
            //===================================
            static void * operator new(size_t size);
            static void operator delete(void * p);
            static void deleter(Promise<T> * p);

        private:

            std::shared_ptr<SharedState<T>> m_sharedState;
            std::atomic_bool m_terminated;

            LOG4CXX_DECLARE_STATIC_LOGGER
        };

        template <typename T>
        inline log4cxx::LoggerPtr Promise<T>::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.ContiguousPoolManager" ) );

        template <class T>
        using PromisePtr = typename Promise<T>::Ptr;
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/Promise.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_PROMISE_H
