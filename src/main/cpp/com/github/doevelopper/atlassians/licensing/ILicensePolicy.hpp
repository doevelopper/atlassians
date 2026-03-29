/**
 * @file   ILicensePolicy.hpp
 * @brief  Pure virtual interface for policy enforcement
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEPOLICY_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEPOLICY_HPP

#include <memory>
#include <string>

#include <com/github/doevelopper/atlassians/licensing/License.hpp>
#include <com/github/doevelopper/atlassians/licensing/LicenseTypes.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

/**
 * @brief Pure virtual interface for license policy enforcement
 *
 * This interface defines the contract for enforcing license policies.
 * Concrete implementations handle time limits, feature access,
 * concurrent usage, hardware binding, etc.
 */
class ILicensePolicy
{
public:
    virtual ~ILicensePolicy() = default;

    // ============================================
    // Policy Enforcement
    // ============================================

    /**
     * @brief Check if the license satisfies this policy
     * @param license The license to check
     * @return true if policy is satisfied, false otherwise
     */
    [[nodiscard]] virtual auto enforce(const License& license) const -> bool = 0;

    /**
     * @brief Check policy with detailed results
     * @param license The license to check
     * @return ValidationResult with details
     */
    [[nodiscard]] virtual auto enforceWithDetails(const License& license) const -> ValidationResult = 0;

    // ============================================
    // Policy Information
    // ============================================

    /**
     * @brief Get the name of this policy
     */
    [[nodiscard]] virtual auto getName() const noexcept -> std::string_view = 0;

    /**
     * @brief Get a description of this policy
     */
    [[nodiscard]] virtual auto getDescription() const noexcept -> std::string_view = 0;

    /**
     * @brief Get the priority of this policy (lower = higher priority)
     */
    [[nodiscard]] virtual auto getPriority() const noexcept -> int = 0;

    /**
     * @brief Check if this policy is mandatory
     */
    [[nodiscard]] virtual auto isMandatory() const noexcept -> bool = 0;

    // ============================================
    // Deleted Copy/Move (Interface)
    // ============================================

    ILicensePolicy(const ILicensePolicy&)            = delete;
    ILicensePolicy& operator=(const ILicensePolicy&) = delete;
    ILicensePolicy(ILicensePolicy&&)                 = default;
    ILicensePolicy& operator=(ILicensePolicy&&)      = default;

protected:
    ILicensePolicy() = default;
};

/**
 * @brief Shared pointer type for ILicensePolicy
 */
using ILicensePolicyPtr = std::shared_ptr<ILicensePolicy>;

/**
 * @brief Unique pointer type for ILicensePolicy
 */
using ILicensePolicyUPtr = std::unique_ptr<ILicensePolicy>;

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEPOLICY_HPP
