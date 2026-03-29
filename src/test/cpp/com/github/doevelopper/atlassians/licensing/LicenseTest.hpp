/**
 * @file   LicenseTest.hpp
 * @brief  Unit tests for License entity
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSETEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSETEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/premisses/licensing/License.hpp>
#include <com/github/doevelopper/premisses/licensing/LicenseTypes.hpp>
#include <com/github/doevelopper/premisses/logging/LogMacros.hpp>

namespace com::github::doevelopper::premisses::licensing::test
{

/**
 * @brief Test fixture for License entity tests
 */
class LicenseTest : public ::testing::Test
{
    LOG4CXX_DECLARE_STATIC_TEST_LOGGER

public:
    LicenseTest();
    LicenseTest(const LicenseTest&)            = delete;
    LicenseTest(LicenseTest&&)                 = delete;
    LicenseTest& operator=(const LicenseTest&) = delete;
    LicenseTest& operator=(LicenseTest&&)      = delete;
    ~LicenseTest() override;

    void SetUp() override;
    void TearDown() override;

protected:
    /**
     * @brief Create a valid test license
     */
    [[nodiscard]] auto createValidLicense() const -> License;

    /**
     * @brief Create an expired test license
     */
    [[nodiscard]] auto createExpiredLicense() const -> License;

    /**
     * @brief Create a license with features
     */
    [[nodiscard]] auto createLicenseWithFeatures(const std::vector<std::string>& features) const -> License;
};

} // namespace com::github::doevelopper::premisses::licensing::test

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSETEST_HPP
