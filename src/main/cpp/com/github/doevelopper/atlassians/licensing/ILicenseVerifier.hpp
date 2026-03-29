/**
 * @file   ILicenseVerifier.hpp
 * @brief  Pure virtual interface for cryptographic verification
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEVERIFIER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEVERIFIER_HPP

#include <memory>
#include <string>
#include <vector>

#include <com/github/doevelopper/atlassians/licensing/License.hpp>
#include <com/github/doevelopper/atlassians/licensing/LicenseTypes.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

/**
 * @brief Pure virtual interface for cryptographic license verification
 *
 * This interface defines the contract for verifying digital signatures
 * on licenses. Concrete implementations can use RSA, ECDSA, or other
 * cryptographic algorithms.
 */
class ILicenseVerifier
{
public:
    virtual ~ILicenseVerifier() = default;

    // ============================================
    // Verification Operations
    // ============================================

    /**
     * @brief Verify the digital signature of a license
     * @param license The license to verify
     * @return true if signature is valid, false otherwise
     */
    [[nodiscard]] virtual auto verifySignature(const License& license) const -> bool = 0;

    /**
     * @brief Verify a signature against data
     * @param data The data that was signed
     * @param signature The signature to verify
     * @return true if signature is valid, false otherwise
     */
    [[nodiscard]] virtual auto verifySignature(std::string_view data, std::string_view signature) const -> bool = 0;

    /**
     * @brief Verify the digital signature with detailed result
     * @param license The license to verify
     * @return ValidationResult with details
     */
    [[nodiscard]] virtual auto verifySignatureWithDetails(const License& license) const -> ValidationResult = 0;

    // ============================================
    // Key Management
    // ============================================

    /**
     * @brief Set the public key for verification
     * @param publicKey The public key in PEM format
     */
    virtual auto setPublicKey(std::string_view publicKey) -> void = 0;

    /**
     * @brief Load public key from file
     * @param filePath Path to the public key file
     * @throws LicenseException if file cannot be read
     */
    virtual auto loadPublicKeyFromFile(const std::filesystem::path& filePath) -> void = 0;

    /**
     * @brief Check if public key is configured
     */
    [[nodiscard]] virtual auto hasPublicKey() const noexcept -> bool = 0;

    // ============================================
    // Algorithm Information
    // ============================================

    /**
     * @brief Get the name of the verification algorithm
     */
    [[nodiscard]] virtual auto getAlgorithmName() const noexcept -> std::string_view = 0;

    /**
     * @brief Get the key size in bits
     */
    [[nodiscard]] virtual auto getKeySize() const noexcept -> int = 0;

    // ============================================
    // Deleted Copy/Move (Interface)
    // ============================================

    ILicenseVerifier(const ILicenseVerifier&)            = delete;
    ILicenseVerifier& operator=(const ILicenseVerifier&) = delete;
    ILicenseVerifier(ILicenseVerifier&&)                 = default;
    ILicenseVerifier& operator=(ILicenseVerifier&&)      = default;

protected:
    ILicenseVerifier() = default;
};

/**
 * @brief Shared pointer type for ILicenseVerifier
 */
using ILicenseVerifierPtr = std::shared_ptr<ILicenseVerifier>;

/**
 * @brief Unique pointer type for ILicenseVerifier
 */
using ILicenseVerifierUPtr = std::unique_ptr<ILicenseVerifier>;

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEVERIFIER_HPP
