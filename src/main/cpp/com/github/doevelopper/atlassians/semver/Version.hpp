/*******************************************************************
 * @file   Version.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Semantic Versioning 2.0.0 compliant Version class
 *
 * This implementation follows the Semantic Versioning 2.0.0 specification
 * as defined at https://semver.org/
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_VERSION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_VERSION_HPP

#include <algorithm>
#include <compare>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::semver
{

// Forward declarations
class VersionConstraint;

/**
 * @brief Exception class for version parsing and validation errors
 *
 * Thrown when a version string does not comply with the SemVer 2.0.0 specification.
 */
class VersionException : public std::runtime_error
{
public:
    /**
     * @brief Construct a VersionException with an error message
     * @param message The error description
     */
    explicit VersionException(const std::string& message)
        : std::runtime_error(message)
    {
    }

    /**
     * @brief Construct a VersionException with an error message
     * @param message The error description
     */
    explicit VersionException(const char* message)
        : std::runtime_error(message)
    {
    }
};

/**
 * @brief Semantic Versioning 2.0.0 compliant Version class
 *
 * This class implements the complete Semantic Versioning 2.0.0 specification
 * (https://semver.org/), including:
 *
 * - Version format: MAJOR.MINOR.PATCH
 * - Pre-release versions: MAJOR.MINOR.PATCH-prerelease
 * - Build metadata: MAJOR.MINOR.PATCH+build or MAJOR.MINOR.PATCH-prerelease+build
 * - Full precedence rules including pre-release comparison
 *
 * Features:
 * - Rule of Five compliant (PIMPL idiom for clean interface)
 * - Modern C++ design (C++17/20 features)
 * - SOLID principles adherence
 * - Thread-safe (const operations are thread-safe)
 * - Exception safety (strong exception guarantee)
 *
 * @example
 * @code
 * // Create versions from strings
 * Version v1 = Version::parse("1.2.3");
 * Version v2 = Version::parse("1.2.3-alpha.1");
 * Version v3 = Version::parse("1.2.3-alpha.1+build.123");
 *
 * // Create versions programmatically
 * Version v4(1, 2, 3);
 * Version v5(1, 2, 3, "beta.2");
 * Version v6(1, 2, 3, "rc.1", "build.456");
 *
 * // Compare versions
 * if (v1 > v2) { ... }  // true: release > pre-release
 *
 * // Increment versions
 * Version v7 = v1.incrementMajor();  // 2.0.0
 * Version v8 = v1.incrementMinor();  // 1.3.0
 * Version v9 = v1.incrementPatch();  // 1.2.4
 * @endcode
 */
class Version
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    // ============================================
    // Type Definitions
    // ============================================

    using IdentifierList = std::vector<std::string>;

    // ============================================
    // Static Constants
    // ============================================

    /**
     * @brief Regular expression pattern for validating SemVer 2.0.0 strings
     *
     * This pattern matches:
     * - MAJOR.MINOR.PATCH (required)
     * - -prerelease (optional)
     * - +build (optional)
     */
    static inline const std::string SEMVER_REGEX_PATTERN =
        R"(^(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$)";

    // ============================================
    // Rule of Five: Special Member Functions
    // ============================================

    /**
     * @brief Default constructor creating version 0.0.0
     */
    Version() noexcept;

    /**
     * @brief Construct a version with major, minor, and patch components
     * @param major Major version number (non-negative)
     * @param minor Minor version number (non-negative)
     * @param patch Patch version number (non-negative)
     */
    Version(std::uint32_t major, std::uint32_t minor, std::uint32_t patch);

    /**
     * @brief Construct a version with pre-release identifier
     * @param major Major version number
     * @param minor Minor version number
     * @param patch Patch version number
     * @param preRelease Pre-release identifier string (e.g., "alpha.1")
     * @throws VersionException if pre-release format is invalid
     */
    Version(std::uint32_t major, std::uint32_t minor, std::uint32_t patch,
            std::string_view preRelease);

    /**
     * @brief Construct a version with pre-release and build metadata
     * @param major Major version number
     * @param minor Minor version number
     * @param patch Patch version number
     * @param preRelease Pre-release identifier string
     * @param buildMetadata Build metadata string
     * @throws VersionException if format is invalid
     */
    Version(std::uint32_t major, std::uint32_t minor, std::uint32_t patch,
            std::string_view preRelease, std::string_view buildMetadata);

    /**
     * @brief Destructor
     */
    ~Version() noexcept;

    /**
     * @brief Copy constructor
     * @param other Version to copy from
     */
    Version(const Version& other);

    /**
     * @brief Copy assignment operator
     * @param other Version to copy from
     * @return Reference to this version
     */
    auto operator=(const Version& other) -> Version&;

    /**
     * @brief Move constructor
     * @param other Version to move from
     */
    Version(Version&& other) noexcept;

    /**
     * @brief Move assignment operator
     * @param other Version to move from
     * @return Reference to this version
     */
    auto operator=(Version&& other) noexcept -> Version&;

    // ============================================
    // Static Factory Methods
    // ============================================

    /**
     * @brief Parse a version string according to SemVer 2.0.0
     * @param versionString The version string to parse
     * @return The parsed Version object
     * @throws VersionException if the string is not valid SemVer 2.0.0
     */
    [[nodiscard]] static auto parse(std::string_view versionString) -> Version;

    /**
     * @brief Try to parse a version string, returning nullopt on failure
     * @param versionString The version string to parse
     * @return Optional Version, nullopt if parsing fails
     */
    [[nodiscard]] static auto tryParse(std::string_view versionString) noexcept
        -> std::optional<Version>;

    /**
     * @brief Check if a string is a valid SemVer 2.0.0 version
     * @param versionString The string to validate
     * @return true if valid, false otherwise
     */
    [[nodiscard]] static auto isValid(std::string_view versionString) noexcept -> bool;

    // ============================================
    // Accessors (Getters)
    // ============================================

    /**
     * @brief Get the major version number
     * @return Major version
     */
    [[nodiscard]] auto major() const noexcept -> std::uint32_t;

    /**
     * @brief Get the minor version number
     * @return Minor version
     */
    [[nodiscard]] auto minor() const noexcept -> std::uint32_t;

    /**
     * @brief Get the patch version number
     * @return Patch version
     */
    [[nodiscard]] auto patch() const noexcept -> std::uint32_t;

    /**
     * @brief Get the pre-release identifier string
     * @return Pre-release string, empty if no pre-release
     */
    [[nodiscard]] auto preRelease() const noexcept -> std::string_view;

    /**
     * @brief Get the pre-release identifiers as a list
     * @return Vector of pre-release identifier strings
     */
    [[nodiscard]] auto preReleaseIdentifiers() const noexcept -> const IdentifierList&;

    /**
     * @brief Get the build metadata string
     * @return Build metadata, empty if no build metadata
     */
    [[nodiscard]] auto buildMetadata() const noexcept -> std::string_view;

    /**
     * @brief Get the build metadata identifiers as a list
     * @return Vector of build metadata identifier strings
     */
    [[nodiscard]] auto buildMetadataIdentifiers() const noexcept -> const IdentifierList&;

    /**
     * @brief Check if this version has a pre-release identifier
     * @return true if has pre-release, false otherwise
     */
    [[nodiscard]] auto hasPreRelease() const noexcept -> bool;

    /**
     * @brief Check if this version has build metadata
     * @return true if has build metadata, false otherwise
     */
    [[nodiscard]] auto hasBuildMetadata() const noexcept -> bool;

    /**
     * @brief Check if this is a stable release (no pre-release identifier)
     * @return true if stable, false if pre-release
     */
    [[nodiscard]] auto isStable() const noexcept -> bool;

    /**
     * @brief Check if this is a pre-release version
     * @return true if pre-release, false if stable
     */
    [[nodiscard]] auto isPreRelease() const noexcept -> bool;

    // ============================================
    // Version Increment Operations
    // ============================================

    /**
     * @brief Create a new version with incremented major component
     *
     * Increments major, resets minor and patch to 0, clears pre-release.
     * Build metadata is preserved if specified.
     *
     * @param preserveBuildMetadata Whether to keep build metadata (default: false)
     * @return New Version with incremented major
     */
    [[nodiscard]] auto incrementMajor(bool preserveBuildMetadata = false) const -> Version;

    /**
     * @brief Create a new version with incremented minor component
     *
     * Increments minor, resets patch to 0, clears pre-release.
     * Build metadata is preserved if specified.
     *
     * @param preserveBuildMetadata Whether to keep build metadata (default: false)
     * @return New Version with incremented minor
     */
    [[nodiscard]] auto incrementMinor(bool preserveBuildMetadata = false) const -> Version;

    /**
     * @brief Create a new version with incremented patch component
     *
     * Increments patch, clears pre-release.
     * Build metadata is preserved if specified.
     *
     * @param preserveBuildMetadata Whether to keep build metadata (default: false)
     * @return New Version with incremented patch
     */
    [[nodiscard]] auto incrementPatch(bool preserveBuildMetadata = false) const -> Version;

    /**
     * @brief Create a new version with modified pre-release
     * @param preRelease New pre-release string
     * @return New Version with updated pre-release
     * @throws VersionException if pre-release format is invalid
     */
    [[nodiscard]] auto withPreRelease(std::string_view preRelease) const -> Version;

    /**
     * @brief Create a new version with modified build metadata
     * @param buildMetadata New build metadata string
     * @return New Version with updated build metadata
     * @throws VersionException if build metadata format is invalid
     */
    [[nodiscard]] auto withBuildMetadata(std::string_view buildMetadata) const -> Version;

    /**
     * @brief Create a new version without pre-release identifier
     * @return New Version without pre-release
     */
    [[nodiscard]] auto withoutPreRelease() const -> Version;

    /**
     * @brief Create a new version without build metadata
     * @return New Version without build metadata
     */
    [[nodiscard]] auto withoutBuildMetadata() const -> Version;

    // ============================================
    // Comparison Operations
    // ============================================

    /**
     * @brief Compare this version with another according to SemVer 2.0.0 precedence
     *
     * Comparison rules (per SemVer 2.0.0):
     * 1. Compare MAJOR, MINOR, PATCH numerically, left to right
     * 2. Pre-release version has LOWER precedence than normal version
     * 3. Pre-release identifiers are compared left to right:
     *    - Numeric identifiers compared as integers
     *    - Alphanumeric identifiers compared lexicographically
     *    - Numeric identifier < alphanumeric identifier
     *    - Larger set of identifiers > smaller set (if all preceding equal)
     * 4. Build metadata is IGNORED for precedence
     *
     * @param other Version to compare with
     * @return Negative if this < other, 0 if equal, positive if this > other
     */
    [[nodiscard]] auto compare(const Version& other) const noexcept -> int;

    /**
     * @brief Check if this version is compatible with a constraint
     * @param constraint The version constraint to check against
     * @return true if this version satisfies the constraint
     */
    [[nodiscard]] auto satisfies(const VersionConstraint& constraint) const -> bool;

    // ============================================
    // Comparison Operators
    // ============================================

    /**
     * @brief Equality operator (build metadata is IGNORED per SemVer 2.0.0)
     */
    [[nodiscard]] auto operator==(const Version& other) const noexcept -> bool;

    /**
     * @brief Inequality operator
     */
    [[nodiscard]] auto operator!=(const Version& other) const noexcept -> bool;

    /**
     * @brief Less than operator
     */
    [[nodiscard]] auto operator<(const Version& other) const noexcept -> bool;

    /**
     * @brief Less than or equal operator
     */
    [[nodiscard]] auto operator<=(const Version& other) const noexcept -> bool;

    /**
     * @brief Greater than operator
     */
    [[nodiscard]] auto operator>(const Version& other) const noexcept -> bool;

    /**
     * @brief Greater than or equal operator
     */
    [[nodiscard]] auto operator>=(const Version& other) const noexcept -> bool;

    /**
     * @brief Three-way comparison operator (C++20 spaceship operator)
     */
    [[nodiscard]] auto operator<=>(const Version& other) const noexcept -> std::strong_ordering;

    // ============================================
    // String Conversion
    // ============================================

    /**
     * @brief Convert version to SemVer 2.0.0 string representation
     * @return Version string in format MAJOR.MINOR.PATCH[-prerelease][+build]
     */
    [[nodiscard]] auto toString() const -> std::string;

    /**
     * @brief Convert version to core version string (MAJOR.MINOR.PATCH only)
     * @return Core version string without pre-release or build metadata
     */
    [[nodiscard]] auto toCoreString() const -> std::string;

    /**
     * @brief Implicit conversion to string
     */
    [[nodiscard]] explicit operator std::string() const;

    // ============================================
    // Stream Operators (Friend Functions)
    // ============================================

    /**
     * @brief Output stream operator
     */
    friend auto operator<<(std::ostream& os, const Version& version) -> std::ostream&;

    // ============================================
    // Hash Support
    // ============================================

    /**
     * @brief Compute hash value for the version
     * @return Hash value (build metadata is IGNORED per SemVer precedence rules)
     */
    [[nodiscard]] auto hash() const noexcept -> std::size_t;

private:
    // ============================================
    // PIMPL Implementation
    // ============================================

    struct Impl;
    std::unique_ptr<Impl> m_pImpl;

    // ============================================
    // Private Helper Functions
    // ============================================

    /**
     * @brief Validate pre-release identifier format
     */
    static auto validatePreReleaseIdentifier(std::string_view identifier) -> void;

    /**
     * @brief Validate build metadata identifier format
     */
    static auto validateBuildMetadataIdentifier(std::string_view identifier) -> void;

    /**
     * @brief Split a string by delimiter
     */
    [[nodiscard]] static auto split(std::string_view str, char delimiter) -> IdentifierList;

    /**
     * @brief Check if string is purely numeric
     */
    [[nodiscard]] static auto isNumeric(std::string_view str) noexcept -> bool;

    /**
     * @brief Compare two pre-release identifier strings
     */
    [[nodiscard]] static auto compareIdentifiers(std::string_view a, std::string_view b) noexcept -> int;
};

}  // namespace com::github::doevelopper::atlassians::semver

// ============================================
// Standard Library Hash Specialization
// ============================================

namespace std
{
template <>
struct hash<com::github::doevelopper::atlassians::semver::Version>
{
    auto operator()(const com::github::doevelopper::atlassians::semver::Version& v) const noexcept
        -> std::size_t
    {
        return v.hash();
    }
};
}  // namespace std

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_VERSION_HPP
