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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_SHARED_STATE_MUTEX_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_SHARED_STATE_MUTEX_H

#include <memory>
#include <stdexcept>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IContext.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Buffer.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/ConditionVariable.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/FutureState.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Traits.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/YieldingThread.hpp>

namespace Bloomberg::quantum
{
        template <class RET>
        class Promise;

        //==============================================================================================
        //                                 class SharedState
        //==============================================================================================
        /*!
         * @class SharedState.
         * @brief Shared state used between a Promise and a Future to exchange values.
         * @note For internal use only.
         */
        template <class T>
        class SharedState
        {
            friend class Promise<T>;

        public:

            template <class V = T>
            int set(V && value);

            template <class V = T>
            int set(ICoroSync::Ptr sync, V && value);

            // Moves value out of the shared state
            T get();

            T get(ICoroSync::Ptr sync);

            const T & getRef() const;

            const T & getRef(ICoroSync::Ptr sync) const;

            void breakPromise();

            void breakPromise(ICoroSync::Ptr sync);

            void wait() const;

            void wait(ICoroSync::Ptr sync) const;

            template <class REP, class PERIOD>
            std::future_status waitFor(const std::chrono::duration<REP, PERIOD> & time) const;

            template <class REP, class PERIOD>
            std::future_status waitFor(ICoroSync::Ptr sync, const std::chrono::duration<REP, PERIOD> & time) const;

            int setException(std::exception_ptr ex);

            int setException(ICoroSync::Ptr sync, std::exception_ptr ex);

        private:

            template <class... ARGS>
            SharedState(ARGS &&... args);

            void conditionWait() const;

            void conditionWait(ICoroSync::Ptr sync) const;

            void checkPromiseState() const;

            bool stateHasChanged() const;

            // ============================= MEMBERS ==============================
            mutable ConditionVariable _cond;
            mutable Mutex _mutex;
            FutureState _state;
            std::exception_ptr _exception;
            T _value;
            LOG4CXX_DECLARE_STATIC_LOGGER
        };
        template <class T>
        inline log4cxx::LoggerPtr SharedState<T>::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.SharedState" ) );
        //==============================================================================================
        //                       class SharedState<Buffer> (partial specialization)
        //==============================================================================================
        template <class T>
        class SharedState<Buffer<T>>
        {
            friend class Promise<Buffer<T>>;

        public:

            template <class V = T>
            void push(V && value);

            template <class V = T>
            void push(ICoroSync::Ptr sync, V && value);

            T pull(bool & isBufferClosed);

            T pull(ICoroSync::Ptr sync, bool & isBufferClosed);

            void breakPromise();

            void breakPromise(ICoroSync::Ptr sync);

            void wait() const;

            void wait(ICoroSync::Ptr sync) const;

            template <class REP, class PERIOD>
            std::future_status waitFor(const std::chrono::duration<REP, PERIOD> & time) const;

            template <class REP, class PERIOD>
            std::future_status waitFor(ICoroSync::Ptr sync, const std::chrono::duration<REP, PERIOD> & time) const;

            int setException(std::exception_ptr ex);

            int setException(ICoroSync::Ptr sync, std::exception_ptr ex);

            int closeBuffer();

            int closeBuffer(ICoroSync::Ptr sync);

        private:

            SharedState();

            void checkPromiseState() const;

            bool stateHasChanged(BufferStatus status) const;

            // ============================= MEMBERS ==============================
            mutable ConditionVariable m_cond;
            mutable Mutex m_mutex;
            FutureState _state;
            std::exception_ptr m_exception;
            Buffer<T> m_reader;
            Buffer<T> m_writer;
            LOG4CXX_DECLARE_STATIC_LOGGER
        };
        template <class T>
        inline log4cxx::LoggerPtr SharedState<Buffer<T>>::logger =
            log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.SharedState" ) );
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/SharedState.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_SHARED_STATE_MUTEX_H
