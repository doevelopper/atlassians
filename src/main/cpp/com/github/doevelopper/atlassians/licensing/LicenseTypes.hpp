/**
 * @file   LicenseTypes.hpp
 * @brief  License type definitions, enums, and exception hierarchy
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSETYPES_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSETYPES_HPP

#include <chrono>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace com::github::doevelopper::atlassians::licensing
{

// ============================================
// License Type Enumeration
// ============================================

/**
 * @brief Strongly typed enumeration of license types
 */
enum class LicenseType : std::uint8_t
{
    Trial,       ///< Time-limited, feature-limited trial license
    Commercial,  ///< Perpetual or subscription commercial license
    Enterprise,  ///< Full features with concurrent user support
    Educational, ///< Restricted use case for educational purposes
    doevelopper,   ///< Debugging/development only license
    Unknown      ///< Unknown or invalid license type
};

/**
 * @brief Convert LicenseType to string representation
 */
[[nodiscard]] constexpr auto licenseTypeToString(LicenseType type) noexcept -> std::string_view
{
    switch (type)
    {
        case LicenseType::Trial:
            return "Trial";
        case LicenseType::Commercial:
            return "Commercial";
        case LicenseType::Enterprise:
            return "Enterprise";
        case LicenseType::Educational:
            return "Educational";
        case LicenseType::doevelopper:
            return "doevelopper";
        case LicenseType::Unknown:
            [[fallthrough]];
        default:
            return "Unknown";
    }
}

/**
 * @brief Parse string to LicenseType
 */
[[nodiscard]] inline auto stringToLicenseType(std::string_view str) noexcept -> LicenseType
{
    if (str == "Trial")
        return LicenseType::Trial;
    if (str == "Commercial")
        return LicenseType::Commercial;
    if (str == "Enterprise")
        return LicenseType::Enterprise;
    if (str == "Educational")
        return LicenseType::Educational;
    if (str == "doevelopper")
        return LicenseType::doevelopper;
    return LicenseType::Unknown;
}

// ============================================
// License Status Enumeration
// ============================================

/**
 * @brief Strongly typed enumeration of license status
 */
enum class LicenseStatus : std::uint8_t
{
    Valid,            ///< License is valid and active
    Expired,          ///< License has expired
    Invalid,          ///< License is invalid (bad signature, format, etc.)
    Revoked,          ///< License has been revoked
    Suspended,        ///< License is temporarily suspended
    GracePeriod,      ///< License is in grace period
    NotActivated,     ///< License has not been activated
    HardwareMismatch, ///< Hardware ID does not match
    ConcurrencyLimit, ///< Concurrent usage limit exceeded
    Unknown           ///< Unknown status
};

/**
 * @brief Convert LicenseStatus to string representation
 */
[[nodiscard]] constexpr auto licenseStatusToString(LicenseStatus status) noexcept -> std::string_view
{
    switch (status)
    {
        case LicenseStatus::Valid:
            return "Valid";
        case LicenseStatus::Expired:
            return "Expired";
        case LicenseStatus::Invalid:
            return "Invalid";
        case LicenseStatus::Revoked:
            return "Revoked";
        case LicenseStatus::Suspended:
            return "Suspended";
        case LicenseStatus::GracePeriod:
            return "GracePeriod";
        case LicenseStatus::NotActivated:
            return "NotActivated";
        case LicenseStatus::HardwareMismatch:
            return "HardwareMismatch";
        case LicenseStatus::ConcurrencyLimit:
            return "ConcurrencyLimit";
        case LicenseStatus::Unknown:
            [[fallthrough]];
        default:
            return "Unknown";
    }
}

// ============================================
// Validation Result Code Enumeration
// ============================================

/**
 * @brief Strongly typed enumeration of validation result codes
 */
enum class ValidationCode : std::uint8_t
{
    Success,                  ///< Validation succeeded
    InvalidFormat,            ///< License format is invalid
    InvalidChecksum,          ///< Checksum verification failed
    InvalidSignature,         ///< Digital signature verification failed
    Expired,                  ///< License has expired
    NotYetValid,              ///< License not yet valid (future issue date)
    HardwareMismatch,         ///< Hardware ID mismatch
    FeatureNotAvailable,      ///< Requested feature not available
    ConcurrencyLimitExceeded, ///< Concurrent user limit exceeded
    LicenseRevoked,           ///< License has been revoked
    StorageError,             ///< Error reading/writing license
    NetworkError,             ///< Network verification failed
    InternalError,            ///< Internal validation error
    Unknown                   ///< Unknown error
};

/**
 * @brief Convert ValidationCode to string representation
 */
[[nodiscard]] constexpr auto validationCodeToString(ValidationCode code) noexcept -> std::string_view
{
    switch (code)
    {
        case ValidationCode::Success:
            return "Success";
        case ValidationCode::InvalidFormat:
            return "InvalidFormat";
        case ValidationCode::InvalidChecksum:
            return "InvalidChecksum";
        case ValidationCode::InvalidSignature:
            return "InvalidSignature";
        case ValidationCode::Expired:
            return "Expired";
        case ValidationCode::NotYetValid:
            return "NotYetValid";
        case ValidationCode::HardwareMismatch:
            return "HardwareMismatch";
        case ValidationCode::FeatureNotAvailable:
            return "FeatureNotAvailable";
        case ValidationCode::ConcurrencyLimitExceeded:
            return "ConcurrencyLimitExceeded";
        case ValidationCode::LicenseRevoked:
            return "LicenseRevoked";
        case ValidationCode::StorageError:
            return "StorageError";
        case ValidationCode::NetworkError:
            return "NetworkError";
        case ValidationCode::InternalError:
            return "InternalError";
        case ValidationCode::Unknown:
            [[fallthrough]];
        default:
            return "Unknown";
    }
}

// ============================================
// Validation Result Structure
// ============================================

/**
 * @brief Result of license validation with details
 */
struct ValidationResult
{
    ValidationCode code{ValidationCode::Unknown};
    std::string message;
    std::optional<std::string> details;
    std::chrono::system_clock::time_point timestamp;

    /**
     * @brief Check if validation succeeded
     */
    [[nodiscard]] auto isValid() const noexcept -> bool
    {
        return code == ValidationCode::Success;
    }

    /**
     * @brief Create a successful validation result
     */
    [[nodiscard]] static auto success(std::string_view msg = "Validation successful") -> ValidationResult
    {
        return ValidationResult{
            ValidationCode::Success,
            std::string(msg),
            std::nullopt,
            std::chrono::system_clock::now()};
    }

    /**
     * @brief Create a failed validation result
     */
    [[nodiscard]] static auto failure(ValidationCode code, std::string_view msg,
                                      std::optional<std::string> details = std::nullopt)
        -> ValidationResult
    {
        return ValidationResult{code, std::string(msg), std::move(details), std::chrono::system_clock::now()};
    }
};

// ============================================
// License Exception Hierarchy
// ============================================

/**
 * @brief Base exception class for all license-related errors
 */
class LicenseException : public std::runtime_error
{
public:
    explicit LicenseException(const std::string& message)
        : std::runtime_error(message)
        , m_code(ValidationCode::Unknown)
    {
    }

    LicenseException(const std::string& message, ValidationCode code)
        : std::runtime_error(message)
        , m_code(code)
    {
    }

    [[nodiscard]] auto code() const noexcept -> ValidationCode
    {
        return m_code;
    }

private:
    ValidationCode m_code;
};

/**
 * @brief Exception thrown when a license has expired
 */
class LicenseExpiredException : public LicenseException
{
public:
    explicit LicenseExpiredException(const std::string& message = "License has expired")
        : LicenseException(message, ValidationCode::Expired)
    {
    }

    LicenseExpiredException(const std::string& message,
                            std::chrono::system_clock::time_point expirationDate)
        : LicenseException(message, ValidationCode::Expired)
        , m_expirationDate(expirationDate)
    {
    }

    [[nodiscard]] auto expirationDate() const noexcept -> std::optional<std::chrono::system_clock::time_point>
    {
        return m_expirationDate;
    }

private:
    std::optional<std::chrono::system_clock::time_point> m_expirationDate;
};

/**
 * @brief Exception thrown when a license is invalid
 */
class LicenseInvalidException : public LicenseException
{
public:
    explicit LicenseInvalidException(const std::string& message = "License is invalid")
        : LicenseException(message, ValidationCode::InvalidFormat)
    {
    }

    LicenseInvalidException(const std::string& message, ValidationCode code)
        : LicenseException(message, code)
    {
    }
};

/**
 * @brief Exception thrown for license storage errors
 */
class LicenseStorageException : public LicenseException
{
public:
    explicit LicenseStorageException(const std::string& message = "License storage error")
        : LicenseException(message, ValidationCode::StorageError)
    {
    }
};

/**
 * @brief Exception thrown when signature verification fails
 */
class LicenseSignatureException : public LicenseException
{
public:
    explicit LicenseSignatureException(const std::string& message = "License signature verification failed")
        : LicenseException(message, ValidationCode::InvalidSignature)
    {
    }
};

/**
 * @brief Exception thrown when hardware ID doesn't match
 */
class LicenseHardwareMismatchException : public LicenseException
{
public:
    explicit LicenseHardwareMismatchException(const std::string& message = "Hardware ID mismatch")
        : LicenseException(message, ValidationCode::HardwareMismatch)
    {
    }

    LicenseHardwareMismatchException(const std::string& message,
                                     std::string_view expectedHardwareId,
                                     std::string_view actualHardwareId)
        : LicenseException(message, ValidationCode::HardwareMismatch)
        , m_expectedHardwareId(expectedHardwareId)
        , m_actualHardwareId(actualHardwareId)
    {
    }

    [[nodiscard]] auto expectedHardwareId() const noexcept -> const std::optional<std::string>&
    {
        return m_expectedHardwareId;
    }

    [[nodiscard]] auto actualHardwareId() const noexcept -> const std::optional<std::string>&
    {
        return m_actualHardwareId;
    }

private:
    std::optional<std::string> m_expectedHardwareId;
    std::optional<std::string> m_actualHardwareId;
};

/**
 * @brief Exception thrown when concurrent usage limit is exceeded
 */
class LicenseConcurrencyException : public LicenseException
{
public:
    explicit LicenseConcurrencyException(const std::string& message = "Concurrent usage limit exceeded")
        : LicenseException(message, ValidationCode::ConcurrencyLimitExceeded)
    {
    }

    LicenseConcurrencyException(const std::string& message, int maxAllowed, int currentCount)
        : LicenseException(message, ValidationCode::ConcurrencyLimitExceeded)
        , m_maxAllowed(maxAllowed)
        , m_currentCount(currentCount)
    {
    }

    [[nodiscard]] auto maxAllowed() const noexcept -> std::optional<int>
    {
        return m_maxAllowed;
    }

    [[nodiscard]] auto currentCount() const noexcept -> std::optional<int>
    {
        return m_currentCount;
    }

private:
    std::optional<int> m_maxAllowed;
    std::optional<int> m_currentCount;
};

/**
 * @brief Exception thrown when a requested feature is not available
 */
class LicenseFeatureException : public LicenseException
{
public:
    explicit LicenseFeatureException(const std::string& message = "Feature not available")
        : LicenseException(message, ValidationCode::FeatureNotAvailable)
    {
    }

    LicenseFeatureException(const std::string& message, std::string_view featureName)
        : LicenseException(message, ValidationCode::FeatureNotAvailable)
        , m_featureName(featureName)
    {
    }

    [[nodiscard]] auto featureName() const noexcept -> const std::optional<std::string>&
    {
        return m_featureName;
    }

private:
    std::optional<std::string> m_featureName;
};

// ============================================
// Type Aliases
// ============================================

using TimePoint   = std::chrono::system_clock::time_point;
using Duration    = std::chrono::system_clock::duration;
using FeatureList = std::vector<std::string>;

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSETYPES_HPP
