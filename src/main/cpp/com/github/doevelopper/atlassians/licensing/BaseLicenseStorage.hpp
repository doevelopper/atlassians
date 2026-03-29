/**
 * @file   BaseLicenseStorage.hpp
 * @brief  Base class for license storage implementations
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_BASELICENSESTORAGE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_BASELICENSESTORAGE_HPP

#include <mutex>

#include <com/github/doevelopper/atlassians/licensing/ILicenseStorage.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

/**
 * @brief Base class for license storage with common functionality
 *
 * This class provides thread-safe storage operations and common
 * serialization/deserialization logic.
 */
class BaseLicenseStorage : public ILicenseStorage
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    explicit BaseLicenseStorage(std::string_view storageId);
    ~BaseLicenseStorage() override;

    // ============================================
    // Common Storage Operations
    // ============================================

    /**
     * @brief Thread-safe save operation
     */
    auto save(const License& license) -> void override;

    /**
     * @brief Thread-safe load operation
     */
    [[nodiscard]] auto load(std::string_view licenseKey) -> std::optional<License> override;

    /**
     * @brief Thread-safe check if license exists
     */
    [[nodiscard]] auto exists(std::string_view licenseKey) const -> bool override;

    /**
     * @brief Get the storage identifier
     */
    [[nodiscard]] auto getStorageLocation() const -> std::string override;

protected:
    // ============================================
    // Abstract Methods (Implement in subclasses)
    // ============================================

    /**
     * @brief Actually perform the save operation
     */
    virtual auto doSave(const License& license) -> void = 0;

    /**
     * @brief Actually perform the load operation
     */
    [[nodiscard]] virtual auto doLoad(std::string_view licenseKey) -> std::optional<License> = 0;

    /**
     * @brief Actually check if license exists
     */
    [[nodiscard]] virtual auto doExists(std::string_view licenseKey) const -> bool = 0;

    // ============================================
    // Serialization Utilities
    // ============================================

    /**
     * @brief Serialize license to string (JSON format)
     */
    [[nodiscard]] auto serializeLicense(const License& license) const -> std::string;

    /**
     * @brief Deserialize license from string (JSON format)
     */
    [[nodiscard]] auto deserializeLicense(std::string_view data) const -> std::optional<License>;

    /**
     * @brief Get the mutex for thread-safe operations
     */
    [[nodiscard]] auto getMutex() const -> std::mutex&;

private:
    std::string m_storageId;
    mutable std::mutex m_mutex;
};

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_BASELICENSESTORAGE_HPP
