/**
 * @file   LicenseManager.hpp
 * @brief  Thread-safe License Manager facade (Singleton pattern)
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSEMANAGER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSEMANAGER_HPP

#include <filesystem>
#include <memory>
#include <mutex>
#include <vector>

#include <com/github/doevelopper/atlassians/licensing/ILicenseObserver.hpp>
#include <com/github/doevelopper/atlassians/licensing/ILicenseStorage.hpp>
#include <com/github/doevelopper/atlassians/licensing/ILicenseValidator.hpp>
#include <com/github/doevelopper/atlassians/licensing/License.hpp>
#include <com/github/doevelopper/atlassians/licensing/ValidationChain.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

/**
 * @brief Thread-safe License Manager facade (Singleton pattern)
 *
 * This class provides a simplified API for license management operations.
 * It coordinates validators, storage, and observers to provide a complete
 * license management solution.
 *
 * Features:
 * - Thread-safe singleton pattern
 * - Facade pattern for simplified API
 * - Observer pattern for license events
 * - Strategy pattern for validators
 * - Chain of Responsibility for validation
 */
class LicenseManager
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    // ============================================
    // Singleton Access
    // ============================================

    /**
     * @brief Get the singleton instance
     * @return Shared pointer to the LicenseManager instance
     */
    [[nodiscard]] static auto getInstance() -> std::shared_ptr<LicenseManager>;

    /**
     * @brief Reset the singleton instance (primarily for testing)
     */
    static auto resetInstance() -> void;

    ~LicenseManager();

    // Prevent copying
    LicenseManager(const LicenseManager&)            = delete;
    LicenseManager& operator=(const LicenseManager&) = delete;
    LicenseManager(LicenseManager&&)                 = delete;
    LicenseManager& operator=(LicenseManager&&)      = delete;

    // ============================================
    // Configuration
    // ============================================

    /**
     * @brief Set the license storage implementation
     */
    auto setStorage(ILicenseStoragePtr storage) -> void;

    /**
     * @brief Add a validator to the validation chain
     */
    auto addValidator(ILicenseValidatorPtr validator) -> void;

    /**
     * @brief Remove a validator by name
     */
    auto removeValidator(std::string_view name) -> bool;

    /**
     * @brief Clear all validators
     */
    auto clearValidators() -> void;

    /**
     * @brief Register a license observer
     */
    auto registerObserver(ILicenseObserverPtr observer) -> void;

    /**
     * @brief Unregister a license observer
     */
    auto unregisterObserver(const ILicenseObserverPtr& observer) -> void;

    // ============================================
    // License Operations
    // ============================================

    /**
     * @brief Load a license from storage
     * @param licenseKey The license key to load
     * @return The loaded license
     * @throws LicenseStorageException if license cannot be loaded
     */
    [[nodiscard]] auto loadLicense(std::string_view licenseKey) -> License;

    /**
     * @brief Load the primary license from storage
     * @return The primary license
     * @throws LicenseStorageException if no primary license exists
     */
    [[nodiscard]] auto loadPrimaryLicense() -> License;

    /**
     * @brief Load license from a file
     * @param filePath Path to the license file
     * @return The loaded license
     */
    [[nodiscard]] auto loadLicenseFromFile(const std::filesystem::path& filePath) -> License;

    /**
     * @brief Save a license to storage
     * @param license The license to save
     */
    auto saveLicense(const License& license) -> void;

    /**
     * @brief Validate a license
     * @param license The license to validate
     * @return true if valid, false otherwise
     */
    [[nodiscard]] auto validate(const License& license) -> bool;

    /**
     * @brief Validate a license with detailed results
     * @param license The license to validate
     * @return ValidationResult with details
     */
    [[nodiscard]] auto validateWithDetails(const License& license) -> ValidationResult;

    /**
     * @brief Check if a feature is enabled for a license
     * @param license The license to check
     * @param featureName The feature to check
     * @return true if feature is enabled
     */
    [[nodiscard]] auto isFeatureEnabled(const License& license, std::string_view featureName) const -> bool;

    /**
     * @brief Get the current license status
     * @param license The license to check
     * @return The license status
     */
    [[nodiscard]] auto getLicenseStatus(const License& license) const -> LicenseStatus;

    // ============================================
    // Quick Validation Methods
    // ============================================

    /**
     * @brief Load and validate the primary license
     * @return true if primary license is valid
     */
    [[nodiscard]] auto isPrimaryLicenseValid() -> bool;

    /**
     * @brief Check if any valid license is available
     * @return true if a valid license exists
     */
    [[nodiscard]] auto hasValidLicense() -> bool;

    // ============================================
    // License Information
    // ============================================

    /**
     * @brief Get days until license expiration
     * @param license The license to check
     * @return Number of days until expiration, or negative if expired
     */
    [[nodiscard]] auto getDaysUntilExpiration(const License& license) const -> int;

    /**
     * @brief Get list of enabled features
     * @param license The license to check
     * @return List of feature names
     */
    [[nodiscard]] auto getEnabledFeatures(const License& license) const -> std::vector<std::string>;

private:
    // Private constructor for singleton
    LicenseManager();

    /**
     * @brief Notify observers of a license event
     */
    auto notifyObservers(LicenseEvent event, const License& license) -> void;

    /**
     * @brief Notify observers of a validation result
     */
    auto notifyValidationResult(const License& license, const ValidationResult& result) -> void;

    // ============================================
    // Member Variables
    // ============================================

    static std::shared_ptr<LicenseManager> s_instance;
    static std::mutex s_instanceMutex;

    mutable std::mutex m_mutex;
    ILicenseStoragePtr m_storage;
    std::unique_ptr<ValidationChain> m_validationChain;
    std::vector<ILicenseObserverWPtr> m_observers;
    std::optional<License> m_cachedLicense;
    std::optional<ValidationResult> m_cachedValidationResult;
};

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSEMANAGER_HPP
