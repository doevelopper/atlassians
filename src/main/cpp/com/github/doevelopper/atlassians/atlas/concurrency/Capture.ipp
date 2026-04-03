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
        //                                   class Capture
        //==============================================================================================
        template <typename RET, typename FUNC, typename... ARGS>
        template <typename F, typename... T>
        Capture<RET, FUNC, ARGS...>::Capture(F && func, T &&... args)
            : _func(std::forward<F>(func))
            , _args(std::forward<T>(args)...) // pack
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        template <typename RET, typename FUNC, typename... ARGS>
        template <typename... T>
        RET Capture<RET, FUNC, ARGS...>::operator()(T &&... t)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return apply<RET>(_func, std::move(_args), std::forward<T>(t)...); // fwd
        }

        template <typename RET, typename FUNC, typename... ARGS>
        Capture<RET, FUNC, ARGS...> makeCapture(FUNC && func, ARGS &&... args)
        {
            // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return Capture<RET, FUNC, ARGS...>(std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        //==============================================================================================
        //                                   class Function
        //==============================================================================================

        template <typename RET, typename... ARGS>
        Function<RET(ARGS...)>::Function(RET (*ptr)(ARGS...))
            : m_callable(reinterpret_cast<void *>(ptr))
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_invoker = [](void * ptr, ARGS... args) -> RET {
                return (*reinterpret_cast<Func>(ptr))(std::forward<ARGS>(args)...);
            };
        }

        template <typename RET, typename... ARGS>
        template <typename FUNCTOR>
        Function<RET(ARGS...)>::Function(FUNCTOR && functor)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            initFunctor(std::forward<FUNCTOR>(functor), std::is_lvalue_reference<FUNCTOR>());
        }

        template <typename RET, typename... ARGS>
        Function<RET(ARGS...)>::Function(Function<RET(ARGS...)> && other)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            *this = std::move(other);
        }

        template <typename RET, typename... ARGS>
        Function<RET(ARGS...)> & Function<RET(ARGS...)>::operator=(Function<RET(ARGS...)> && other)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (this != &other)
            {
                this->~Function(); // delete current
                m_invoker    = other.m_invoker;
                m_destructor = other.m_destructor;
                m_deleter    = other.m_deleter;
                if (other.m_callable == other.m_storage.data())
                {
                    // copy byte-wise data
                    // Note that in this case the destructor will be called on a seemingly different object than the
                    // constructor however this is valid.
                    m_storage  = other.m_storage;
                    m_callable = m_storage.data();
                }
                else
                {
                    m_callable = other.m_callable; // steal buffer
                }
                other.m_callable = nullptr; // disable other callable
            }
            return *this;
        }

        template <typename RET, typename... ARGS>
        Function<RET(ARGS...)>::~Function()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_destructor)
                m_destructor(m_callable);
            if (m_deleter)
                m_deleter(m_callable);
        }

        template <typename RET, typename... ARGS>
        RET Function<RET(ARGS...)>::operator()(ARGS... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_invoker(m_callable, std::forward<ARGS>(args)...);
        }

        template <typename RET, typename... ARGS>
        Function<RET(ARGS...)>::operator bool() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return !!m_callable;
        }

        template <typename RET, typename... ARGS>
        template <typename FUNCTOR>
        void Function<RET(ARGS...)>::initFunctor(FUNCTOR && functor, std::true_type)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_callable = std::addressof(functor);
            m_invoker  = [](void * ptr, ARGS... args) -> RET {
                return (*reinterpret_cast<FUNCTOR *>(ptr))(std::forward<ARGS>(args)...);
            };
        }

        template <typename RET, typename... ARGS>
        template <typename FUNCTOR>
        void Function<RET(ARGS...)>::initFunctor(FUNCTOR && functor, std::false_type)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_destructor = [](void * ptr) {
                if (!ptr)
                    return;
                reinterpret_cast<FUNCTOR *>(ptr)->~FUNCTOR(); // invoke destructor
            };
            if (sizeof(FUNCTOR) <= size)
            {
                new (m_storage.data()) FUNCTOR(std::forward<FUNCTOR>(functor));
                m_callable = m_storage.data();
            }
            else
            {
                m_callable = new char[sizeof(FUNCTOR)];
                new (m_callable) FUNCTOR(std::forward<FUNCTOR>(functor));
                m_deleter = deleter;
            }
            m_invoker = [](void * ptr, ARGS... args) -> RET {
                return (*reinterpret_cast<FUNCTOR *>(ptr))(std::forward<ARGS>(args)...);
            };
        }
}
