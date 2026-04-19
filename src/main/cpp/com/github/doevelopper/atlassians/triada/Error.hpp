/**
 * @file Error.hpp
 * @brief Comprehensive error type combining code, context, and std::error_code
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERROR_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERROR_HPP

#include <memory>
#include <string>
#include <string_view>
#include <optional>
#include <system_error>
#include <exception>
#include <functional>
#include <vector>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>
#include <com/github/doevelopper/atlassians/triada/ErrorCode.hpp>
#include <com/github/doevelopper/atlassians/triada/ErrorContext.hpp>
#include <com/github/doevelopper/atlassians/triada/ErrorCategory.hpp>

namespace com::github::doevelopper::atlassians::triada
{

/**
 * @brief Comprehensive error type
 *
 * Combines:
 * - 64-bit structured ErrorCode
 * - Standard std::error_code for compatibility
 * - Rich ErrorContext with location and stack trace
 * - Human-readable message
 * - Nested/chained errors for error propagation
 *
 * @example
 * @code
 * auto result = doSomething();
 * if (!result) {
 *     Error err = Error::create(
 *         ErrorCode::create(ErrorSeverity::Recoverable, ServiceId::Network, 1, 100),
 *         "Connection failed",
 *         NetworkError::ConnectionTimeout
 *     ).withContext(TRIADA_ERROR_CONTEXT());
 *
 *     LOG4CXX_ERROR(logger, err.toString());
 * }
 * @endcode
 */
class Error
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    using ErrorChain = std::vector<Error>;

    // ============================================
    // Rule of Five
    // ============================================

    /**
     * @brief Default constructor - creates no error
     */
    Error() noexcept;

    /**
     * @brief Construct from ErrorCode
     */
    explicit Error(ErrorCode code) noexcept;

    /**
     * @brief Construct from ErrorCode and message
     */
    Error(ErrorCode code, std::string_view message);

    /**
     * @brief Construct from std::error_code
     */
    explicit Error(std::error_code ec);

    /**
     * @brief Construct from std::error_code and message
     */
    Error(std::error_code ec, std::string_view message);

    /**
     * @brief Destructor
     */
    ~Error() noexcept;

    /**
     * @brief Copy constructor
     */
    Error(const Error& other);

    /**
     * @brief Copy assignment
     */
    auto operator=(const Error& other) -> Error&;

    /**
     * @brief Move constructor
     */
    Error(Error&& other) noexcept;

    /**
     * @brief Move assignment
     */
    auto operator=(Error&& other) noexcept -> Error&;

    // ============================================
    // Factory Methods
    // ============================================

    /**
     * @brief Create error with all components
     */
    [[nodiscard]] static auto create(
        ErrorCode code,
        std::string_view message,
        std::error_code ec = {},
        const std::source_location& loc = std::source_location::current()) -> Error;

    /**
     * @brief Create from std::error_code
     */
    [[nodiscard]] static auto fromStdError(
        std::error_code ec,
        std::string_view message = {},
        const std::source_location& loc = std::source_location::current()) -> Error;

    /**
     * @brief Create from exception
     */
    [[nodiscard]] static auto fromException(
        const std::exception& ex,
        const std::source_location& loc = std::source_location::current()) -> Error;

    /**
     * @brief Create success (no error)
     */
    [[nodiscard]] static auto success() noexcept -> Error;

    // ============================================
    // Accessors
    // ============================================

    /**
     * @brief Get the 64-bit error code
     */
    [[nodiscard]] auto code() const noexcept -> ErrorCode;

    /**
     * @brief Get the std::error_code
     */
    [[nodiscard]] auto stdError() const noexcept -> std::error_code;

    /**
     * @brief Get the error message
     */
    [[nodiscard]] auto message() const noexcept -> std::string_view;

    /**
     * @brief Get the error context
     */
    [[nodiscard]] auto context() const noexcept -> const ErrorContext&;

    /**
     * @brief Get chained/nested errors
     */
    [[nodiscard]] auto chain() const noexcept -> const ErrorChain&;

    /**
     * @brief Get the root cause error
     */
    [[nodiscard]] auto rootCause() const noexcept -> const Error&;

    // ============================================
    // Status Queries
    // ============================================

    /**
     * @brief Check if this represents success (no error)
     */
    [[nodiscard]] auto isSuccess() const noexcept -> bool;

    /**
     * @brief Check if this represents an error
     */
    [[nodiscard]] auto isError() const noexcept -> bool;

    /**
     * @brief Check if this is recoverable
     */
    [[nodiscard]] auto isRecoverable() const noexcept -> bool;

    /**
     * @brief Check if this is fatal
     */
    [[nodiscard]] auto isFatal() const noexcept -> bool;

    /**
     * @brief Get the severity
     */
    [[nodiscard]] auto severity() const noexcept -> ErrorSeverity;

    /**
     * @brief Explicit bool conversion (true if error)
     */
    [[nodiscard]] explicit operator bool() const noexcept;

    // ============================================
    // Builder Pattern Modifiers
    // ============================================

    /**
     * @brief Add context information
     */
    auto withContext(ErrorContext ctx) -> Error&;

    /**
     * @brief Set/override message
     */
    auto withMessage(std::string_view msg) -> Error&;

    /**
     * @brief Wrap another error as cause
     */
    auto withCause(Error cause) -> Error&;

    /**
     * @brief Capture stack trace
     */
    auto withStackTrace(std::size_t skipFrames = 2) -> Error&;

    /**
     * @brief Add correlation ID
     */
    auto withCorrelationId(std::string_view id) -> Error&;

    /**
     * @brief Add context variable
     */
    template<typename T>
    auto withVariable(std::string_view name, T&& value) -> Error&
    {
        m_context.addVariable(name, std::forward<T>(value));
        return *this;
    }

    // ============================================
    // Comparison
    // ============================================

    [[nodiscard]] auto operator==(const Error& other) const noexcept -> bool;
    [[nodiscard]] auto operator!=(const Error& other) const noexcept -> bool;

    // ============================================
    // Formatting
    // ============================================

    /**
     * @brief Format as human-readable string
     */
    [[nodiscard]] auto toString() const -> std::string;

    /**
     * @brief Format as JSON
     */
    [[nodiscard]] auto toJson() const -> std::string;

    /**
     * @brief Format full error chain
     */
    [[nodiscard]] auto toFullString() const -> std::string;

private:
    ErrorCode m_code;
    std::error_code m_stdError;
    std::string m_message;
    ErrorContext m_context;
    ErrorChain m_chain;
};

/**
 * @brief Exception wrapper for Error
 */
class ErrorException : public std::exception
{
public:
    explicit ErrorException(Error error);

    [[nodiscard]] auto what() const noexcept -> const char* override;
    [[nodiscard]] auto error() const noexcept -> const Error&;

private:
    Error m_error;
    mutable std::string m_what;
};

/**
 * @brief Result type combining value or error
 *
 * @tparam T Value type on success
 *
 * @example
 * @code
 * auto divide(double a, double b) -> Result<double> {
 *     if (b == 0) {
 *         return Error::create(
 *             ErrorCode::create(ErrorSeverity::Recoverable, ServiceId::Core, 0, 1),
 *             "Division by zero");
 *     }
 *     return a / b;
 * }
 *
 * auto result = divide(10, 2);
 * if (result) {
 *     std::cout << "Result: " << result.value() << std::endl;
 * } else {
 *     std::cout << "Error: " << result.error().message() << std::endl;
 * }
 * @endcode
 */
template<typename T>
class Result
{
public:
    // Type traits
    using ValueType = T;
    using ErrorType = Error;

    /**
     * @brief Construct with value (success)
     */
    Result(T value) noexcept(std::is_nothrow_move_constructible_v<T>)
        : m_value{std::move(value)}
        , m_hasValue{true}
    {
    }

    /**
     * @brief Construct with error
     */
    Result(Error error) noexcept
        : m_error{std::move(error)}
        , m_hasValue{false}
    {
    }

    /**
     * @brief Check if has value (success)
     */
    [[nodiscard]] auto hasValue() const noexcept -> bool
    {
        return m_hasValue;
    }

    /**
     * @brief Check if has error
     */
    [[nodiscard]] auto hasError() const noexcept -> bool
    {
        return !m_hasValue;
    }

    /**
     * @brief Explicit bool conversion (true if has value)
     */
    [[nodiscard]] explicit operator bool() const noexcept
    {
        return m_hasValue;
    }

    /**
     * @brief Get value (throws if error)
     */
    [[nodiscard]] auto value() & -> T&
    {
        if (!m_hasValue)
        {
            throw ErrorException{m_error};
        }
        return m_value;
    }

    /**
     * @brief Get value (const)
     */
    [[nodiscard]] auto value() const& -> const T&
    {
        if (!m_hasValue)
        {
            throw ErrorException{m_error};
        }
        return m_value;
    }

    /**
     * @brief Get value (rvalue)
     */
    [[nodiscard]] auto value() && -> T&&
    {
        if (!m_hasValue)
        {
            throw ErrorException{m_error};
        }
        return std::move(m_value);
    }

    /**
     * @brief Get value or default
     */
    [[nodiscard]] auto valueOr(T defaultValue) const& -> T
    {
        return m_hasValue ? m_value : std::move(defaultValue);
    }

    /**
     * @brief Get error
     */
    [[nodiscard]] auto error() const& -> const Error&
    {
        return m_error;
    }

    /**
     * @brief Get error (rvalue)
     */
    [[nodiscard]] auto error() && -> Error&&
    {
        return std::move(m_error);
    }

    /**
     * @brief Map value to another type
     */
    template<typename F>
    [[nodiscard]] auto map(F&& func) const -> Result<std::invoke_result_t<F, const T&>>
    {
        using ResultType = Result<std::invoke_result_t<F, const T&>>;
        if (m_hasValue)
        {
            return ResultType{std::invoke(std::forward<F>(func), m_value)};
        }
        return ResultType{m_error};
    }

    /**
     * @brief Chain operations that may fail
     */
    template<typename F>
    [[nodiscard]] auto andThen(F&& func) const -> std::invoke_result_t<F, const T&>
    {
        using ResultType = std::invoke_result_t<F, const T&>;
        if (m_hasValue)
        {
            return std::invoke(std::forward<F>(func), m_value);
        }
        return ResultType{m_error};
    }

    /**
     * @brief Handle error with recovery function
     */
    template<typename F>
    [[nodiscard]] auto orElse(F&& func) const -> Result<T>
    {
        if (m_hasValue)
        {
            return *this;
        }
        return std::invoke(std::forward<F>(func), m_error);
    }

private:
    T m_value{};
    Error m_error;
    bool m_hasValue{false};
};

/**
 * @brief Specialization for void results
 */
template<>
class Result<void>
{
public:
    using ValueType = void;
    using ErrorType = Error;

    /**
     * @brief Construct as success
     */
    Result() noexcept
        : m_hasValue{true}
    {
    }

    /**
     * @brief Construct with error
     */
    Result(Error error) noexcept
        : m_error{std::move(error)}
        , m_hasValue{false}
    {
    }

    [[nodiscard]] auto hasValue() const noexcept -> bool
    {
        return m_hasValue;
    }

    [[nodiscard]] auto hasError() const noexcept -> bool
    {
        return !m_hasValue;
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return m_hasValue;
    }

    auto value() const -> void
    {
        if (!m_hasValue)
        {
            throw ErrorException{m_error};
        }
    }

    [[nodiscard]] auto error() const& -> const Error&
    {
        return m_error;
    }

    [[nodiscard]] auto error() && -> Error&&
    {
        return std::move(m_error);
    }

private:
    Error m_error;
    bool m_hasValue{false};
};

// Type alias for void result
using Status = Result<void>;

} // namespace com::github::doevelopper::atlassians::triada

// ============================================
// Error Creation Macros
// ============================================

/**
 * @brief Create an error with current source location
 */
#define TRIADA_ERROR(code, msg) \
    ::com::github::doevelopper::atlassians::triada::Error::create( \
        (code), (msg), {}, std::source_location::current())

/**
 * @brief Create error from std::error_code
 */
#define TRIADA_STD_ERROR(ec, msg) \
    ::com::github::doevelopper::atlassians::triada::Error::fromStdError( \
        (ec), (msg), std::source_location::current())

/**
 * @brief Return early if result is error
 */
#define TRIADA_TRY(result) \
    do { \
        auto&& _triada_result = (result); \
        if (!_triada_result) { \
            return std::move(_triada_result).error(); \
        } \
    } while(false)

/**
 * @brief Assign value or return error
 */
#define TRIADA_ASSIGN_OR_RETURN(var, result) \
    auto&& _triada_tmp_##var = (result); \
    if (!_triada_tmp_##var) { \
        return std::move(_triada_tmp_##var).error(); \
    } \
    var = std::move(_triada_tmp_##var).value()

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERROR_HPP
