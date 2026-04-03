/**
 * @file   ILicenseValidator.hpp
 * @brief  Pure virtual interface for license validation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEVALIDATOR_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEVALIDATOR_HPP

#include <memory>
#include <string>

#include <com/github/doevelopper/atlassians/licensing/License.hpp>
#include <com/github/doevelopper/atlassians/licensing/LicenseTypes.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

/**
 * @brief Pure virtual interface for license validation
 *
 * This interface defines the contract for license validators using
 * the Strategy pattern. Different validation strategies can be
 * implemented by concrete classes.
 */
class ILicenseValidator
{
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~ILicenseValidator() = default;

    /**
     * @brief Validate a license
     * @param license The license to validate
     * @return true if the license is valid, false otherwise
     */
    [[nodiscard]] virtual auto validate(const License& license) const -> bool = 0;

    /**
     * @brief Validate a license with detailed results
     * @param license The license to validate
     * @return ValidationResult with details about the validation
     */
    [[nodiscard]] virtual auto validateWithDetails(const License& license) const -> ValidationResult = 0;

    /**
     * @brief Get the name of this validator
     */
    [[nodiscard]] virtual auto getName() const noexcept -> std::string_view = 0;

    /**
     * @brief Get the priority of this validator (lower = higher priority)
     */
    [[nodiscard]] virtual auto getPriority() const noexcept -> int = 0;

    // ============================================
    // Deleted Copy/Move (Interface)
    // ============================================

    ILicenseValidator(const ILicenseValidator&)            = delete;
    ILicenseValidator& operator=(const ILicenseValidator&) = delete;
    ILicenseValidator(ILicenseValidator&&)                 = default;
    ILicenseValidator& operator=(ILicenseValidator&&)      = default;

protected:
    ILicenseValidator() = default;
};

/**
 * @brief Shared pointer type for ILicenseValidator
 */
using ILicenseValidatorPtr = std::shared_ptr<ILicenseValidator>;

/**
 * @brief Unique pointer type for ILicenseValidator
 */
using ILicenseValidatorUPtr = std::unique_ptr<ILicenseValidator>;

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEVALIDATOR_HPP
