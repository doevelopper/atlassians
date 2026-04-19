/**
 * @file   FileLicenseStorage.hpp
 * @brief  File-based license storage implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_FILELICENSESTORAGE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_FILELICENSESTORAGE_HPP

#include <filesystem>

#include <com/github/doevelopper/atlassians/licensing/BaseLicenseStorage.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

/**
 * @brief File-based license storage implementation
 *
 * Stores licenses as individual files in a directory.
 * Each license is stored as a JSON file with the license key as filename.
 */
class FileLicenseStorage : public BaseLicenseStorage
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    /**
     * @brief Constructor with storage directory
     * @param storageDir Directory to store license files
     * @param createIfMissing Create directory if it doesn't exist
     */
    explicit FileLicenseStorage(
        const std::filesystem::path& storageDir,
        bool createIfMissing = true);

    ~FileLicenseStorage() override;

    // ============================================
    // ILicenseStorage Implementation
    // ============================================

    [[nodiscard]] auto loadPrimary() -> std::optional<License> override;
    auto remove(std::string_view licenseKey) -> bool override;
    [[nodiscard]] auto listAll() const -> std::vector<std::string> override;
    auto clear() -> void override;
    [[nodiscard]] auto isAvailable() const noexcept -> bool override;

protected:
    // ============================================
    // BaseLicenseStorage Implementation
    // ============================================

    auto doSave(const License& license) -> void override;
    [[nodiscard]] auto doLoad(std::string_view licenseKey) -> std::optional<License> override;
    [[nodiscard]] auto doExists(std::string_view licenseKey) const -> bool override;

private:
    /**
     * @brief Get the file path for a license key
     */
    [[nodiscard]] auto getLicenseFilePath(std::string_view licenseKey) const -> std::filesystem::path;

    /**
     * @brief Sanitize license key for use as filename
     */
    [[nodiscard]] auto sanitizeFilename(std::string_view licenseKey) const -> std::string;

    std::filesystem::path m_storageDir;
    std::string m_primaryLicenseFile{"primary.lic"};
};

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_FILELICENSESTORAGE_HPP
