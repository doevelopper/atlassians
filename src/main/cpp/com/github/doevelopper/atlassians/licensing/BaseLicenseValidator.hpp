/**
 * @file   BaseLicenseValidator.hpp
 * @brief  Base class for license validators using Template Method pattern
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_BASELICENSEVALIDATOR_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_BASELICENSEVALIDATOR_HPP

#include <com/github/doevelopper/atlassians/licensing/ILicenseValidator.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

/**
 * @brief Base class for license validators implementing Template Method pattern
 *
 * This class provides a template method for validation that calls several
 * protected virtual methods. Subclasses can override specific validation
 * steps while inheriting the common validation workflow.
 */
class BaseLicenseValidator : public ILicenseValidator
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    /**
     * @brief Constructor with validator name
     */
    explicit BaseLicenseValidator(std::string_view name, int priority = 100);

    ~BaseLicenseValidator() override;

    // ============================================
    // ILicenseValidator Implementation
    // ============================================

    /**
     * @brief Template method for license validation
     */
    [[nodiscard]] auto validate(const License& license) const -> bool override;

    /**
     * @brief Template method for license validation with details
     */
    [[nodiscard]] auto validateWithDetails(const License& license) const -> ValidationResult override;

    [[nodiscard]] auto getName() const noexcept -> std::string_view override;

    [[nodiscard]] auto getPriority() const noexcept -> int override;

protected:
    // ============================================
    // Template Method Hooks (Override in subclasses)
    // ============================================

    /**
     * @brief Pre-validation hook (called before main validation)
     * @return ValidationResult - continue if successful
     */
    [[nodiscard]] virtual auto preValidate(const License& license) const -> ValidationResult;

    /**
     * @brief Format validation hook
     * @return ValidationResult - continue if successful
     */
    [[nodiscard]] virtual auto validateFormat(const License& license) const -> ValidationResult;

    /**
     * @brief Main validation logic (must be overridden)
     * @return ValidationResult with validation details
     */
    [[nodiscard]] virtual auto doValidate(const License& license) const -> ValidationResult = 0;

    /**
     * @brief Post-validation hook (called after main validation)
     * @return ValidationResult - can modify or finalize result
     */
    [[nodiscard]] virtual auto postValidate(const License& license, const ValidationResult& result) const
        -> ValidationResult;

    // ============================================
    // Utility Methods for Subclasses
    // ============================================

    /**
     * @brief Check if license has required fields
     */
    [[nodiscard]] auto hasRequiredFields(const License& license) const -> bool;

private:
    std::string m_name;
    int m_priority;
};

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_BASELICENSEVALIDATOR_HPP
