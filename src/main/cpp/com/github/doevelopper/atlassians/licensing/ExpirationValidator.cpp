/**
 * @file   ExpirationValidator.cpp
 * @brief  License expiration date validator implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/licensing/ExpirationValidator.hpp>

using namespace com::github::doevelopper::atlassians::licensing;

log4cxx::LoggerPtr ExpirationValidator::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.licensing.ExpirationValidator"));

ExpirationValidator::ExpirationValidator(int gracePeriodDays)
    : BaseLicenseValidator("ExpirationValidator", 10)
    , m_gracePeriodDays(gracePeriodDays)
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " Grace period: " << gracePeriodDays << " days");
}

ExpirationValidator::~ExpirationValidator()
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

auto ExpirationValidator::setGracePeriod(int days) noexcept -> void
{
    m_gracePeriodDays = days;
}

auto ExpirationValidator::getGracePeriod() const noexcept -> int
{
    return m_gracePeriodDays;
}

auto ExpirationValidator::doValidate(const License& license) const -> ValidationResult
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto expirationDate = license.getExpirationDate();

    // Check if expiration date is set (zero means perpetual)
    if (expirationDate == License::TimePoint{})
    {
        LOG4CXX_DEBUG(logger, "License has no expiration date (perpetual)");
        return ValidationResult::success("License is perpetual - no expiration");
    }

    auto now = std::chrono::system_clock::now();

    // Calculate expiration with grace period
    auto effectiveExpiration = expirationDate + std::chrono::hours(24 * m_gracePeriodDays);

    if (now > effectiveExpiration)
    {
        auto daysPastExpiration = std::chrono::duration_cast<std::chrono::hours>(now - expirationDate).count() / 24;

        std::string message = "License expired " + std::to_string(daysPastExpiration) + " days ago";

        LOG4CXX_DEBUG(logger, message);
        return ValidationResult::failure(ValidationCode::Expired, message);
    }

    // Check if in grace period
    if (now > expirationDate)
    {
        auto daysInGrace = std::chrono::duration_cast<std::chrono::hours>(now - expirationDate).count() / 24;
        auto daysRemaining = m_gracePeriodDays - static_cast<int>(daysInGrace);

        std::string message = "License in grace period - " + std::to_string(daysRemaining) + " days remaining";

        LOG4CXX_WARN(logger, message);
        // Return success but with a warning in details
        return ValidationResult{
            ValidationCode::Success,
            message,
            "License is in grace period. Please renew soon.",
            std::chrono::system_clock::now()};
    }

    // Check for upcoming expiration
    auto daysUntilExpiration = license.getDaysUntilExpiration();
    if (daysUntilExpiration <= 30)
    {
        std::string message = "License valid - expires in " + std::to_string(daysUntilExpiration) + " days";
        LOG4CXX_DEBUG(logger, message);

        return ValidationResult{
            ValidationCode::Success,
            message,
            "License is expiring soon. Consider renewing.",
            std::chrono::system_clock::now()};
    }

    return ValidationResult::success("License expiration check passed");
}
