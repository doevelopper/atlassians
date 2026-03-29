/**
 * @file ErrorContext.hpp
 * @brief Comprehensive error context information capture
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 *
 * Captures comprehensive context for each error:
 * - Function name
 * - File name and line number
 * - Timestamp
 * - Thread/task identifier
 * - Stack trace (when applicable)
 * - Relevant variable states
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORCONTEXT_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORCONTEXT_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <chrono>
#include <thread>
#include <optional>
#include <memory>
#include <map>
#include <variant>
#include <source_location>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::triada
{

/**
 * @brief Source location information (C++20 std::source_location wrapper)
 */
class SourceLocation
{
public:
    using LineType = std::uint_least32_t;
    using ColumnType = std::uint_least32_t;

    /**
     * @brief Default constructor capturing current location
     */
    constexpr SourceLocation(
        const std::source_location& loc = std::source_location::current()) noexcept
        : m_fileName{loc.file_name()}
        , m_functionName{loc.function_name()}
        , m_line{loc.line()}
        , m_column{loc.column()}
    {
    }

    /**
     * @brief Construct from explicit values
     */
    constexpr SourceLocation(
        std::string_view fileName,
        std::string_view functionName,
        LineType line,
        ColumnType column = 0) noexcept
        : m_fileName{fileName}
        , m_functionName{functionName}
        , m_line{line}
        , m_column{column}
    {
    }

    [[nodiscard]] constexpr auto fileName() const noexcept -> std::string_view { return m_fileName; }
    [[nodiscard]] constexpr auto functionName() const noexcept -> std::string_view { return m_functionName; }
    [[nodiscard]] constexpr auto line() const noexcept -> LineType { return m_line; }
    [[nodiscard]] constexpr auto column() const noexcept -> ColumnType { return m_column; }

    [[nodiscard]] auto toString() const -> std::string;

private:
    std::string_view m_fileName;
    std::string_view m_functionName;
    LineType m_line{0};
    ColumnType m_column{0};
};

/**
 * @brief Stack frame information
 */
struct StackFrame
{
    std::string functionName;
    std::string fileName;
    std::uint32_t line{0};
    std::uintptr_t address{0};
    std::string moduleName;

    [[nodiscard]] auto toString() const -> std::string;
};

/**
 * @brief Stack trace capture
 */
class StackTrace
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    using FrameContainer = std::vector<StackFrame>;

    /**
     * @brief Default constructor - empty stack trace
     */
    StackTrace() noexcept = default;

    /**
     * @brief Capture current stack trace
     * @param skipFrames Number of frames to skip from the top
     * @param maxFrames Maximum number of frames to capture
     */
    explicit StackTrace(std::size_t skipFrames, std::size_t maxFrames = 64);

    /**
     * @brief Factory method to capture current stack trace
     */
    [[nodiscard]] static auto capture(
        std::size_t skipFrames = 0,
        std::size_t maxFrames = 64) -> StackTrace;

    [[nodiscard]] auto frames() const noexcept -> const FrameContainer&;
    [[nodiscard]] auto empty() const noexcept -> bool;
    [[nodiscard]] auto size() const noexcept -> std::size_t;
    [[nodiscard]] auto toString() const -> std::string;

private:
    FrameContainer m_frames;
};

/**
 * @brief Context variable types
 */
using ContextValue = std::variant<
    std::monostate,
    bool,
    std::int64_t,
    std::uint64_t,
    double,
    std::string,
    std::vector<std::uint8_t>
>;

/**
 * @brief Comprehensive error context information
 *
 * Captures all relevant context when an error occurs:
 * - Where: File, function, line
 * - When: Timestamp
 * - Who: Thread ID, correlation ID
 * - What: Custom context variables
 * - How: Stack trace
 */
class ErrorContext
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;
    using ThreadId = std::thread::id;
    using ContextMap = std::map<std::string, ContextValue>;

    // ============================================
    // Rule of Five
    // ============================================

    /**
     * @brief Default constructor
     */
    ErrorContext() noexcept;

    /**
     * @brief Construct with source location
     */
    explicit ErrorContext(const SourceLocation& location);

    /**
     * @brief Construct with source location and message
     */
    ErrorContext(const SourceLocation& location, std::string_view message);

    /**
     * @brief Destructor
     */
    ~ErrorContext() noexcept;

    /**
     * @brief Copy constructor
     */
    ErrorContext(const ErrorContext& other);

    /**
     * @brief Copy assignment
     */
    auto operator=(const ErrorContext& other) -> ErrorContext&;

    /**
     * @brief Move constructor
     */
    ErrorContext(ErrorContext&& other) noexcept;

    /**
     * @brief Move assignment
     */
    auto operator=(ErrorContext&& other) noexcept -> ErrorContext&;

    // ============================================
    // Factory Methods
    // ============================================

    /**
     * @brief Create context capturing current location
     */
    [[nodiscard]] static auto here(
        const std::source_location& loc = std::source_location::current()) -> ErrorContext;

    /**
     * @brief Create context with message
     */
    [[nodiscard]] static auto withMessage(
        std::string_view message,
        const std::source_location& loc = std::source_location::current()) -> ErrorContext;

    // ============================================
    // Accessors
    // ============================================

    [[nodiscard]] auto location() const noexcept -> const SourceLocation&;
    [[nodiscard]] auto message() const noexcept -> std::string_view;
    [[nodiscard]] auto timestamp() const noexcept -> TimePoint;
    [[nodiscard]] auto threadId() const noexcept -> ThreadId;
    [[nodiscard]] auto correlationId() const noexcept -> std::string_view;
    [[nodiscard]] auto stackTrace() const noexcept -> const StackTrace&;
    [[nodiscard]] auto contextVariables() const noexcept -> const ContextMap&;

    // ============================================
    // Modifiers (Builder pattern)
    // ============================================

    /**
     * @brief Set the error message
     */
    auto setMessage(std::string_view msg) -> ErrorContext&;

    /**
     * @brief Set correlation ID for distributed tracing
     */
    auto setCorrelationId(std::string_view id) -> ErrorContext&;

    /**
     * @brief Capture and attach stack trace
     */
    auto captureStackTrace(std::size_t skipFrames = 2) -> ErrorContext&;

    /**
     * @brief Add a context variable
     */
    template<typename T>
    auto addVariable(std::string_view name, T&& value) -> ErrorContext&
    {
        if constexpr (std::is_same_v<std::decay_t<T>, bool>)
        {
            m_variables[std::string{name}] = value;
        }
        else if constexpr (std::is_integral_v<std::decay_t<T>>)
        {
            if constexpr (std::is_signed_v<std::decay_t<T>>)
            {
                m_variables[std::string{name}] = static_cast<std::int64_t>(value);
            }
            else
            {
                m_variables[std::string{name}] = static_cast<std::uint64_t>(value);
            }
        }
        else if constexpr (std::is_floating_point_v<std::decay_t<T>>)
        {
            m_variables[std::string{name}] = static_cast<double>(value);
        }
        else if constexpr (std::is_convertible_v<T, std::string>)
        {
            m_variables[std::string{name}] = std::string{std::forward<T>(value)};
        }
        else
        {
            m_variables[std::string{name}] = std::forward<T>(value);
        }
        return *this;
    }

    /**
     * @brief Get a context variable
     */
    [[nodiscard]] auto getVariable(std::string_view name) const -> std::optional<ContextValue>;

    // ============================================
    // Formatting
    // ============================================

    /**
     * @brief Format as human-readable string
     */
    [[nodiscard]] auto toString() const -> std::string;

    /**
     * @brief Format as JSON string
     */
    [[nodiscard]] auto toJson() const -> std::string;

    /**
     * @brief Format timestamp as ISO 8601 string
     */
    [[nodiscard]] auto timestampString() const -> std::string;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;

    SourceLocation m_location;
    std::string m_message;
    TimePoint m_timestamp;
    ThreadId m_threadId;
    std::string m_correlationId;
    StackTrace m_stackTrace;
    ContextMap m_variables;
};

} // namespace com::github::doevelopper::atlassians::triada

/**
 * @brief Macro to create ErrorContext at current location
 */
#define TRIADA_ERROR_CONTEXT() \
    ::com::github::doevelopper::atlassians::triada::ErrorContext::here()

/**
 * @brief Macro to create ErrorContext with message at current location
 */
#define TRIADA_ERROR_CONTEXT_MSG(msg) \
    ::com::github::doevelopper::atlassians::triada::ErrorContext::withMessage(msg)

/**
 * @brief Macro to add variable to context
 */
#define TRIADA_CTX_VAR(ctx, var) \
    (ctx).addVariable(#var, var)

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORCONTEXT_HPP
