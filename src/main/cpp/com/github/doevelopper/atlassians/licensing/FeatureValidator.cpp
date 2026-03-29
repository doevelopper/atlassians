/**
 * @file   FeatureValidator.cpp
 * @brief  Feature-based license validator implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/licensing/FeatureValidator.hpp>

#include <algorithm>
#include <sstream>

using namespace com::github::doevelopper::atlassians::licensing;

log4cxx::LoggerPtr FeatureValidator::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.licensing.FeatureValidator"));

FeatureValidator::FeatureValidator()
    : BaseLicenseValidator("FeatureValidator", 30)
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

FeatureValidator::~FeatureValidator()
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

auto FeatureValidator::addRequiredFeature(std::string_view feature) -> void
{
    m_requiredFeatures.insert(std::string(feature));
    LOG4CXX_DEBUG(logger, "Added required feature: " << feature);
}

auto FeatureValidator::removeRequiredFeature(std::string_view feature) -> void
{
    m_requiredFeatures.erase(std::string(feature));
}

auto FeatureValidator::clearRequiredFeatures() -> void
{
    m_requiredFeatures.clear();
}

auto FeatureValidator::isFeatureRequired(std::string_view feature) const -> bool
{
    return m_requiredFeatures.find(std::string(feature)) != m_requiredFeatures.end();
}

auto FeatureValidator::getRequiredFeatures() const -> std::vector<std::string>
{
    return std::vector<std::string>(m_requiredFeatures.begin(), m_requiredFeatures.end());
}

auto FeatureValidator::licenseHasFeature(const License& license, std::string_view feature) -> bool
{
    return license.hasFeature(feature);
}

auto FeatureValidator::doValidate(const License& license) const -> ValidationResult
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // If no required features, validation passes
    if (m_requiredFeatures.empty())
    {
        LOG4CXX_DEBUG(logger, "No required features configured");
        return ValidationResult::success("No required features to validate");
    }

    // Check each required feature
    std::vector<std::string> missingFeatures;

    for (const auto& feature : m_requiredFeatures)
    {
        if (!license.hasFeature(feature))
        {
            missingFeatures.push_back(feature);
        }
    }

    if (!missingFeatures.empty())
    {
        std::ostringstream message;
        message << "Missing required features: ";
        for (size_t i = 0; i < missingFeatures.size(); ++i)
        {
            message << missingFeatures[i];
            if (i < missingFeatures.size() - 1)
            {
                message << ", ";
            }
        }

        LOG4CXX_WARN(logger, message.str());

        return ValidationResult::failure(
            ValidationCode::FeatureNotAvailable,
            message.str(),
            "Upgrade your license to access these features");
    }

    LOG4CXX_DEBUG(logger, "All required features available");
    return ValidationResult::success("All required features are available");
}
