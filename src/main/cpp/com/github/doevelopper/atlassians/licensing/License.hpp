/**
 * @file   License.hpp
 * @brief  License entity class with Builder pattern
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSE_HPP

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <com/github/doevelopper/atlassians/licensing/LicenseTypes.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

class LicenseBuilder; // Forward declaration

/**
 * @brief License entity representing a software license
 *
 * This class follows modern C++ best practices:
 * - Rule of Five implementation
 * - PIMPL idiom for ABI stability
 * - Move semantics support
 * - const correctness
 * - noexcept specifications
 */
class License
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    // ============================================
    // Type Aliases
    // ============================================
    using TimePoint   = std::chrono::system_clock::time_point;
    using FeatureList = std::vector<std::string>;

    // ============================================
    // Rule of Five: Special Member Functions
    // ============================================

    /**
     * @brief Default constructor - creates an invalid license
     */
    License() noexcept;

    /**
     * @brief Destructor
     */
    ~License() noexcept;

    /**
     * @brief Copy constructor
     */
    License(const License& other);

    /**
     * @brief Copy assignment operator
     */
    auto operator=(const License& other) -> License&;

    /**
     * @brief Move constructor
     */
    License(License&& other) noexcept;

    /**
     * @brief Move assignment operator
     */
    auto operator=(License&& other) noexcept -> License&;

    // ============================================
    // Static Factory Methods
    // ============================================

    /**
     * @brief Create a LicenseBuilder for constructing a License
     */
    [[nodiscard]] static auto builder() -> LicenseBuilder;

    /**
     * @brief Create an empty/invalid license
     */
    [[nodiscard]] static auto createEmpty() -> License;

    // ============================================
    // Getters
    // ============================================

    /**
     * @brief Get the license key
     */
    [[nodiscard]] auto getLicenseKey() const noexcept -> std::string_view;

    /**
     * @brief Get the product ID
     */
    [[nodiscard]] auto getProductId() const noexcept -> std::string_view;

    /**
     * @brief Get the expiration date
     */
    [[nodiscard]] auto getExpirationDate() const noexcept -> TimePoint;

    /**
     * @brief Get the issue date
     */
    [[nodiscard]] auto getIssueDate() const noexcept -> TimePoint;

    /**
     * @brief Get the license type
     */
    [[nodiscard]] auto getType() const noexcept -> LicenseType;

    /**
     * @brief Get the list of enabled features
     */
    [[nodiscard]] auto getEnabledFeatures() const noexcept -> const FeatureList&;

    /**
     * @brief Get the maximum concurrent users (if applicable)
     */
    [[nodiscard]] auto getMaxConcurrentUsers() const noexcept -> std::optional<int>;

    /**
     * @brief Get the hardware ID (if applicable)
     */
    [[nodiscard]] auto getHardwareId() const noexcept -> std::optional<std::string_view>;

    /**
     * @brief Get the digital signature
     */
    [[nodiscard]] auto getDigitalSignature() const noexcept -> std::string_view;

    /**
     * @brief Get the licensee name
     */
    [[nodiscard]] auto getLicenseeName() const noexcept -> std::string_view;

    /**
     * @brief Get the licensee email
     */
    [[nodiscard]] auto getLicenseeEmail() const noexcept -> std::string_view;

    /**
     * @brief Get the license version
     */
    [[nodiscard]] auto getVersion() const noexcept -> std::string_view;

    /**
     * @brief Get custom metadata
     */
    [[nodiscard]] auto getMetadata() const noexcept -> const std::vector<std::pair<std::string, std::string>>&;

    // ============================================
    // Status Methods
    // ============================================

    /**
     * @brief Check if the license is valid (not expired, properly formatted)
     */
    [[nodiscard]] auto isValid() const noexcept -> bool;

    /**
     * @brief Check if the license has expired
     */
    [[nodiscard]] auto isExpired() const noexcept -> bool;

    /**
     * @brief Check if a specific feature is enabled
     */
    [[nodiscard]] auto hasFeature(std::string_view featureName) const noexcept -> bool;

    /**
     * @brief Check if the license is hardware-bound
     */
    [[nodiscard]] auto isHardwareBound() const noexcept -> bool;

    /**
     * @brief Check if the license has concurrent user limits
     */
    [[nodiscard]] auto hasConcurrencyLimit() const noexcept -> bool;

    /**
     * @brief Get remaining days until expiration
     * @return Number of days, or negative if expired
     */
    [[nodiscard]] auto getDaysUntilExpiration() const noexcept -> int;

    /**
     * @brief Get a formatted string representation of the license
     */
    [[nodiscard]] auto toString() const -> std::string;

    // ============================================
    // Comparison Operators
    // ============================================

    [[nodiscard]] auto operator==(const License& other) const noexcept -> bool;
    [[nodiscard]] auto operator!=(const License& other) const noexcept -> bool;

private:
    friend class LicenseBuilder;

    struct Impl;
    std::unique_ptr<Impl> m_pImpl;

    /**
     * @brief Private constructor used by LicenseBuilder
     */
    explicit License(std::unique_ptr<Impl> impl) noexcept;
};

/**
 * @brief Builder class for constructing License objects
 *
 * Implements the Builder pattern for creating complex License objects
 * with a fluent interface.
 */
class LicenseBuilder
{
public:
    LicenseBuilder();
    ~LicenseBuilder() noexcept;

    LicenseBuilder(const LicenseBuilder&) = delete;
    LicenseBuilder& operator=(const LicenseBuilder&) = delete;
    LicenseBuilder(LicenseBuilder&&) noexcept;
    LicenseBuilder& operator=(LicenseBuilder&&) noexcept;

    // ============================================
    // Builder Methods (Fluent Interface)
    // ============================================

    /**
     * @brief Set the license key
     */
    auto withLicenseKey(std::string_view key) -> LicenseBuilder&;

    /**
     * @brief Set the product ID
     */
    auto withProductId(std::string_view productId) -> LicenseBuilder&;

    /**
     * @brief Set the expiration date
     */
    auto withExpirationDate(License::TimePoint expirationDate) -> LicenseBuilder&;

    /**
     * @brief Set expiration to N days from now
     */
    auto expiresInDays(int days) -> LicenseBuilder&;

    /**
     * @brief Set the issue date
     */
    auto withIssueDate(License::TimePoint issueDate) -> LicenseBuilder&;

    /**
     * @brief Set issue date to now
     */
    auto issuedNow() -> LicenseBuilder&;

    /**
     * @brief Set the license type
     */
    auto withType(LicenseType type) -> LicenseBuilder&;

    /**
     * @brief Add a single enabled feature
     */
    auto withFeature(std::string_view feature) -> LicenseBuilder&;

    /**
     * @brief Add multiple enabled features
     */
    auto withFeatures(const std::vector<std::string>& features) -> LicenseBuilder&;

    /**
     * @brief Set the maximum concurrent users
     */
    auto withMaxConcurrentUsers(int maxUsers) -> LicenseBuilder&;

    /**
     * @brief Set the hardware ID
     */
    auto withHardwareId(std::string_view hardwareId) -> LicenseBuilder&;

    /**
     * @brief Set the digital signature
     */
    auto withDigitalSignature(std::string_view signature) -> LicenseBuilder&;

    /**
     * @brief Set the licensee name
     */
    auto withLicenseeName(std::string_view name) -> LicenseBuilder&;

    /**
     * @brief Set the licensee email
     */
    auto withLicenseeEmail(std::string_view email) -> LicenseBuilder&;

    /**
     * @brief Set the license version
     */
    auto withVersion(std::string_view version) -> LicenseBuilder&;

    /**
     * @brief Add custom metadata
     */
    auto withMetadata(std::string_view key, std::string_view value) -> LicenseBuilder&;

    // ============================================
    // Build Method
    // ============================================

    /**
     * @brief Build and return the License object
     * @throws LicenseInvalidException if required fields are missing
     */
    [[nodiscard]] auto build() -> License;

    /**
     * @brief Build and return the License object, or nullopt if invalid
     */
    [[nodiscard]] auto tryBuild() noexcept -> std::optional<License>;

private:
    struct BuilderImpl;
    std::unique_ptr<BuilderImpl> m_pImpl;
};

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSE_HPP
