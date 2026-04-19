/**
 * @file   ILicenseGenerator.hpp
 * @brief  Pure virtual interface for license generation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEGENERATOR_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEGENERATOR_HPP

#include <memory>
#include <string>
#include <vector>

#include <com/github/doevelopper/atlassians/licensing/License.hpp>
#include <com/github/doevelopper/atlassians/licensing/LicenseTypes.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

/**
 * @brief Configuration for license generation
 */
struct LicenseGenerationConfig
{
    std::string productId;
    LicenseType type{LicenseType::Trial};
    int validityDays{30};
    std::vector<std::string> features;
    std::optional<int> maxConcurrentUsers;
    std::optional<std::string> hardwareId;
    std::string licenseeName;
    std::string licenseeEmail;
    std::vector<std::pair<std::string, std::string>> metadata;
};

/**
 * @brief Pure virtual interface for license generation
 *
 * This interface defines the contract for generating new licenses.
 * Concrete implementations handle key generation, signing, and
 * formatting of license data.
 */
class ILicenseGenerator
{
public:
    virtual ~ILicenseGenerator() = default;

    // ============================================
    // License Generation
    // ============================================

    /**
     * @brief Generate a new license
     * @param config Configuration for the license
     * @return The generated license
     * @throws LicenseException on generation failure
     */
    [[nodiscard]] virtual auto generate(const LicenseGenerationConfig& config) -> License = 0;

    /**
     * @brief Generate a license key
     * @return A new unique license key
     */
    [[nodiscard]] virtual auto generateLicenseKey() const -> std::string = 0;

    /**
     * @brief Generate a digital signature for license data
     * @param data The data to sign
     * @return The digital signature
     */
    [[nodiscard]] virtual auto signData(std::string_view data) const -> std::string = 0;

    // ============================================
    // Key Management
    // ============================================

    /**
     * @brief Set the private key for signing
     * @param privateKey The private key in PEM format
     */
    virtual auto setPrivateKey(std::string_view privateKey) -> void = 0;

    /**
     * @brief Load private key from file
     * @param filePath Path to the private key file
     * @throws LicenseException if file cannot be read
     */
    virtual auto loadPrivateKeyFromFile(const std::filesystem::path& filePath) -> void = 0;

    /**
     * @brief Check if private key is configured
     */
    [[nodiscard]] virtual auto hasPrivateKey() const noexcept -> bool = 0;

    // ============================================
    // Deleted Copy/Move (Interface)
    // ============================================

    ILicenseGenerator(const ILicenseGenerator&)            = delete;
    ILicenseGenerator& operator=(const ILicenseGenerator&) = delete;
    ILicenseGenerator(ILicenseGenerator&&)                 = default;
    ILicenseGenerator& operator=(ILicenseGenerator&&)      = default;

protected:
    ILicenseGenerator() = default;
};

/**
 * @brief Shared pointer type for ILicenseGenerator
 */
using ILicenseGeneratorPtr = std::shared_ptr<ILicenseGenerator>;

/**
 * @brief Unique pointer type for ILicenseGenerator
 */
using ILicenseGeneratorUPtr = std::unique_ptr<ILicenseGenerator>;

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEGENERATOR_HPP
