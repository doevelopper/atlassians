/**
 * @file ErrorCode.hpp
 * @brief 64-bit structured error code implementation for embedded systems
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 *
 * This file defines the core 64-bit error code structure:
 * ┌─────────────────┬──────────┬──────────┬────────────┬──────────────────┐
 * │ Recovery Class  │ Reserved │ Services │   Mission  │      Code        │
 * │    (Severity)   │          │          │   Defined  │                  │
 * ├─────────────────┼──────────┼──────────┼────────────┼──────────────────┤
 * │     3 bits      │  4 bits  │  5 bits  │  12 bits   │    40 bits       │
 * ├─────────────────┼──────────┼──────────┼────────────┼──────────────────┤
 * │   Bits 63-61    │Bits 60-57│Bits 56-52│ Bits 51-40 │   Bits 39-0      │
 * └─────────────────┴──────────┴──────────┴────────────┴──────────────────┘
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORCODE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORCODE_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <compare>
#include <optional>
#include <functional>
#include <memory>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::triada
{

/**
 * @brief Error severity levels (Recovery Class)
 *
 * Defines the severity of an error and the expected recovery behavior.
 * Values 0-7 map to the 3-bit Recovery Class field.
 */
enum class ErrorSeverity : std::uint8_t
{
    Trace = 0,           ///< Trace information for debugging
    Debug = 1,           ///< Debug information, continue operation
    Informational = 2,   ///< Informational message, log for review
    Warning = 3,         ///< Warning, elevated priority logging
    Recoverable = 4,     ///< Recoverable error, initiate recovery
    Error = 5,           ///< Error requiring attention
    Critical = 6,        ///< Critical error, emergency recovery needed
    Fatal = 7            ///< Fatal error, immediate shutdown required
};

/**
 * @brief Service/subsystem identifiers
 *
 * Defines the service or subsystem where an error originated.
 * Values 0-31 map to the 5-bit Services field.
 */
enum class ServiceId : std::uint8_t
{
    Core = 0,            ///< Core system services
    Memory = 1,          ///< Memory management
    IO = 2,              ///< Input/Output operations
    Network = 3,         ///< Network communications
    Storage = 4,         ///< Storage/persistence
    Security = 5,        ///< Security subsystem
    Configuration = 6,   ///< Configuration management
    Logging = 7,         ///< Logging subsystem
    Communication = 8,   ///< Inter-process communication
    Hardware = 9,        ///< Hardware abstraction
    Scheduler = 10,      ///< Task scheduling
    Timer = 11,          ///< Timer services
    Watchdog = 12,       ///< Watchdog services
    Power = 13,          ///< Power management
    Diagnostic = 14,     ///< Diagnostics and monitoring
    Protocol = 15,       ///< Protocol handlers
    Application = 16,    ///< Application layer
    User = 17,           ///< User-defined services
    External = 18,       ///< External interfaces
    Test = 19,           ///< Testing subsystem
    Reserved20 = 20,
    Reserved21 = 21,
    Reserved22 = 22,
    Reserved23 = 23,
    Reserved24 = 24,
    Reserved25 = 25,
    Reserved26 = 26,
    Reserved27 = 27,
    Reserved28 = 28,
    Reserved29 = 29,
    Reserved30 = 30,
    Vendor = 31          ///< Vendor-specific
};

/**
 * @brief 64-bit structured error code
 *
 * Provides a comprehensive error code structure with fields for:
 * - Recovery Class (Severity): 3 bits [63-61]
 * - Reserved: 4 bits [60-57]
 * - Services: 5 bits [56-52]
 * - Mission Defined: 12 bits [51-40]
 * - Code: 40 bits [39-0]
 *
 * @example
 * @code
 * ErrorCode err = ErrorCode::create(
 *     ErrorSeverity::Recoverable,
 *     ServiceId::Network,
 *     0x001,  // Mission-defined: connection errors
 *     0x0000000001  // Code: timeout
 * );
 *
 * if (err.isRecoverable()) {
 *     // Attempt recovery
 * }
 * @endcode
 */
class ErrorCode
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    // Type aliases
    using ValueType = std::uint64_t;
    using CodeType = std::uint64_t;
    using MissionType = std::uint16_t;

    // Bit field masks and shifts
    static constexpr int SEVERITY_SHIFT = 61;
    static constexpr int RESERVED_SHIFT = 57;
    static constexpr int SERVICE_SHIFT = 52;
    static constexpr int MISSION_SHIFT = 40;
    static constexpr int CODE_SHIFT = 0;

    static constexpr ValueType SEVERITY_MASK = 0x7ULL << SEVERITY_SHIFT;  // 3 bits
    static constexpr ValueType RESERVED_MASK = 0xFULL << RESERVED_SHIFT;  // 4 bits
    static constexpr ValueType SERVICE_MASK = 0x1FULL << SERVICE_SHIFT;   // 5 bits
    static constexpr ValueType MISSION_MASK = 0xFFFULL << MISSION_SHIFT;  // 12 bits
    static constexpr ValueType CODE_MASK = 0xFFFFFFFFFFULL;               // 40 bits

    // Maximum values for each field
    static constexpr std::uint8_t MAX_SEVERITY = 7;
    static constexpr std::uint8_t MAX_RESERVED = 15;
    static constexpr std::uint8_t MAX_SERVICE = 31;
    static constexpr MissionType MAX_MISSION = 4095;
    static constexpr CodeType MAX_CODE = 0xFFFFFFFFFFULL;

    // ============================================
    // Rule of Five: Special Member Functions
    // ============================================

    /**
     * @brief Default constructor creates a success code
     */
    constexpr ErrorCode() noexcept;

    /**
     * @brief Construct from raw 64-bit value
     * @param value Raw error code value
     */
    explicit constexpr ErrorCode(ValueType value) noexcept;

    /**
     * @brief Destructor
     */
    ~ErrorCode() noexcept = default;

    /**
     * @brief Copy constructor
     */
    constexpr ErrorCode(const ErrorCode& other) noexcept = default;

    /**
     * @brief Copy assignment operator
     */
    constexpr auto operator=(const ErrorCode& other) noexcept -> ErrorCode& = default;

    /**
     * @brief Move constructor
     */
    constexpr ErrorCode(ErrorCode&& other) noexcept = default;

    /**
     * @brief Move assignment operator
     */
    constexpr auto operator=(ErrorCode&& other) noexcept -> ErrorCode& = default;

    // ============================================
    // Factory Methods
    // ============================================

    /**
     * @brief Create an error code from individual components
     * @param severity Error severity level
     * @param service Service/subsystem identifier
     * @param mission Mission-defined classification
     * @param code Unique error code within category
     * @param reserved Reserved field value (default 0)
     * @return Constructed ErrorCode
     */
    [[nodiscard]] static constexpr auto create(
        ErrorSeverity severity,
        ServiceId service,
        MissionType mission,
        CodeType code,
        std::uint8_t reserved = 0) noexcept -> ErrorCode;

    /**
     * @brief Create a success code (no error)
     */
    [[nodiscard]] static constexpr auto success() noexcept -> ErrorCode;

    // ============================================
    // Field Accessors
    // ============================================

    /**
     * @brief Get the raw 64-bit value
     */
    [[nodiscard]] constexpr auto value() const noexcept -> ValueType;

    /**
     * @brief Get the severity level
     */
    [[nodiscard]] constexpr auto severity() const noexcept -> ErrorSeverity;

    /**
     * @brief Get the reserved field value
     */
    [[nodiscard]] constexpr auto reserved() const noexcept -> std::uint8_t;

    /**
     * @brief Get the service identifier
     */
    [[nodiscard]] constexpr auto service() const noexcept -> ServiceId;

    /**
     * @brief Get the mission-defined classification
     */
    [[nodiscard]] constexpr auto mission() const noexcept -> MissionType;

    /**
     * @brief Get the error code
     */
    [[nodiscard]] constexpr auto code() const noexcept -> CodeType;

    // ============================================
    // Field Modifiers
    // ============================================

    /**
     * @brief Set the severity level
     * @param sev New severity level
     * @return Reference to this for chaining
     */
    constexpr auto withSeverity(ErrorSeverity sev) noexcept -> ErrorCode&;

    /**
     * @brief Set the reserved field
     * @param res New reserved value
     * @return Reference to this for chaining
     */
    constexpr auto withReserved(std::uint8_t res) noexcept -> ErrorCode&;

    /**
     * @brief Set the service identifier
     * @param svc New service identifier
     * @return Reference to this for chaining
     */
    constexpr auto withService(ServiceId svc) noexcept -> ErrorCode&;

    /**
     * @brief Set the mission-defined value
     * @param mis New mission value
     * @return Reference to this for chaining
     */
    constexpr auto withMission(MissionType mis) noexcept -> ErrorCode&;

    /**
     * @brief Set the error code
     * @param cod New code value
     * @return Reference to this for chaining
     */
    constexpr auto withCode(CodeType cod) noexcept -> ErrorCode&;

    // ============================================
    // Status Queries
    // ============================================

    /**
     * @brief Check if this represents success (no error)
     */
    [[nodiscard]] constexpr auto isSuccess() const noexcept -> bool;

    /**
     * @brief Check if this represents an error
     */
    [[nodiscard]] constexpr auto isError() const noexcept -> bool;

    /**
     * @brief Check if this is a warning
     */
    [[nodiscard]] constexpr auto isWarning() const noexcept -> bool;

    /**
     * @brief Check if this error is recoverable
     */
    [[nodiscard]] constexpr auto isRecoverable() const noexcept -> bool;

    /**
     * @brief Check if this is a fatal error
     */
    [[nodiscard]] constexpr auto isFatal() const noexcept -> bool;

    /**
     * @brief Check if this is a critical error
     */
    [[nodiscard]] constexpr auto isCritical() const noexcept -> bool;

    /**
     * @brief Explicit bool conversion (true if error)
     */
    [[nodiscard]] explicit constexpr operator bool() const noexcept;

    // ============================================
    // Comparison Operators
    // ============================================

    [[nodiscard]] constexpr auto operator==(const ErrorCode& other) const noexcept -> bool = default;
    [[nodiscard]] constexpr auto operator<=>(const ErrorCode& other) const noexcept = default;

    // ============================================
    // String Conversion
    // ============================================

    /**
     * @brief Get severity as string
     */
    [[nodiscard]] auto severityString() const -> std::string_view;

    /**
     * @brief Get service as string
     */
    [[nodiscard]] auto serviceString() const -> std::string_view;

    /**
     * @brief Convert to hexadecimal string representation
     */
    [[nodiscard]] auto toHexString() const -> std::string;

    /**
     * @brief Convert to human-readable string
     */
    [[nodiscard]] auto toString() const -> std::string;

    // ============================================
    // Hash Support
    // ============================================

    /**
     * @brief Get hash value for use in containers
     */
    [[nodiscard]] auto hash() const noexcept -> std::size_t;

private:
    ValueType m_value{0};
};

// ============================================
// Inline Constexpr Implementations
// ============================================

constexpr ErrorCode::ErrorCode() noexcept
    : m_value{0}
{
}

constexpr ErrorCode::ErrorCode(ValueType value) noexcept
    : m_value{value}
{
}

constexpr auto ErrorCode::create(
    ErrorSeverity severity,
    ServiceId service,
    MissionType mission,
    CodeType code,
    std::uint8_t reserved) noexcept -> ErrorCode
{
    ValueType value = 0;
    value |= (static_cast<ValueType>(severity) & 0x7ULL) << SEVERITY_SHIFT;
    value |= (static_cast<ValueType>(reserved) & 0xFULL) << RESERVED_SHIFT;
    value |= (static_cast<ValueType>(service) & 0x1FULL) << SERVICE_SHIFT;
    value |= (static_cast<ValueType>(mission) & 0xFFFULL) << MISSION_SHIFT;
    value |= (code & CODE_MASK);
    return ErrorCode{value};
}

constexpr auto ErrorCode::success() noexcept -> ErrorCode
{
    return ErrorCode{0};
}

constexpr auto ErrorCode::value() const noexcept -> ValueType
{
    return m_value;
}

constexpr auto ErrorCode::severity() const noexcept -> ErrorSeverity
{
    return static_cast<ErrorSeverity>((m_value & SEVERITY_MASK) >> SEVERITY_SHIFT);
}

constexpr auto ErrorCode::reserved() const noexcept -> std::uint8_t
{
    return static_cast<std::uint8_t>((m_value & RESERVED_MASK) >> RESERVED_SHIFT);
}

constexpr auto ErrorCode::service() const noexcept -> ServiceId
{
    return static_cast<ServiceId>((m_value & SERVICE_MASK) >> SERVICE_SHIFT);
}

constexpr auto ErrorCode::mission() const noexcept -> MissionType
{
    return static_cast<MissionType>((m_value & MISSION_MASK) >> MISSION_SHIFT);
}

constexpr auto ErrorCode::code() const noexcept -> CodeType
{
    return m_value & CODE_MASK;
}

constexpr auto ErrorCode::withSeverity(ErrorSeverity sev) noexcept -> ErrorCode&
{
    m_value = (m_value & ~SEVERITY_MASK) |
              ((static_cast<ValueType>(sev) & 0x7ULL) << SEVERITY_SHIFT);
    return *this;
}

constexpr auto ErrorCode::withReserved(std::uint8_t res) noexcept -> ErrorCode&
{
    m_value = (m_value & ~RESERVED_MASK) |
              ((static_cast<ValueType>(res) & 0xFULL) << RESERVED_SHIFT);
    return *this;
}

constexpr auto ErrorCode::withService(ServiceId svc) noexcept -> ErrorCode&
{
    m_value = (m_value & ~SERVICE_MASK) |
              ((static_cast<ValueType>(svc) & 0x1FULL) << SERVICE_SHIFT);
    return *this;
}

constexpr auto ErrorCode::withMission(MissionType mis) noexcept -> ErrorCode&
{
    m_value = (m_value & ~MISSION_MASK) |
              ((static_cast<ValueType>(mis) & 0xFFFULL) << MISSION_SHIFT);
    return *this;
}

constexpr auto ErrorCode::withCode(CodeType cod) noexcept -> ErrorCode&
{
    m_value = (m_value & ~CODE_MASK) | (cod & CODE_MASK);
    return *this;
}

constexpr auto ErrorCode::isSuccess() const noexcept -> bool
{
    return m_value == 0;
}

constexpr auto ErrorCode::isError() const noexcept -> bool
{
    return severity() >= ErrorSeverity::Recoverable;
}

constexpr auto ErrorCode::isWarning() const noexcept -> bool
{
    return severity() == ErrorSeverity::Warning;
}

constexpr auto ErrorCode::isRecoverable() const noexcept -> bool
{
    auto sev = severity();
    return sev >= ErrorSeverity::Recoverable && sev < ErrorSeverity::Fatal;
}

constexpr auto ErrorCode::isFatal() const noexcept -> bool
{
    return severity() == ErrorSeverity::Fatal;
}

constexpr auto ErrorCode::isCritical() const noexcept -> bool
{
    return severity() >= ErrorSeverity::Critical;
}

constexpr ErrorCode::operator bool() const noexcept
{
    return m_value != 0;
}

inline auto ErrorCode::hash() const noexcept -> std::size_t
{
    return std::hash<ValueType>{}(m_value);
}

} // namespace com::github::doevelopper::atlassians::triada

// Specialization of std::hash for ErrorCode
template<>
struct std::hash<com::github::doevelopper::atlassians::triada::ErrorCode>
{
    [[nodiscard]] auto operator()(
        const com::github::doevelopper::atlassians::triada::ErrorCode& ec) const noexcept -> std::size_t
    {
        return ec.hash();
    }
};

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORCODE_HPP
