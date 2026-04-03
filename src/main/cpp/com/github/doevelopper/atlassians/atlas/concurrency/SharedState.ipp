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

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                                     class SharedState
        //==============================================================================================
        template <class T>
        template <class... ARGS>
        SharedState<T>::SharedState(ARGS &&... args)
            : _state(FutureState::PromiseNotSatisfied)
            , _value(T(std::forward<ARGS>(args)...))
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        template <class T>
        template <class V>
        int SharedState<T>::set(V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return set(nullptr, std::forward<V>(value));
        }

        template <class T>
        template <class V>
        int SharedState<T>::set(ICoroSync::Ptr sync, V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            {
                //========= LOCKED SCOPE =========
                Mutex::Guard lock(sync, _mutex);
                if (_state != FutureState::PromiseNotSatisfied)
                {
                    ThrowFutureException(_state);
                }
                _value = std::forward<V>(value);
                _state = FutureState::PromiseAlreadySatisfied;
            }
            _cond.notifyAll(sync);
            return 0;
        }

        template <class T>
        T SharedState<T>::get()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return get(nullptr);
        }

        template <class T>
        const T & SharedState<T>::getRef() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return getRef(nullptr);
        }

        template <class T>
        T SharedState<T>::get(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            //========= LOCKED SCOPE =========
            Mutex::Guard lock(sync, _mutex);
            conditionWait(sync);
            _state = FutureState::FutureAlreadyRetrieved;
            return std::move(_value);
        }

        template <class T>
        const T & SharedState<T>::getRef(ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            //========= LOCKED SCOPE =========
            Mutex::Guard lock(sync, _mutex);
            conditionWait(sync);
            return _value;
        }

        template <class T>
        void SharedState<T>::breakPromise()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return breakPromise(nullptr);
        }

        template <class T>
        void SharedState<T>::breakPromise(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            { //========= LOCKED SCOPE =========
                Mutex::Guard lock(sync, _mutex);
                if (_state == FutureState::PromiseNotSatisfied)
                {
                    _state = FutureState::BrokenPromise;
                }
            }
            _cond.notifyAll(sync);
        }

        template <class T>
        void SharedState<T>::wait() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return wait(nullptr);
        }

        template <class T>
        void SharedState<T>::wait(ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            //========= LOCKED SCOPE =========
            Mutex::Guard lock(sync, _mutex);
            _cond.wait(sync, _mutex, [this]() -> bool { return stateHasChanged(); });
        }

        template <class T>
        template <class REP, class PERIOD>
        std::future_status SharedState<T>::waitFor(const std::chrono::duration<REP, PERIOD> & time) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return waitFor(nullptr, time);
        }

        template <class T>
        template <class REP, class PERIOD>
        std::future_status
        SharedState<T>::waitFor(ICoroSync::Ptr sync, const std::chrono::duration<REP, PERIOD> & time) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            //========= LOCKED SCOPE =========
            Mutex::Guard lock(sync, _mutex);
            _cond.waitFor(sync, _mutex, time, [this]() -> bool { return stateHasChanged(); });
            return _state == FutureState::PromiseNotSatisfied ? std::future_status::timeout : std::future_status::ready;
        }

        template <class T>
        int SharedState<T>::setException(std::exception_ptr ex)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return setException(nullptr, ex);
        }

        template <class T>
        int SharedState<T>::setException(ICoroSync::Ptr sync, std::exception_ptr ex)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            { //========= LOCKED SCOPE =========
                Mutex::Guard lock(sync, _mutex);
                _exception = ex;
            }
            _cond.notifyAll(sync);
            return -1;
        }

        template <class T>
        void SharedState<T>::conditionWait() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return conditionWait(nullptr);
        }

        template <class T>
        void SharedState<T>::conditionWait(ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            _cond.wait(sync, _mutex, [this]() -> bool { return stateHasChanged(); });
            checkPromiseState();
        }

        template <class T>
        void SharedState<T>::checkPromiseState() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (_exception)
            {
                std::rethrow_exception(_exception);
            }
            if ((_state == FutureState::BrokenPromise) || (_state == FutureState::FutureAlreadyRetrieved))
            {
                ThrowFutureException(_state);
            }
        }

        template <class T>
        bool SharedState<T>::stateHasChanged() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return (_state != FutureState::PromiseNotSatisfied) || (_exception != nullptr);
        }

        //==============================================================================================
        //                       class SharedState<Buffer> (partial specialization)
        //==============================================================================================
        template <class T>
        SharedState<Buffer<T>>::SharedState()
            : _state(FutureState::PromiseNotSatisfied)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        template <class T>
        void SharedState<Buffer<T>>::breakPromise()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return breakPromise(nullptr);
        }

        template <class T>
        void SharedState<Buffer<T>>::breakPromise(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            { //========= LOCKED SCOPE =========
                Mutex::Guard lock(sync, m_mutex);
                if ((_state == FutureState::PromiseNotSatisfied) || (_state == FutureState::BufferingData))
                {
                    _state = FutureState::BrokenPromise;
                }
            }
            m_cond.notifyAll(sync);
        }

        template <class T>
        void SharedState<Buffer<T>>::wait() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return wait(nullptr);
        }

        template <class T>
        void SharedState<Buffer<T>>::wait(ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_reader.empty())
            {
                return; // there is still data available
            }
            //========= LOCKED SCOPE =========
            Mutex::Guard lock(sync, m_mutex);
            m_cond.wait(sync, m_mutex, [this]() -> bool {
                BufferStatus status = m_writer.empty()
                    ? (m_writer.isClosed() ? BufferStatus::Closed : BufferStatus::DataPending)
                                        : BufferStatus::DataPosted;
                return stateHasChanged(status);
            });
        }

        template <class T>
        template <class REP, class PERIOD>
        std::future_status SharedState<Buffer<T>>::waitFor(const std::chrono::duration<REP, PERIOD> & time) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return waitFor(nullptr, time);
        }

        template <class T>
        template <class REP, class PERIOD>
        std::future_status
        SharedState<Buffer<T>>::waitFor(ICoroSync::Ptr sync, const std::chrono::duration<REP, PERIOD> & time) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_reader.empty())
            {
                return std::future_status::ready; // there is still data available
            }
            //========= LOCKED SCOPE =========
            Mutex::Guard lock(sync, m_mutex);
            m_cond.waitFor(sync, m_mutex, time, [this]() -> bool {
                BufferStatus status = m_writer.empty()
                    ? (m_writer.isClosed() ? BufferStatus::Closed : BufferStatus::DataPending)
                                        : BufferStatus::DataPosted;
                return stateHasChanged(status);
            });
            return (m_writer.empty() && !m_writer.isClosed()) ? std::future_status::timeout : std::future_status::ready;
        }

        template <class T>
        int SharedState<Buffer<T>>::setException(std::exception_ptr ex)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return setException(nullptr, ex);
        }

        template <class T>
        int SharedState<Buffer<T>>::setException(ICoroSync::Ptr sync, std::exception_ptr ex)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            { //========= LOCKED SCOPE =========
                Mutex::Guard lock(sync, m_mutex);
                m_exception = ex;
            }
            m_cond.notifyAll();
            return -1;
        }

        template <class T>
        template <class V>
        void SharedState<Buffer<T>>::push(V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            push(nullptr, std::forward<V>(value));
        }

        template <class T>
        template <class V>
        void SharedState<Buffer<T>>::push(ICoroSync::Ptr sync, V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            { //========= LOCKED SCOPE =========
                Mutex::Guard lock(sync, m_mutex);
                if ((_state != FutureState::PromiseNotSatisfied) && (_state != FutureState::BufferingData))
                {
                    ThrowFutureException(_state);
                }
                BufferStatus status = m_writer.push(std::forward<V>(value));
                if (status == BufferStatus::Closed)
                {
                    ThrowFutureException(FutureState::BufferClosed);
                }
                _state = FutureState::BufferingData;
            }
            m_cond.notifyAll(sync);
        }

        template <class T>
        T SharedState<Buffer<T>>::pull(bool & isBufferClosed)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return pull(nullptr, isBufferClosed);
        }

        template <class T>
        T SharedState<Buffer<T>>::pull(ICoroSync::Ptr sync, bool & isBufferClosed)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            T out {};
            if (!m_reader.empty())
            {
                m_reader.pull(out);
                return out;
            }
            { //========= LOCKED SCOPE =========
                Mutex::Guard lock(sync, m_mutex);
                m_cond.wait(sync, m_mutex, [this]() -> bool {
                    BufferStatus status = m_writer.empty()
                        ? (m_writer.isClosed() ? BufferStatus::Closed : BufferStatus::DataPending)
                                            : BufferStatus::DataPosted;
                    bool changed        = stateHasChanged(status);
                    if (changed)
                    {
                        // Move the writer to the reader for consumption
                        m_reader = std::move(m_writer);
                    }
                    return changed;
                });
            }
            isBufferClosed = m_reader.empty() && m_reader.isClosed();
            if (isBufferClosed)
            {
                // Mark the future as fully retrieved
                _state = FutureState::FutureAlreadyRetrieved;
                return out;
            }
            m_reader.pull(out);
            checkPromiseState();
            return out;
        }

        template <class T>
        int SharedState<Buffer<T>>::closeBuffer()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return closeBuffer(nullptr);
        }

        template <class T>
        int SharedState<Buffer<T>>::closeBuffer(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            { //========= LOCKED SCOPE =========
                Mutex::Guard lock(sync, m_mutex);
                if ((_state == FutureState::PromiseNotSatisfied) || (_state == FutureState::BufferingData))
                {
                    _state = FutureState::BufferClosed;
                }
                m_writer.close();
            }
            m_cond.notifyAll(sync);
            return 0;
        }

        template <class T>
        void SharedState<Buffer<T>>::checkPromiseState() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_exception)
            {
                std::rethrow_exception(m_exception);
            }
            if ((_state == FutureState::BrokenPromise) || (_state == FutureState::FutureAlreadyRetrieved))
            {
                ThrowFutureException(_state);
            }
        }

        template <class T>
        bool SharedState<Buffer<T>>::stateHasChanged(BufferStatus status) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return ((status == BufferStatus::DataPosted) || (status == BufferStatus::DataReceived)
                    || (status == BufferStatus::Closed))
                || ((_state == FutureState::BrokenPromise) || (_state == FutureState::FutureAlreadyRetrieved))
                || (m_exception != nullptr);
        }
}
