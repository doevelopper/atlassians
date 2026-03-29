/**
 * @file   LicenseValidatorTest.hpp
 * @brief  Unit tests for license validators
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSEVALIDATORTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSEVALIDATORTEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/premisses/licensing/ExpirationValidator.hpp>
#include <com/github/doevelopper/premisses/licensing/FeatureValidator.hpp>
#include <com/github/doevelopper/premisses/licensing/HardwareIdValidator.hpp>
#include <com/github/doevelopper/premisses/licensing/License.hpp>
#include <com/github/doevelopper/premisses/licensing/ValidationChain.hpp>
#include <com/github/doevelopper/premisses/logging/LogMacros.hpp>

namespace com::github::doevelopper::premisses::licensing::test
{

/**
 * @brief Test fixture for License Validator tests
 */
class LicenseValidatorTest : public ::testing::Test
{
    LOG4CXX_DECLARE_STATIC_TEST_LOGGER

public:
    LicenseValidatorTest();
    ~LicenseValidatorTest() override;

    void SetUp() override;
    void TearDown() override;

protected:
    [[nodiscard]] auto createValidLicense() const -> License;
    [[nodiscard]] auto createExpiredLicense() const -> License;
    [[nodiscard]] auto createHardwareBoundLicense(std::string_view hardwareId) const -> License;
    [[nodiscard]] auto createLicenseWithFeatures(const std::vector<std::string>& features) const -> License;
};

} // namespace com::github::doevelopper::premisses::licensing::test

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSEVALIDATORTEST_HPP
