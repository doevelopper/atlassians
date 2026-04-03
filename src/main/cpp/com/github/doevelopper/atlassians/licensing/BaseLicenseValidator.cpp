/**
 * @file   BaseLicenseValidator.cpp
 * @brief  Base class implementation for license validators
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/licensing/BaseLicenseValidator.hpp>

using namespace com::github::doevelopper::atlassians::licensing;

log4cxx::LoggerPtr BaseLicenseValidator::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.licensing.BaseLicenseValidator"));

BaseLicenseValidator::BaseLicenseValidator(std::string_view name, int priority)
    : m_name(name)
    , m_priority(priority)
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " Creating validator: " << name);
}

BaseLicenseValidator::~BaseLicenseValidator()
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

auto BaseLicenseValidator::validate(const License& license) const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return validateWithDetails(license).isValid();
}

auto BaseLicenseValidator::validateWithDetails(const License& license) const -> ValidationResult
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " Validating license: " << license.getLicenseKey());

    // Step 1: Pre-validation hook
    auto preResult = preValidate(license);
    if (!preResult.isValid())
    {
        LOG4CXX_DEBUG(logger, "Pre-validation failed: " << preResult.message);
        return preResult;
    }

    // Step 2: Format validation
    auto formatResult = validateFormat(license);
    if (!formatResult.isValid())
    {
        LOG4CXX_DEBUG(logger, "Format validation failed: " << formatResult.message);
        return formatResult;
    }

    // Step 3: Main validation (implemented by subclass)
    auto mainResult = doValidate(license);
    if (!mainResult.isValid())
    {
        LOG4CXX_DEBUG(logger, "Main validation failed: " << mainResult.message);
        return mainResult;
    }

    // Step 4: Post-validation hook
    auto finalResult = postValidate(license, mainResult);

    LOG4CXX_DEBUG(logger, "Validation completed: " << (finalResult.isValid() ? "SUCCESS" : "FAILED"));
    return finalResult;
}

auto BaseLicenseValidator::getName() const noexcept -> std::string_view
{
    return m_name;
}

auto BaseLicenseValidator::getPriority() const noexcept -> int
{
    return m_priority;
}

auto BaseLicenseValidator::preValidate([[maybe_unused]] const License& license) const -> ValidationResult
{
    // Default implementation: always pass
    return ValidationResult::success("Pre-validation passed");
}

auto BaseLicenseValidator::validateFormat(const License& license) const -> ValidationResult
{
    // Default format validation
    if (!hasRequiredFields(license))
    {
        return ValidationResult::failure(
            ValidationCode::InvalidFormat,
            "License is missing required fields");
    }
    return ValidationResult::success("Format validation passed");
}

auto BaseLicenseValidator::postValidate(
    [[maybe_unused]] const License& license,
    const ValidationResult& result) const -> ValidationResult
{
    // Default implementation: return the result as-is
    return result;
}

auto BaseLicenseValidator::hasRequiredFields(const License& license) const -> bool
{
    return !license.getLicenseKey().empty() &&
           !license.getProductId().empty() &&
           license.getType() != LicenseType::Unknown;
}
