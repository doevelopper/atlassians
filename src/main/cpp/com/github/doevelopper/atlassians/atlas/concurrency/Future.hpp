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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_FUTURE_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_FUTURE_H

#include <stdexcept>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IContext.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IFuture.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/SharedState.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                   class Future
        //==============================================================================================
        /*!
         * @class Future
         * @brief Class representing a promised future. Can only be instantiated via a Promise object.
         * @tparam T Type of value returned by the future object.
         * @note See IThreadFuture and ICoroFuture interfaces for usage details. An instance of this class
         *      can only be obtained via a Promise object and cannot be created on its own.
         */
        template <class T>
        class Future : public IThreadFuture<T>, public ICoroFuture<T>
        {
        public:

            template <class F>
            friend class Promise;
            using Ptr = std::shared_ptr<Future<T>>;

            // Default constructor with empty state
            Future() = default;

            bool valid() const final;

            // IThreadFutureBase
            void wait() const final;
            std::future_status waitFor(std::chrono::milliseconds timeMs) const final;

            // IThreadFuture
            template <class V = T>
            NonBufferRetType<V> get();

            template <class V = T>
            const NonBufferRetType<V> & getRef() const;

            template <class V = T>
            BufferRetType<V> pull(bool & isBufferClosed);

            // ICoroFutureBase
            void wait(ICoroSync::Ptr sync) const final;
            std::future_status waitFor(ICoroSync::Ptr sync, std::chrono::milliseconds timeMs) const final;

            // ICoroFuture
            template <class V = T>
            NonBufferRetType<V> get(ICoroSync::Ptr sync);

            template <class V = T>
            const NonBufferRetType<V> & getRef(ICoroSync::Ptr sync) const;

            template <class V = T>
            BufferRetType<V> pull(ICoroSync::Ptr sync, bool & isBufferClosed);

            //===================================
            //           NEW / DELETE
            //===================================
            static void * operator new(size_t size);
            static void operator delete(void * p);
            static void deleter(Future<T> * p);

        private:

            explicit Future(std::shared_ptr<SharedState<T>> sharedState);

            // Members
            std::shared_ptr<SharedState<T>> m_sharedState;
            LOG4CXX_DECLARE_STATIC_LOGGER
        };

        template <typename T>
        inline log4cxx::LoggerPtr Future<T>::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.Future" ) );

        template <class T>
        using FuturePtr = typename Future<T>::Ptr;
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/Future.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_FUTURE_H
