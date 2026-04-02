/**
 * @file   LicenseStorageTest.hpp
 * @brief  Unit tests for License Storage implementations
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSESTORAGETEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSESTORAGETEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/atlassians/licensing/FileLicenseStorage.hpp>
#include <com/github/doevelopper/atlassians/licensing/License.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::licensing::test
{

/**
 * @brief Test fixture for License Storage tests
 */
class LicenseStorageTest : public ::testing::Test
{
    LOG4CXX_DECLARE_STATIC_TEST_LOGGER

public:
    LicenseStorageTest();
    ~LicenseStorageTest() override;

    void SetUp() override;
    void TearDown() override;

protected:
    [[nodiscard]] auto createTestLicense(std::string_view key) const -> License;

    std::filesystem::path m_testDir;
    std::shared_ptr<FileLicenseStorage> m_storage;
};

} // namespace com::github::doevelopper::atlassians::licensing::test

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_LICENSESTORAGETEST_HPP
