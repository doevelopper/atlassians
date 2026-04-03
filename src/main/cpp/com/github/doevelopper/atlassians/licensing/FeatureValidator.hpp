/**
 * @file   FeatureValidator.hpp
 * @brief  Feature-based license validator
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_FEATUREVALIDATOR_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_FEATUREVALIDATOR_HPP

#include <set>

#include <com/github/doevelopper/atlassians/licensing/BaseLicenseValidator.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

/**
 * @brief Validator for feature availability
 *
 * Checks if required features are available in the license.
 */
class FeatureValidator : public BaseLicenseValidator
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    /**
     * @brief Constructor
     */
    FeatureValidator();

    ~FeatureValidator() override;

    /**
     * @brief Add a required feature
     */
    auto addRequiredFeature(std::string_view feature) -> void;

    /**
     * @brief Remove a required feature
     */
    auto removeRequiredFeature(std::string_view feature) -> void;

    /**
     * @brief Clear all required features
     */
    auto clearRequiredFeatures() -> void;

    /**
     * @brief Check if a specific feature is required
     */
    [[nodiscard]] auto isFeatureRequired(std::string_view feature) const -> bool;

    /**
     * @brief Get all required features
     */
    [[nodiscard]] auto getRequiredFeatures() const -> std::vector<std::string>;

    /**
     * @brief Check if a license has a specific feature
     */
    [[nodiscard]] static auto licenseHasFeature(const License& license, std::string_view feature) -> bool;

protected:
    [[nodiscard]] auto doValidate(const License& license) const -> ValidationResult override;

private:
    std::set<std::string> m_requiredFeatures;
};

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_FEATUREVALIDATOR_HPP
