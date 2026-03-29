/**
 * @file ErrorContext.cpp
 * @brief Implementation of comprehensive error context
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/triada/ErrorContext.hpp>

#include <sstream>
#include <iomanip>
#include <ctime>

// Platform-specific stack trace capture
#if defined(__linux__) || defined(__APPLE__)
    #include <execinfo.h>
    #include <cxxabi.h>
    #include <cstdlib>
    #define HAS_BACKTRACE 1
#endif

using namespace com::github::doevelopper::atlassians::triada;

// Logger definitions
log4cxx::LoggerPtr StackTrace::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.StackTrace"));

log4cxx::LoggerPtr ErrorContext::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.ErrorContext"));

// ============================================
// SourceLocation Implementation
// ============================================

auto SourceLocation::toString() const -> std::string
{
    std::ostringstream oss;
    oss << m_fileName << ":" << m_line;
    if (m_column > 0)
    {
        oss << ":" << m_column;
    }
    oss << " in " << m_functionName;
    return oss.str();
}

// ============================================
// StackFrame Implementation
// ============================================

auto StackFrame::toString() const -> std::string
{
    std::ostringstream oss;
    oss << std::hex << "0x" << std::setfill('0') << std::setw(sizeof(void*) * 2)
        << address << std::dec << ": ";

    if (!moduleName.empty())
    {
        oss << moduleName << " ";
    }

    oss << functionName;

    if (!fileName.empty())
    {
        oss << " at " << fileName;
        if (line > 0)
        {
            oss << ":" << line;
        }
    }

    return oss.str();
}

// ============================================
// StackTrace Implementation
// ============================================

StackTrace::StackTrace(std::size_t skipFrames, std::size_t maxFrames)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

#if defined(HAS_BACKTRACE)
    std::vector<void*> addresses(maxFrames + skipFrames);
    int frameCount = backtrace(addresses.data(),
                               static_cast<int>(addresses.size()));

    if (frameCount <= 0)
    {
        return;
    }

    char** symbols = backtrace_symbols(addresses.data(), frameCount);
    if (symbols == nullptr)
    {
        return;
    }

    for (int i = static_cast<int>(skipFrames); i < frameCount; ++i)
    {
        StackFrame frame;
        frame.address = reinterpret_cast<std::uintptr_t>(addresses[i]);
        frame.functionName = symbols[i];

        // Attempt to demangle C++ symbols
        // Format: module(function+offset) [address]
        std::string symbol = symbols[i];

        auto openParen = symbol.find('(');
        auto plus = symbol.find('+', openParen);

        if (openParen != std::string::npos && plus != std::string::npos)
        {
            frame.moduleName = symbol.substr(0, openParen);
            std::string mangledName = symbol.substr(openParen + 1, plus - openParen - 1);

            int status = 0;
            char* demangled = abi::__cxa_demangle(mangledName.c_str(), nullptr, nullptr, &status);

            if (status == 0 && demangled != nullptr)
            {
                frame.functionName = demangled;
                std::free(demangled);
            }
            else if (!mangledName.empty())
            {
                frame.functionName = mangledName;
            }
        }

        m_frames.push_back(std::move(frame));
    }

    std::free(symbols);
#else
    // Stack trace not available on this platform
    (void)skipFrames;
    (void)maxFrames;
#endif
}

auto StackTrace::capture(std::size_t skipFrames, std::size_t maxFrames) -> StackTrace
{
    return StackTrace{skipFrames + 1, maxFrames};  // +1 to skip this function
}

auto StackTrace::frames() const noexcept -> const FrameContainer&
{
    return m_frames;
}

auto StackTrace::empty() const noexcept -> bool
{
    return m_frames.empty();
}

auto StackTrace::size() const noexcept -> std::size_t
{
    return m_frames.size();
}

auto StackTrace::toString() const -> std::string
{
    std::ostringstream oss;
    oss << "Stack trace (" << m_frames.size() << " frames):\n";

    for (std::size_t i = 0; i < m_frames.size(); ++i)
    {
        oss << "  #" << i << ": " << m_frames[i].toString() << "\n";
    }

    return oss.str();
}

// ============================================
// ErrorContext::Impl
// ============================================

struct ErrorContext::Impl
{
    // Additional implementation details if needed
};

// ============================================
// ErrorContext Implementation
// ============================================

ErrorContext::ErrorContext() noexcept
    : m_timestamp{Clock::now()}
    , m_threadId{std::this_thread::get_id()}
{
}

ErrorContext::ErrorContext(const SourceLocation& location)
    : m_location{location}
    , m_timestamp{Clock::now()}
    , m_threadId{std::this_thread::get_id()}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ErrorContext::ErrorContext(const SourceLocation& location, std::string_view message)
    : m_location{location}
    , m_message{message}
    , m_timestamp{Clock::now()}
    , m_threadId{std::this_thread::get_id()}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ErrorContext::~ErrorContext() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ErrorContext::ErrorContext(const ErrorContext& other)
    : m_location{other.m_location}
    , m_message{other.m_message}
    , m_timestamp{other.m_timestamp}
    , m_threadId{other.m_threadId}
    , m_correlationId{other.m_correlationId}
    , m_stackTrace{other.m_stackTrace}
    , m_variables{other.m_variables}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ErrorContext::operator=(const ErrorContext& other) -> ErrorContext&
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (this != &other)
    {
        m_location = other.m_location;
        m_message = other.m_message;
        m_timestamp = other.m_timestamp;
        m_threadId = other.m_threadId;
        m_correlationId = other.m_correlationId;
        m_stackTrace = other.m_stackTrace;
        m_variables = other.m_variables;
    }
    return *this;
}

ErrorContext::ErrorContext(ErrorContext&& other) noexcept
    : m_location{other.m_location}
    , m_message{std::move(other.m_message)}
    , m_timestamp{other.m_timestamp}
    , m_threadId{other.m_threadId}
    , m_correlationId{std::move(other.m_correlationId)}
    , m_stackTrace{std::move(other.m_stackTrace)}
    , m_variables{std::move(other.m_variables)}
{
}

auto ErrorContext::operator=(ErrorContext&& other) noexcept -> ErrorContext&
{
    if (this != &other)
    {
        m_location = other.m_location;
        m_message = std::move(other.m_message);
        m_timestamp = other.m_timestamp;
        m_threadId = other.m_threadId;
        m_correlationId = std::move(other.m_correlationId);
        m_stackTrace = std::move(other.m_stackTrace);
        m_variables = std::move(other.m_variables);
    }
    return *this;
}

auto ErrorContext::here(const std::source_location& loc) -> ErrorContext
{
    return ErrorContext{SourceLocation{loc}};
}

auto ErrorContext::withMessage(std::string_view message, const std::source_location& loc) -> ErrorContext
{
    return ErrorContext{SourceLocation{loc}, message};
}

auto ErrorContext::location() const noexcept -> const SourceLocation&
{
    return m_location;
}

auto ErrorContext::message() const noexcept -> std::string_view
{
    return m_message;
}

auto ErrorContext::timestamp() const noexcept -> TimePoint
{
    return m_timestamp;
}

auto ErrorContext::threadId() const noexcept -> ThreadId
{
    return m_threadId;
}

auto ErrorContext::correlationId() const noexcept -> std::string_view
{
    return m_correlationId;
}

auto ErrorContext::stackTrace() const noexcept -> const StackTrace&
{
    return m_stackTrace;
}

auto ErrorContext::contextVariables() const noexcept -> const ContextMap&
{
    return m_variables;
}

auto ErrorContext::setMessage(std::string_view msg) -> ErrorContext&
{
    m_message = msg;
    return *this;
}

auto ErrorContext::setCorrelationId(std::string_view id) -> ErrorContext&
{
    m_correlationId = id;
    return *this;
}

auto ErrorContext::captureStackTrace(std::size_t skipFrames) -> ErrorContext&
{
    m_stackTrace = StackTrace::capture(skipFrames + 1);
    return *this;
}

auto ErrorContext::getVariable(std::string_view name) const -> std::optional<ContextValue>
{
    auto it = m_variables.find(std::string{name});
    if (it != m_variables.end())
    {
        return it->second;
    }
    return std::nullopt;
}

auto ErrorContext::timestampString() const -> std::string
{
    auto timeT = Clock::to_time_t(m_timestamp);
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        m_timestamp.time_since_epoch()) % 1000000;

    std::tm tm{};
    gmtime_r(&timeT, &tm);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
        << '.' << std::setfill('0') << std::setw(6) << microseconds.count()
        << 'Z';

    return oss.str();
}

auto ErrorContext::toString() const -> std::string
{
    std::ostringstream oss;

    oss << "ErrorContext {\n"
        << "  location: " << m_location.toString() << "\n"
        << "  timestamp: " << timestampString() << "\n"
        << "  thread: " << m_threadId << "\n";

    if (!m_message.empty())
    {
        oss << "  message: " << m_message << "\n";
    }

    if (!m_correlationId.empty())
    {
        oss << "  correlationId: " << m_correlationId << "\n";
    }

    if (!m_variables.empty())
    {
        oss << "  variables: {\n";
        for (const auto& [key, value] : m_variables)
        {
            oss << "    " << key << ": ";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::monostate>)
                {
                    oss << "null";
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    oss << (arg ? "true" : "false");
                }
                else if constexpr (std::is_same_v<T, std::vector<std::uint8_t>>)
                {
                    oss << "[" << arg.size() << " bytes]";
                }
                else
                {
                    oss << arg;
                }
            }, value);
            oss << "\n";
        }
        oss << "  }\n";
    }

    if (!m_stackTrace.empty())
    {
        oss << "  stackTrace:\n";
        for (std::size_t i = 0; i < m_stackTrace.size(); ++i)
        {
            oss << "    #" << i << ": " << m_stackTrace.frames()[i].toString() << "\n";
        }
    }

    oss << "}";
    return oss.str();
}

auto ErrorContext::toJson() const -> std::string
{
    std::ostringstream oss;

    oss << "{\n"
        << "  \"location\": {\n"
        << "    \"file\": \"" << m_location.fileName() << "\",\n"
        << "    \"function\": \"" << m_location.functionName() << "\",\n"
        << "    \"line\": " << m_location.line() << "\n"
        << "  },\n"
        << "  \"timestamp\": \"" << timestampString() << "\",\n"
        << "  \"threadId\": \"" << m_threadId << "\"";

    if (!m_message.empty())
    {
        oss << ",\n  \"message\": \"" << m_message << "\"";
    }

    if (!m_correlationId.empty())
    {
        oss << ",\n  \"correlationId\": \"" << m_correlationId << "\"";
    }

    if (!m_variables.empty())
    {
        oss << ",\n  \"variables\": {";
        bool first = true;
        for (const auto& [key, value] : m_variables)
        {
            if (!first) oss << ",";
            first = false;

            oss << "\n    \"" << key << "\": ";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::monostate>)
                {
                    oss << "null";
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    oss << (arg ? "true" : "false");
                }
                else if constexpr (std::is_same_v<T, std::string>)
                {
                    oss << "\"" << arg << "\"";
                }
                else if constexpr (std::is_same_v<T, std::vector<std::uint8_t>>)
                {
                    oss << "\"[" << arg.size() << " bytes]\"";
                }
                else
                {
                    oss << arg;
                }
            }, value);
        }
        oss << "\n  }";
    }

    oss << "\n}";
    return oss.str();
}
