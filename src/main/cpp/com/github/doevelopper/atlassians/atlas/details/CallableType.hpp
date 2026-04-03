/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     21/06/24 W 20:43
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_DETAILS_CALLABLETYPE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_DETAILS_CALLABLETYPE_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::details
{

    /*!
     * @brief Deduces the argument types and result of any callable object.
     *
     * @details This does not work with overloaded functions or overloaded implementations
     *   of operator().
     */
    template<typename Callable>
    struct CallableType final : public CallableType<decltype(&Callable::operator())>
    {
    public:
        CallableType () = delete;
    };

    template<typename Callable, typename CallableResult, typename... CallableArgs>
    struct CallableType<CallableResult (Callable::*)(CallableArgs...) const> : CallableType<CallableResult(CallableArgs...)>
    {
    public:
        CallableType () = delete;
    };

    template<typename CallableResult, typename... CallableArgs>
    struct CallableType<CallableResult (*)(CallableArgs...)> final : CallableType<CallableResult(CallableArgs...)>
    {
    public:
        CallableType () = delete;
    };

    // #if __has_feature(blocks)
    //
    // template<typename CallableResult, typename... CallableArgs>
    // struct CallableType<CallableResult (^)(CallableArgs...)> final : CallableType<CallableResult(CallableArgs...)>
    // {
    // public:
    //     CallableType () = delete;
    // };
    // #endif // __has_feature(blocks)
    /// This is the "main" definition of CallableType that the others ultimately
    /// inherit from.
    template<typename CallableResult, typename... CallableArgs>
    class CallableType<CallableResult(CallableArgs...)>
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:
        using result_type = CallableResult;
        using argument_types = std::tuple<CallableArgs...>;
        using function_type = std::function<result_type(CallableArgs...)>;

        // #if __has_feature(blocks)
        // using block_type = result_type (^)(CallableArgs...);
        // #endif

        CallableType() noexcept;
        CallableType(const CallableType&) = default;
        CallableType(CallableType&&) = default;
        CallableType& operator=(const CallableType&) = default;
        CallableType& operator=(CallableType&&) = default;
        virtual ~CallableType() noexcept;
    protected:
    private:
    };

    template <typename CallableResult, typename ... CallableArgs>
    log4cxx::LoggerPtr CallableType<CallableResult(CallableArgs...)>::logger =
        log4cxx::Logger::getLogger(std::string("com.github.doevelopper.atlas.details.CallableType"));

    template <typename CallableResult, typename ... CallableArgs>
    CallableType<CallableResult(CallableArgs...)>::CallableType() noexcept
    {
        LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    }

    template <typename CallableResult, typename ... CallableArgs>
    CallableType<CallableResult(CallableArgs...)>::~CallableType() noexcept
    {
        LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    }
}
#endif
