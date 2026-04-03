/**
 * @file   ILicenseStorage.hpp
 * @brief  Pure virtual interface for license persistence
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSESTORAGE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSESTORAGE_HPP

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <com/github/doevelopper/atlassians/licensing/License.hpp>
#include <com/github/doevelopper/atlassians/licensing/LicenseTypes.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

/**
 * @brief Pure virtual interface for license storage/persistence
 *
 * This interface defines the contract for storing and retrieving
 * licenses. Concrete implementations can use files, databases,
 * or other storage mechanisms.
 */
class ILicenseStorage
{
public:
    virtual ~ILicenseStorage() = default;

    // ============================================
    // CRUD Operations
    // ============================================

    /**
     * @brief Save a license to storage
     * @param license The license to save
     * @throws LicenseStorageException on failure
     */
    virtual auto save(const License& license) -> void = 0;

    /**
     * @brief Load a license by its key
     * @param licenseKey The license key to load
     * @return The loaded license, or nullopt if not found
     * @throws LicenseStorageException on storage errors
     */
    [[nodiscard]] virtual auto load(std::string_view licenseKey) -> std::optional<License> = 0;

    /**
     * @brief Load the default/primary license
     * @return The primary license, or nullopt if none exists
     * @throws LicenseStorageException on storage errors
     */
    [[nodiscard]] virtual auto loadPrimary() -> std::optional<License> = 0;

    /**
     * @brief Delete a license from storage
     * @param licenseKey The license key to delete
     * @return true if deleted, false if not found
     * @throws LicenseStorageException on storage errors
     */
    virtual auto remove(std::string_view licenseKey) -> bool = 0;

    /**
     * @brief Check if a license exists in storage
     * @param licenseKey The license key to check
     */
    [[nodiscard]] virtual auto exists(std::string_view licenseKey) const -> bool = 0;

    /**
     * @brief List all stored license keys
     */
    [[nodiscard]] virtual auto listAll() const -> std::vector<std::string> = 0;

    // ============================================
    // Storage Management
    // ============================================

    /**
     * @brief Clear all stored licenses
     */
    virtual auto clear() -> void = 0;

    /**
     * @brief Get the storage location identifier
     */
    [[nodiscard]] virtual auto getStorageLocation() const -> std::string = 0;

    /**
     * @brief Check if storage is available and accessible
     */
    [[nodiscard]] virtual auto isAvailable() const noexcept -> bool = 0;

    // ============================================
    // Deleted Copy/Move (Interface)
    // ============================================

    ILicenseStorage(const ILicenseStorage&)            = delete;
    ILicenseStorage& operator=(const ILicenseStorage&) = delete;
    ILicenseStorage(ILicenseStorage&&)                 = default;
    ILicenseStorage& operator=(ILicenseStorage&&)      = default;

protected:
    ILicenseStorage() = default;
};

/**
 * @brief Shared pointer type for ILicenseStorage
 */
using ILicenseStoragePtr = std::shared_ptr<ILicenseStorage>;

/**
 * @brief Unique pointer type for ILicenseStorage
 */
using ILicenseStorageUPtr = std::unique_ptr<ILicenseStorage>;

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSESTORAGE_HPP
