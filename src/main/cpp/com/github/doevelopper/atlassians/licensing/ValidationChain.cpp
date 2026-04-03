/**
 * @file   ValidationChain.cpp
 * @brief  Chain of Responsibility implementation for license validation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/licensing/ValidationChain.hpp>

#include <algorithm>

using namespace com::github::doevelopper::atlassians::licensing;

log4cxx::LoggerPtr ValidationChain::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.licensing.ValidationChain"));

ValidationChain::ValidationChain()
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

ValidationChain::~ValidationChain()
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

auto ValidationChain::addValidator(ILicenseValidatorPtr validator) -> ValidationChain&
{
    if (validator)
    {
        LOG4CXX_DEBUG(logger, "Adding validator: " << validator->getName()
                                                   << " (priority: " << validator->getPriority() << ")");
        m_validators.push_back(std::move(validator));
    }
    return *this;
}

auto ValidationChain::removeValidator(std::string_view name) -> bool
{
    auto it = std::remove_if(m_validators.begin(), m_validators.end(),
                             [name](const ILicenseValidatorPtr& v) {
                                 return v && v->getName() == name;
                             });

    if (it != m_validators.end())
    {
        m_validators.erase(it, m_validators.end());
        LOG4CXX_DEBUG(logger, "Removed validator: " << name);
        return true;
    }
    return false;
}

auto ValidationChain::clearValidators() -> void
{
    m_validators.clear();
    LOG4CXX_DEBUG(logger, "Cleared all validators");
}

auto ValidationChain::getValidatorCount() const noexcept -> std::size_t
{
    return m_validators.size();
}

auto ValidationChain::sortByPriority() -> void
{
    std::sort(m_validators.begin(), m_validators.end(),
              [](const ILicenseValidatorPtr& a, const ILicenseValidatorPtr& b) {
                  return a->getPriority() < b->getPriority();
              });
    LOG4CXX_DEBUG(logger, "Validators sorted by priority");
}

auto ValidationChain::validate(const License& license) const -> bool
{
    return validateWithDetails(license).isValid();
}

auto ValidationChain::validateWithDetails(const License& license) const -> ValidationResult
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " Running validation chain with " << m_validators.size() << " validators");

    if (m_validators.empty())
    {
        LOG4CXX_WARN(logger, "Validation chain is empty");
        return ValidationResult::success("No validators configured");
    }

    for (const auto& validator : m_validators)
    {
        if (!validator)
        {
            continue;
        }

        LOG4CXX_DEBUG(logger, "Running validator: " << validator->getName());

        auto result = validator->validateWithDetails(license);
        if (!result.isValid())
        {
            LOG4CXX_DEBUG(logger, "Validation chain failed at: " << validator->getName()
                                                                 << " - " << result.message);
            return result;
        }

        LOG4CXX_DEBUG(logger, "Validator passed: " << validator->getName());
    }

    LOG4CXX_DEBUG(logger, "Validation chain completed successfully");
    return ValidationResult::success("All validators passed");
}

auto ValidationChain::validateAll(const License& license) const -> std::vector<ValidationResult>
{
    std::vector<ValidationResult> results;
    results.reserve(m_validators.size());

    for (const auto& validator : m_validators)
    {
        if (validator)
        {
            results.push_back(validator->validateWithDetails(license));
        }
    }

    return results;
}

auto ValidationChain::getName() const noexcept -> std::string_view
{
    return "ValidationChain";
}

auto ValidationChain::getPriority() const noexcept -> int
{
    return 0;
}
