/**
 * @file   LicenseManagerTest.hpp
 * @brief  Unit tests for LicenseManager facade
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSEMANAGERTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSEMANAGERTEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/premisses/licensing/FileLicenseStorage.hpp>
#include <com/github/doevelopper/premisses/licensing/LicenseManager.hpp>
#include <com/github/doevelopper/premisses/logging/LogMacros.hpp>

namespace com::github::doevelopper::premisses::licensing::test
{

/**
 * @brief Test fixture for LicenseManager tests
 */
class LicenseManagerTest : public ::testing::Test
{
    LOG4CXX_DECLARE_STATIC_TEST_LOGGER

public:
    LicenseManagerTest();
    ~LicenseManagerTest() override;

    void SetUp() override;
    void TearDown() override;

protected:
    [[nodiscard]] auto createTestLicense() const -> License;

    std::shared_ptr<LicenseManager> m_manager;
    std::filesystem::path m_testDir;
};

/**
 * @brief Mock license observer for testing
 */
class MockLicenseObserver : public ILicenseObserver
{
public:
    void onLicenseEvent(LicenseEvent event, const License& license) override
    {
        lastEvent   = event;
        lastLicense = std::make_optional(License(license));
        eventCount++;
    }

    void onValidationResult(const License& license, const ValidationResult& result) override
    {
        lastResult       = result;
        validationCount++;
        (void)license; // Suppress unused warning
    }

    std::optional<LicenseEvent> lastEvent;
    std::optional<License> lastLicense;
    ValidationResult lastResult;
    int eventCount{0};
    int validationCount{0};
};

} // namespace com::github::doevelopper::premisses::licensing::test

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSEMANAGERTEST_HPP
