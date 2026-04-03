/*****************************************************************************
 * Try.hpp
 *
 * Created: 31/10/2023 2023 by hroland
 *
 * Copyright 2023 hroland. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#ifndef TRY_HPP
#define TRY_HPP

#include <cassert>
#include <exception>
#include <functional>
#include <new>
#include <stdexcept>
#include <utility>
#include <variant>

#include <com/github/doevelopper/atlassians/atlas/concurrency/util/SimplestType.hpp>

namespace Bloomberg::quantum
{

    /*!
     *  @brief Different from assert, logicAssert is meaningful in
     *      release mode. logicAssert should be used in case that
     *      we need to make assumption for users.
     *      In another word, if assert fails, it means there is
     *      a bug in the library. If logicAssert fails, it means
     *      there is a bug in the user code.
     */
    inline void logicAssert(bool x, const char * errorMsg)
    {
        if (x) [[likely]]
        {
            return;
        }
        throw std::logic_error(errorMsg);
    }

    // Forward declaration
    template <typename T>
    class Try;

    template <>
    class Try<void>;

    /*!
     *  @brief Try<T> contains either an instance of T, an exception, or nothing.
     *     Try<T>::value() will return the instance.
     *     If exception or nothing inside, Try<T>::value() would throw an exception.
     *
     *    You can create a Try<T> by:
     *        1. default constructor: contains nothing.
     *        2. construct from exception_ptr.
     *        3. construct from value T.
     *        4. moved from another Try<T> instance.
     */
    template <typename T>
    class Try
    {
        Try(const Try &) = delete;
        //        Try & operator=(Try &&) = delete;
        Try & operator=(const Try &) = delete;

    public:

        Try() noexcept;
        //        Try(Try&&) noexcept =  default;
        virtual ~Try() noexcept;
        Try(Try<T> && other)             = default;
        Try & operator=(Try<T> && other) = default;

        template <typename T2 = T>
        Try(std::enable_if_t<std::is_same<SimplestType, T2>::value, const Try<void> &> other)
        {
            if (other.hasError())
            {
                m_value.template emplace<std::exception_ptr>(other.m_value);
            }
            else
            {
                m_value.template emplace<T>();
            }
        }

        Try & operator=(std::exception_ptr error)
        {
            if (std::holds_alternative<std::exception_ptr>(m_value) && std::get<std::exception_ptr>(m_value) == error)
            {
                return *this;
            }

            m_value.template emplace<std::exception_ptr>(error);
            return *this;
        }

        template <class... U>
        Try(U &&... value)
        requires std::is_constructible_v<T, U...>
            : m_value(std::in_place_type<T>, std::forward<U>(value)...)
        {
        }

        Try(std::exception_ptr error)
            : m_value(error)
        {
        }

        constexpr bool available() const noexcept
        {
            return !std::holds_alternative<std::monostate>(m_value);
        }

        constexpr bool hasError() const noexcept
        {
            return std::holds_alternative<std::exception_ptr>(m_value);
        }

        const T & value() const &
        {
            checkHasTry();
            return std::get<T>(m_value);
        }

        T & value() &
        {
            checkHasTry();
            return std::get<T>(m_value);
        }

        T && value() &&
        {
            checkHasTry();
            return std::move(std::get<T>(m_value));
        }

        const T && value() const &&
        {
            checkHasTry();
            return std::move(std::get<T>(m_value));
        }

        template <class... Args>
        T & emplace(Args &&... args)
        {
            return m_value.template emplace<T>(std::forward<Args>(args)...);
        }

        void setException(std::exception_ptr error)
        {
            if (std::holds_alternative<std::exception_ptr>(m_value) && std::get<std::exception_ptr>(m_value) == error)
            {
                return;
            }
            m_value.template emplace<std::exception_ptr>(error);
        }

        std::exception_ptr getException() const
        {
            logicAssert(std::holds_alternative<std::exception_ptr>(m_value), "Try object do not has an error");
            return std::get<std::exception_ptr>(m_value);
        }

        operator Try<void>() const;

    protected:
    private:

        __attribute__((__always_inline__)) inline void checkHasTry() const
        {
            if (std::holds_alternative<T>(m_value)) [[likely]]
            {
                return;
            }
            else if (std::holds_alternative<std::exception_ptr>(m_value))
            {
                std::rethrow_exception(std::get<std::exception_ptr>(m_value));
            }
            else if (std::holds_alternative<std::monostate>(m_value))
            {
                throw std::logic_error("Try object is empty");
            }
            else
            {
                assert(false);
            }
        }

        std::variant<std::monostate, T, std::exception_ptr> m_value;
        friend Try<SimplestType>;
    };

    template <>
    class Try<void>
    {
    public:

        Try() { }

        Try(std::exception_ptr error)
            : m_error(error)
        {
        }

        Try & operator=(std::exception_ptr error)
        {
            m_error = error;
            return *this;
        }

        Try(Try && other)
            : m_error(std::move(other.m_error))
        {
        }

        Try & operator=(Try && other)
        {
            if (this != &other)
            {
                std::swap(m_error, other.m_error);
            }
            return *this;
        }

        void value()
        {
            if (m_error)
            {
                std::rethrow_exception(m_error);
            }
        }

        bool hasError() const
        {
            return m_error.operator bool();
        }

        void setException(std::exception_ptr error)
        {
            m_error = error;
        }

        std::exception_ptr getException()
        {
            return m_error;
        }

    private:

        std::exception_ptr m_error;
        friend Try<SimplestType>;
    };

    template <class T>
    Try<T>::operator Try<void>() const
    {
        if (hasError())
        {
            return Try<void>(getException());
        }
        return Try<void>();
    }

    template <typename F, typename... Args>
    auto makeTryCall(F && f, Args &&... args)
    {
        using T = std::invoke_result_t<F, Args...>;
        try
        {
            if constexpr (std::is_void_v<T>)
            {
                std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
                return Try<void>();
            }
            else
            {
                return Try<T>(std::invoke(std::forward<F>(f), std::forward<Args>(args)...));
            }
        }
        catch (...)
        {
            return Try<T>(std::current_exception());
        }
    }
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/util/Try.ipp>
#endif // TRY_HPP
