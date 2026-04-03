/**
 * @file Error.cpp
 * @brief Implementation of comprehensive Error type
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/triada/Error.hpp>

#include <sstream>
#include <typeinfo>

using namespace com::github::doevelopper::atlassians::triada;

log4cxx::LoggerPtr Error::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.Error"));

// ============================================
// Error Implementation
// ============================================

Error::Error() noexcept = default;

Error::Error(ErrorCode code) noexcept
    : m_code{code}
{
}

Error::Error(ErrorCode code, std::string_view message)
    : m_code{code}
    , m_message{message}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

Error::Error(std::error_code ec)
    : m_stdError{ec}
    , m_message{ec.message()}
{
    // Map std::error_code severity based on error category
    if (ec)
    {
        m_code = ErrorCode::create(
            ErrorSeverity::Recoverable,
            ServiceId::Core,
            0,
            static_cast<ErrorCode::CodeType>(ec.value()));
    }
}

Error::Error(std::error_code ec, std::string_view message)
    : m_stdError{ec}
    , m_message{message}
{
    if (ec)
    {
        m_code = ErrorCode::create(
            ErrorSeverity::Recoverable,
            ServiceId::Core,
            0,
            static_cast<ErrorCode::CodeType>(ec.value()));
    }
}

Error::~Error() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

Error::Error(const Error& other)
    : m_code{other.m_code}
    , m_stdError{other.m_stdError}
    , m_message{other.m_message}
    , m_context{other.m_context}
    , m_chain{other.m_chain}
{
}

auto Error::operator=(const Error& other) -> Error&
{
    if (this != &other)
    {
        m_code = other.m_code;
        m_stdError = other.m_stdError;
        m_message = other.m_message;
        m_context = other.m_context;
        m_chain = other.m_chain;
    }
    return *this;
}

Error::Error(Error&& other) noexcept
    : m_code{other.m_code}
    , m_stdError{other.m_stdError}
    , m_message{std::move(other.m_message)}
    , m_context{std::move(other.m_context)}
    , m_chain{std::move(other.m_chain)}
{
}

auto Error::operator=(Error&& other) noexcept -> Error&
{
    if (this != &other)
    {
        m_code = other.m_code;
        m_stdError = other.m_stdError;
        m_message = std::move(other.m_message);
        m_context = std::move(other.m_context);
        m_chain = std::move(other.m_chain);
    }
    return *this;
}

auto Error::create(
    ErrorCode code,
    std::string_view message,
    std::error_code ec,
    const std::source_location& loc) -> Error
{
    Error err;
    err.m_code = code;
    err.m_message = message;
    err.m_stdError = ec;
    err.m_context = ErrorContext{SourceLocation{loc}};
    return err;
}

auto Error::fromStdError(
    std::error_code ec,
    std::string_view message,
    const std::source_location& loc) -> Error
{
    Error err{ec};
    if (!message.empty())
    {
        err.m_message = message;
    }
    err.m_context = ErrorContext{SourceLocation{loc}};
    return err;
}

auto Error::fromException(
    const std::exception& ex,
    const std::source_location& loc) -> Error
{
    Error err;
    err.m_code = ErrorCode::create(
        ErrorSeverity::Error,
        ServiceId::Core,
        0,
        1);
    err.m_message = ex.what();
    err.m_context = ErrorContext{SourceLocation{loc}};
    err.m_context.addVariable("exception_type", std::string{typeid(ex).name()});
    return err;
}

auto Error::success() noexcept -> Error
{
    return Error{};
}

auto Error::code() const noexcept -> ErrorCode
{
    return m_code;
}

auto Error::stdError() const noexcept -> std::error_code
{
    return m_stdError;
}

auto Error::message() const noexcept -> std::string_view
{
    return m_message;
}

auto Error::context() const noexcept -> const ErrorContext&
{
    return m_context;
}

auto Error::chain() const noexcept -> const ErrorChain&
{
    return m_chain;
}

auto Error::rootCause() const noexcept -> const Error&
{
    if (m_chain.empty())
    {
        return *this;
    }
    return m_chain.back().rootCause();
}

auto Error::isSuccess() const noexcept -> bool
{
    return m_code.isSuccess() && !m_stdError;
}

auto Error::isError() const noexcept -> bool
{
    return !isSuccess();
}

auto Error::isRecoverable() const noexcept -> bool
{
    return m_code.isRecoverable();
}

auto Error::isFatal() const noexcept -> bool
{
    return m_code.isFatal();
}

auto Error::severity() const noexcept -> ErrorSeverity
{
    return m_code.severity();
}

Error::operator bool() const noexcept
{
    return isError();
}

auto Error::withContext(ErrorContext ctx) -> Error&
{
    m_context = std::move(ctx);
    return *this;
}

auto Error::withMessage(std::string_view msg) -> Error&
{
    m_message = msg;
    return *this;
}

auto Error::withCause(Error cause) -> Error&
{
    m_chain.push_back(std::move(cause));
    return *this;
}

auto Error::withStackTrace(std::size_t skipFrames) -> Error&
{
    m_context.captureStackTrace(skipFrames + 1);
    return *this;
}

auto Error::withCorrelationId(std::string_view id) -> Error&
{
    m_context.setCorrelationId(id);
    return *this;
}

auto Error::operator==(const Error& other) const noexcept -> bool
{
    return m_code == other.m_code &&
           m_stdError == other.m_stdError &&
           m_message == other.m_message;
}

auto Error::operator!=(const Error& other) const noexcept -> bool
{
    return !(*this == other);
}

auto Error::toString() const -> std::string
{
    std::ostringstream oss;

    oss << "Error{";

    if (m_code.isSuccess())
    {
        oss << "success";
    }
    else
    {
        oss << "severity=" << m_code.severityString()
            << ", service=" << m_code.serviceString()
            << ", code=" << m_code.toHexString();
    }

    if (!m_message.empty())
    {
        oss << ", message=\"" << m_message << "\"";
    }

    if (m_stdError)
    {
        oss << ", std_error={" << m_stdError.category().name()
            << ":" << m_stdError.value() << " \"" << m_stdError.message() << "\"}";
    }

    auto loc = m_context.location();
    if (!loc.fileName().empty())
    {
        oss << ", location=" << loc.toString();
    }

    oss << "}";

    return oss.str();
}

auto Error::toJson() const -> std::string
{
    std::ostringstream oss;

    oss << "{\n"
        << "  \"code\": \"" << m_code.toHexString() << "\",\n"
        << "  \"severity\": \"" << m_code.severityString() << "\",\n"
        << "  \"service\": \"" << m_code.serviceString() << "\",\n"
        << "  \"message\": \"" << m_message << "\"";

    if (m_stdError)
    {
        oss << ",\n  \"stdError\": {\n"
            << "    \"category\": \"" << m_stdError.category().name() << "\",\n"
            << "    \"value\": " << m_stdError.value() << ",\n"
            << "    \"message\": \"" << m_stdError.message() << "\"\n"
            << "  }";
    }

    // Add context info
    oss << ",\n  \"context\": " << m_context.toJson();

    if (!m_chain.empty())
    {
        oss << ",\n  \"causes\": [";
        for (std::size_t i = 0; i < m_chain.size(); ++i)
        {
            if (i > 0) oss << ",";
            oss << "\n    " << m_chain[i].toJson();
        }
        oss << "\n  ]";
    }

    oss << "\n}";

    return oss.str();
}

auto Error::toFullString() const -> std::string
{
    std::ostringstream oss;

    oss << toString() << "\n";
    oss << "Context:\n" << m_context.toString() << "\n";

    if (!m_chain.empty())
    {
        oss << "\nCaused by:\n";
        for (std::size_t i = 0; i < m_chain.size(); ++i)
        {
            oss << "  [" << i << "] " << m_chain[i].toString() << "\n";
        }
    }

    return oss.str();
}

// ============================================
// ErrorException Implementation
// ============================================

ErrorException::ErrorException(Error error)
    : m_error{std::move(error)}
{
}

auto ErrorException::what() const noexcept -> const char*
{
    if (m_what.empty())
    {
        m_what = m_error.toString();
    }
    return m_what.c_str();
}

auto ErrorException::error() const noexcept -> const Error&
{
    return m_error;
}
