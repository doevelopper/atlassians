/**
 * @file   ValidationChain.hpp
 * @brief  Chain of Responsibility pattern for license validation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_VALIDATIONCHAIN_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_VALIDATIONCHAIN_HPP

#include <memory>
#include <vector>

#include <com/github/doevelopper/atlassians/licensing/ILicenseValidator.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

/**
 * @brief Chain of Responsibility implementation for license validation
 *
 * Manages a chain of validators that are executed in order.
 * Validation fails if any validator in the chain fails.
 */
class ValidationChain : public ILicenseValidator
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    ValidationChain();
    ~ValidationChain() override;

    // ============================================
    // Chain Management
    // ============================================

    /**
     * @brief Add a validator to the chain
     * @param validator The validator to add
     * @return Reference to this for fluent interface
     */
    auto addValidator(ILicenseValidatorPtr validator) -> ValidationChain&;

    /**
     * @brief Remove a validator from the chain by name
     */
    auto removeValidator(std::string_view name) -> bool;

    /**
     * @brief Clear all validators from the chain
     */
    auto clearValidators() -> void;

    /**
     * @brief Get the number of validators in the chain
     */
    [[nodiscard]] auto getValidatorCount() const noexcept -> std::size_t;

    /**
     * @brief Sort validators by priority
     */
    auto sortByPriority() -> void;

    // ============================================
    // ILicenseValidator Implementation
    // ============================================

    [[nodiscard]] auto validate(const License& license) const -> bool override;
    [[nodiscard]] auto validateWithDetails(const License& license) const -> ValidationResult override;
    [[nodiscard]] auto getName() const noexcept -> std::string_view override;
    [[nodiscard]] auto getPriority() const noexcept -> int override;

    /**
     * @brief Validate and collect all results (not stopping on first failure)
     */
    [[nodiscard]] auto validateAll(const License& license) const -> std::vector<ValidationResult>;

private:
    std::vector<ILicenseValidatorPtr> m_validators;
};

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_VALIDATIONCHAIN_HPP
