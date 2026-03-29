/**
 * @file ErrorCategory.hpp
 * @brief Custom error categories extending std::error_category
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 *
 * Provides custom error categories for:
 * - Generic application errors
 * - System/platform errors
 * - I/O errors
 * - Network errors
 * - Protocol errors
 * - Application-specific errors
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORCATEGORY_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORCATEGORY_HPP

#include <system_error>
#include <string>
#include <string_view>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>
#include <com/github/doevelopper/atlassians/triada/ErrorCode.hpp>

namespace com::github::doevelopper::atlassians::triada
{

/**
 * @brief Generic application error codes
 */
enum class GenericError : int
{
    Success = 0,
    Unknown = 1,
    InvalidArgument = 2,
    OutOfRange = 3,
    InvalidState = 4,
    NotInitialized = 5,
    AlreadyInitialized = 6,
    NotSupported = 7,
    NotImplemented = 8,
    Timeout = 9,
    Cancelled = 10,
    ResourceExhausted = 11,
    ResourceBusy = 12,
    PermissionDenied = 13,
    NotFound = 14,
    AlreadyExists = 15,
    DataCorruption = 16,
    InternalError = 17,
    ConfigurationError = 18,
    DependencyFailure = 19
};

/**
 * @brief I/O error codes
 */
enum class IOError : int
{
    Success = 0,
    Unknown = 1,
    ReadError = 2,
    WriteError = 3,
    OpenError = 4,
    CloseError = 5,
    SeekError = 6,
    EndOfFile = 7,
    BufferOverflow = 8,
    BufferUnderflow = 9,
    DeviceNotReady = 10,
    DeviceError = 11,
    FormatError = 12,
    EncodingError = 13,
    Interrupted = 14
};

/**
 * @brief Network error codes
 */
enum class NetworkError : int
{
    Success = 0,
    Unknown = 1,
    ConnectionRefused = 2,
    ConnectionReset = 3,
    ConnectionAborted = 4,
    ConnectionTimeout = 5,
    HostUnreachable = 6,
    NetworkUnreachable = 7,
    AddressInUse = 8,
    AddressNotAvailable = 9,
    DNSResolutionFailed = 10,
    SSLHandshakeFailed = 11,
    ProtocolError = 12,
    MessageTooLarge = 13,
    NetworkDown = 14,
    NoRoute = 15
};

/**
 * @brief Protocol error codes
 */
enum class ProtocolError : int
{
    Success = 0,
    Unknown = 1,
    InvalidMessage = 2,
    InvalidHeader = 3,
    InvalidPayload = 4,
    InvalidChecksum = 5,
    InvalidVersion = 6,
    UnsupportedVersion = 7,
    SequenceError = 8,
    AuthenticationFailed = 9,
    AuthorizationFailed = 10,
    RequestTooLarge = 11,
    ResponseTooLarge = 12,
    RateLimited = 13,
    ServiceUnavailable = 14,
    InternalServiceError = 15
};

/**
 * @brief Memory error codes
 */
enum class MemoryError : int
{
    Success = 0,
    Unknown = 1,
    AllocationFailed = 2,
    OutOfMemory = 3,
    InvalidPointer = 4,
    DoubleFree = 5,
    UseAfterFree = 6,
    BufferOverrun = 7,
    StackOverflow = 8,
    HeapCorruption = 9,
    MemoryLeak = 10,
    AlignmentError = 11
};

/**
 * @brief Base class for custom error categories
 */
class ErrorCategoryBase : public std::error_category
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    /**
     * @brief Register an error message
     * @param code Error code
     * @param message Human-readable message
     */
    auto registerMessage(int code, std::string_view message) -> void;

    /**
     * @brief Get all registered messages
     */
    [[nodiscard]] auto messages() const -> const std::unordered_map<int, std::string>&;

protected:
    ErrorCategoryBase() = default;
    ~ErrorCategoryBase() override = default;

    mutable std::mutex m_mutex;
    std::unordered_map<int, std::string> m_messages;
};

/**
 * @brief Generic application error category
 */
class GenericErrorCategory final : public ErrorCategoryBase
{
public:
    static auto instance() -> const GenericErrorCategory&;

    [[nodiscard]] auto name() const noexcept -> const char* override;
    [[nodiscard]] auto message(int code) const -> std::string override;
    [[nodiscard]] auto default_error_condition(int code) const noexcept
        -> std::error_condition override;
    [[nodiscard]] auto equivalent(int code, const std::error_condition& condition) const noexcept
        -> bool override;

private:
    GenericErrorCategory();
};

/**
 * @brief I/O error category
 */
class IOErrorCategory final : public ErrorCategoryBase
{
public:
    static auto instance() -> const IOErrorCategory&;

    [[nodiscard]] auto name() const noexcept -> const char* override;
    [[nodiscard]] auto message(int code) const -> std::string override;

private:
    IOErrorCategory();
};

/**
 * @brief Network error category
 */
class NetworkErrorCategory final : public ErrorCategoryBase
{
public:
    static auto instance() -> const NetworkErrorCategory&;

    [[nodiscard]] auto name() const noexcept -> const char* override;
    [[nodiscard]] auto message(int code) const -> std::string override;

private:
    NetworkErrorCategory();
};

/**
 * @brief Protocol error category
 */
class ProtocolErrorCategory final : public ErrorCategoryBase
{
public:
    static auto instance() -> const ProtocolErrorCategory&;

    [[nodiscard]] auto name() const noexcept -> const char* override;
    [[nodiscard]] auto message(int code) const -> std::string override;

private:
    ProtocolErrorCategory();
};

/**
 * @brief Memory error category
 */
class MemoryErrorCategory final : public ErrorCategoryBase
{
public:
    static auto instance() -> const MemoryErrorCategory&;

    [[nodiscard]] auto name() const noexcept -> const char* override;
    [[nodiscard]] auto message(int code) const -> std::string override;

private:
    MemoryErrorCategory();
};

// ============================================
// Category Access Functions
// ============================================

/**
 * @brief Get the generic error category
 */
[[nodiscard]] auto generic_error_category() -> const std::error_category&;

/**
 * @brief Get the I/O error category
 */
[[nodiscard]] auto io_error_category() -> const std::error_category&;

/**
 * @brief Get the network error category
 */
[[nodiscard]] auto network_error_category() -> const std::error_category&;

/**
 * @brief Get the protocol error category
 */
[[nodiscard]] auto protocol_error_category() -> const std::error_category&;

/**
 * @brief Get the memory error category
 */
[[nodiscard]] auto memory_error_category() -> const std::error_category&;

// ============================================
// make_error_code Overloads
// ============================================

[[nodiscard]] inline auto make_error_code(GenericError e) -> std::error_code
{
    return {static_cast<int>(e), generic_error_category()};
}

[[nodiscard]] inline auto make_error_code(IOError e) -> std::error_code
{
    return {static_cast<int>(e), io_error_category()};
}

[[nodiscard]] inline auto make_error_code(NetworkError e) -> std::error_code
{
    return {static_cast<int>(e), network_error_category()};
}

[[nodiscard]] inline auto make_error_code(ProtocolError e) -> std::error_code
{
    return {static_cast<int>(e), protocol_error_category()};
}

[[nodiscard]] inline auto make_error_code(MemoryError e) -> std::error_code
{
    return {static_cast<int>(e), memory_error_category()};
}

} // namespace com::github::doevelopper::atlassians::triada

// Enable automatic conversion from enums to std::error_code
template<>
struct std::is_error_code_enum<com::github::doevelopper::atlassians::triada::GenericError>
    : std::true_type {};

template<>
struct std::is_error_code_enum<com::github::doevelopper::atlassians::triada::IOError>
    : std::true_type {};

template<>
struct std::is_error_code_enum<com::github::doevelopper::atlassians::triada::NetworkError>
    : std::true_type {};

template<>
struct std::is_error_code_enum<com::github::doevelopper::atlassians::triada::ProtocolError>
    : std::true_type {};

template<>
struct std::is_error_code_enum<com::github::doevelopper::atlassians::triada::MemoryError>
    : std::true_type {};

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORCATEGORY_HPP
