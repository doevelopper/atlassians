/**
 * @file   ExpirationValidator.hpp
 * @brief  License expiration date validator
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_EXPIRATIONVALIDATOR_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_EXPIRATIONVALIDATOR_HPP

#include <com/github/doevelopper/atlassians/licensing/BaseLicenseValidator.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

/**
 * @brief Validator for license expiration dates
 *
 * Checks if the license has expired and optionally supports
 * grace periods for recently expired licenses.
 */
class ExpirationValidator : public BaseLicenseValidator
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    /**
     * @brief Constructor
     * @param gracePeriodDays Number of days grace period after expiration (0 = no grace)
     */
    explicit ExpirationValidator(int gracePeriodDays = 0);

    ~ExpirationValidator() override;

    /**
     * @brief Set the grace period
     */
    auto setGracePeriod(int days) noexcept -> void;

    /**
     * @brief Get the current grace period
     */
    [[nodiscard]] auto getGracePeriod() const noexcept -> int;

protected:
    [[nodiscard]] auto doValidate(const License& license) const -> ValidationResult override;

private:
    int m_gracePeriodDays;
};

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_EXPIRATIONVALIDATOR_HPP
