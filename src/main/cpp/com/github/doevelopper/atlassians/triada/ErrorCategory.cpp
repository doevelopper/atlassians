/**
 * @file ErrorCategory.cpp
 * @brief Implementation of custom error categories
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/triada/ErrorCategory.hpp>

using namespace com::github::doevelopper::atlassians::triada;

log4cxx::LoggerPtr ErrorCategoryBase::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.ErrorCategoryBase"));

// ============================================
// ErrorCategoryBase Implementation
// ============================================

auto ErrorCategoryBase::registerMessage(int code, std::string_view message) -> void
{
    std::lock_guard lock{m_mutex};
    m_messages[code] = std::string{message};
}

auto ErrorCategoryBase::messages() const -> const std::unordered_map<int, std::string>&
{
    return m_messages;
}

// ============================================
// GenericErrorCategory Implementation
// ============================================

GenericErrorCategory::GenericErrorCategory()
{
    registerMessage(static_cast<int>(GenericError::Success), "Success");
    registerMessage(static_cast<int>(GenericError::Unknown), "Unknown error");
    registerMessage(static_cast<int>(GenericError::InvalidArgument), "Invalid argument");
    registerMessage(static_cast<int>(GenericError::OutOfRange), "Value out of range");
    registerMessage(static_cast<int>(GenericError::InvalidState), "Invalid state");
    registerMessage(static_cast<int>(GenericError::NotInitialized), "Not initialized");
    registerMessage(static_cast<int>(GenericError::AlreadyInitialized), "Already initialized");
    registerMessage(static_cast<int>(GenericError::NotSupported), "Operation not supported");
    registerMessage(static_cast<int>(GenericError::NotImplemented), "Not implemented");
    registerMessage(static_cast<int>(GenericError::Timeout), "Operation timed out");
    registerMessage(static_cast<int>(GenericError::Cancelled), "Operation cancelled");
    registerMessage(static_cast<int>(GenericError::ResourceExhausted), "Resource exhausted");
    registerMessage(static_cast<int>(GenericError::ResourceBusy), "Resource busy");
    registerMessage(static_cast<int>(GenericError::PermissionDenied), "Permission denied");
    registerMessage(static_cast<int>(GenericError::NotFound), "Not found");
    registerMessage(static_cast<int>(GenericError::AlreadyExists), "Already exists");
    registerMessage(static_cast<int>(GenericError::DataCorruption), "Data corruption detected");
    registerMessage(static_cast<int>(GenericError::InternalError), "Internal error");
    registerMessage(static_cast<int>(GenericError::ConfigurationError), "Configuration error");
    registerMessage(static_cast<int>(GenericError::DependencyFailure), "Dependency failure");
}

auto GenericErrorCategory::instance() -> const GenericErrorCategory&
{
    static GenericErrorCategory instance;
    return instance;
}

auto GenericErrorCategory::name() const noexcept -> const char*
{
    return "triada.generic";
}

auto GenericErrorCategory::message(int code) const -> std::string
{
    std::lock_guard lock{m_mutex};
    auto it = m_messages.find(code);
    if (it != m_messages.end())
    {
        return it->second;
    }
    return "Unknown generic error (" + std::to_string(code) + ")";
}

auto GenericErrorCategory::default_error_condition(int code) const noexcept -> std::error_condition
{
    switch (static_cast<GenericError>(code))
    {
        case GenericError::InvalidArgument:
            return std::errc::invalid_argument;
        case GenericError::OutOfRange:
            return std::errc::result_out_of_range;
        case GenericError::PermissionDenied:
            return std::errc::permission_denied;
        case GenericError::ResourceBusy:
            return std::errc::device_or_resource_busy;
        case GenericError::NotSupported:
            return std::errc::not_supported;
        case GenericError::Timeout:
            return std::errc::timed_out;
        case GenericError::Cancelled:
            return std::errc::operation_canceled;
        default:
            return std::error_condition{code, *this};
    }
}

auto GenericErrorCategory::equivalent(int code, const std::error_condition& condition) const noexcept -> bool
{
    return default_error_condition(code) == condition;
}

// ============================================
// IOErrorCategory Implementation
// ============================================

IOErrorCategory::IOErrorCategory()
{
    registerMessage(static_cast<int>(IOError::Success), "Success");
    registerMessage(static_cast<int>(IOError::Unknown), "Unknown I/O error");
    registerMessage(static_cast<int>(IOError::ReadError), "Read error");
    registerMessage(static_cast<int>(IOError::WriteError), "Write error");
    registerMessage(static_cast<int>(IOError::OpenError), "Failed to open");
    registerMessage(static_cast<int>(IOError::CloseError), "Failed to close");
    registerMessage(static_cast<int>(IOError::SeekError), "Seek error");
    registerMessage(static_cast<int>(IOError::EndOfFile), "End of file");
    registerMessage(static_cast<int>(IOError::BufferOverflow), "Buffer overflow");
    registerMessage(static_cast<int>(IOError::BufferUnderflow), "Buffer underflow");
    registerMessage(static_cast<int>(IOError::DeviceNotReady), "Device not ready");
    registerMessage(static_cast<int>(IOError::DeviceError), "Device error");
    registerMessage(static_cast<int>(IOError::FormatError), "Format error");
    registerMessage(static_cast<int>(IOError::EncodingError), "Encoding error");
    registerMessage(static_cast<int>(IOError::Interrupted), "Operation interrupted");
}

auto IOErrorCategory::instance() -> const IOErrorCategory&
{
    static IOErrorCategory instance;
    return instance;
}

auto IOErrorCategory::name() const noexcept -> const char*
{
    return "triada.io";
}

auto IOErrorCategory::message(int code) const -> std::string
{
    std::lock_guard lock{m_mutex};
    auto it = m_messages.find(code);
    if (it != m_messages.end())
    {
        return it->second;
    }
    return "Unknown I/O error (" + std::to_string(code) + ")";
}

// ============================================
// NetworkErrorCategory Implementation
// ============================================

NetworkErrorCategory::NetworkErrorCategory()
{
    registerMessage(static_cast<int>(NetworkError::Success), "Success");
    registerMessage(static_cast<int>(NetworkError::Unknown), "Unknown network error");
    registerMessage(static_cast<int>(NetworkError::ConnectionRefused), "Connection refused");
    registerMessage(static_cast<int>(NetworkError::ConnectionReset), "Connection reset");
    registerMessage(static_cast<int>(NetworkError::ConnectionAborted), "Connection aborted");
    registerMessage(static_cast<int>(NetworkError::ConnectionTimeout), "Connection timed out");
    registerMessage(static_cast<int>(NetworkError::HostUnreachable), "Host unreachable");
    registerMessage(static_cast<int>(NetworkError::NetworkUnreachable), "Network unreachable");
    registerMessage(static_cast<int>(NetworkError::AddressInUse), "Address already in use");
    registerMessage(static_cast<int>(NetworkError::AddressNotAvailable), "Address not available");
    registerMessage(static_cast<int>(NetworkError::DNSResolutionFailed), "DNS resolution failed");
    registerMessage(static_cast<int>(NetworkError::SSLHandshakeFailed), "SSL/TLS handshake failed");
    registerMessage(static_cast<int>(NetworkError::ProtocolError), "Protocol error");
    registerMessage(static_cast<int>(NetworkError::MessageTooLarge), "Message too large");
    registerMessage(static_cast<int>(NetworkError::NetworkDown), "Network is down");
    registerMessage(static_cast<int>(NetworkError::NoRoute), "No route to host");
}

auto NetworkErrorCategory::instance() -> const NetworkErrorCategory&
{
    static NetworkErrorCategory instance;
    return instance;
}

auto NetworkErrorCategory::name() const noexcept -> const char*
{
    return "triada.network";
}

auto NetworkErrorCategory::message(int code) const -> std::string
{
    std::lock_guard lock{m_mutex};
    auto it = m_messages.find(code);
    if (it != m_messages.end())
    {
        return it->second;
    }
    return "Unknown network error (" + std::to_string(code) + ")";
}

// ============================================
// ProtocolErrorCategory Implementation
// ============================================

ProtocolErrorCategory::ProtocolErrorCategory()
{
    registerMessage(static_cast<int>(ProtocolError::Success), "Success");
    registerMessage(static_cast<int>(ProtocolError::Unknown), "Unknown protocol error");
    registerMessage(static_cast<int>(ProtocolError::InvalidMessage), "Invalid message");
    registerMessage(static_cast<int>(ProtocolError::InvalidHeader), "Invalid header");
    registerMessage(static_cast<int>(ProtocolError::InvalidPayload), "Invalid payload");
    registerMessage(static_cast<int>(ProtocolError::InvalidChecksum), "Invalid checksum");
    registerMessage(static_cast<int>(ProtocolError::InvalidVersion), "Invalid version");
    registerMessage(static_cast<int>(ProtocolError::UnsupportedVersion), "Unsupported version");
    registerMessage(static_cast<int>(ProtocolError::SequenceError), "Sequence error");
    registerMessage(static_cast<int>(ProtocolError::AuthenticationFailed), "Authentication failed");
    registerMessage(static_cast<int>(ProtocolError::AuthorizationFailed), "Authorization failed");
    registerMessage(static_cast<int>(ProtocolError::RequestTooLarge), "Request too large");
    registerMessage(static_cast<int>(ProtocolError::ResponseTooLarge), "Response too large");
    registerMessage(static_cast<int>(ProtocolError::RateLimited), "Rate limited");
    registerMessage(static_cast<int>(ProtocolError::ServiceUnavailable), "Service unavailable");
    registerMessage(static_cast<int>(ProtocolError::InternalServiceError), "Internal service error");
}

auto ProtocolErrorCategory::instance() -> const ProtocolErrorCategory&
{
    static ProtocolErrorCategory instance;
    return instance;
}

auto ProtocolErrorCategory::name() const noexcept -> const char*
{
    return "triada.protocol";
}

auto ProtocolErrorCategory::message(int code) const -> std::string
{
    std::lock_guard lock{m_mutex};
    auto it = m_messages.find(code);
    if (it != m_messages.end())
    {
        return it->second;
    }
    return "Unknown protocol error (" + std::to_string(code) + ")";
}

// ============================================
// MemoryErrorCategory Implementation
// ============================================

MemoryErrorCategory::MemoryErrorCategory()
{
    registerMessage(static_cast<int>(MemoryError::Success), "Success");
    registerMessage(static_cast<int>(MemoryError::Unknown), "Unknown memory error");
    registerMessage(static_cast<int>(MemoryError::AllocationFailed), "Memory allocation failed");
    registerMessage(static_cast<int>(MemoryError::OutOfMemory), "Out of memory");
    registerMessage(static_cast<int>(MemoryError::InvalidPointer), "Invalid pointer");
    registerMessage(static_cast<int>(MemoryError::DoubleFree), "Double free detected");
    registerMessage(static_cast<int>(MemoryError::UseAfterFree), "Use after free detected");
    registerMessage(static_cast<int>(MemoryError::BufferOverrun), "Buffer overrun");
    registerMessage(static_cast<int>(MemoryError::StackOverflow), "Stack overflow");
    registerMessage(static_cast<int>(MemoryError::HeapCorruption), "Heap corruption");
    registerMessage(static_cast<int>(MemoryError::MemoryLeak), "Memory leak detected");
    registerMessage(static_cast<int>(MemoryError::AlignmentError), "Memory alignment error");
}

auto MemoryErrorCategory::instance() -> const MemoryErrorCategory&
{
    static MemoryErrorCategory instance;
    return instance;
}

auto MemoryErrorCategory::name() const noexcept -> const char*
{
    return "triada.memory";
}

auto MemoryErrorCategory::message(int code) const -> std::string
{
    std::lock_guard lock{m_mutex};
    auto it = m_messages.find(code);
    if (it != m_messages.end())
    {
        return it->second;
    }
    return "Unknown memory error (" + std::to_string(code) + ")";
}

// ============================================
// Category Access Functions
// ============================================

auto com::github::doevelopper::atlassians::triada::generic_error_category() -> const std::error_category&
{
    return GenericErrorCategory::instance();
}

auto com::github::doevelopper::atlassians::triada::io_error_category() -> const std::error_category&
{
    return IOErrorCategory::instance();
}

auto com::github::doevelopper::atlassians::triada::network_error_category() -> const std::error_category&
{
    return NetworkErrorCategory::instance();
}

auto com::github::doevelopper::atlassians::triada::protocol_error_category() -> const std::error_category&
{
    return ProtocolErrorCategory::instance();
}

auto com::github::doevelopper::atlassians::triada::memory_error_category() -> const std::error_category&
{
    return MemoryErrorCategory::instance();
}
