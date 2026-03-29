/*******************************************************************
 * @file   VersionTest.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Comprehensive tests for Semantic Versioning 2.0.0 Version class
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_VERSIONTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_VERSIONTEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/premisses/semver/Version.hpp>
#include <com/github/doevelopper/premisses/logging/LogMacros.hpp>

namespace com::github::doevelopper::premisses::semver::test
{

/**
 * @brief Test fixture for Version class
 *
 * Tests comprehensive SemVer 2.0.0 compliance including:
 * - Parsing and validation
 * - Version comparison and precedence
 * - Pre-release handling
 * - Build metadata handling
 * - Version increment operations
 * - Edge cases and error handling
 */
class VersionTest : public ::testing::Test
{
    LOG4CXX_DECLARE_STATIC_TEST_LOGGER

public:
    VersionTest();
    VersionTest(const VersionTest&) = delete;
    VersionTest(VersionTest&&) = delete;
    VersionTest& operator=(const VersionTest&) = delete;
    VersionTest& operator=(VersionTest&&) = delete;
    ~VersionTest() override;

    void SetUp() override;
    void TearDown() override;

protected:
    // Helper methods for common test patterns
    void assertVersionEquals(const Version& v,
                            std::uint32_t major,
                            std::uint32_t minor,
                            std::uint32_t patch,
                            std::string_view preRelease = "",
                            std::string_view buildMetadata = "");

    void assertVersionParsesTo(std::string_view versionString,
                               std::uint32_t major,
                               std::uint32_t minor,
                               std::uint32_t patch,
                               std::string_view preRelease = "",
                               std::string_view buildMetadata = "");

    void assertParsingFails(std::string_view versionString);

    void assertPrecedence(std::string_view lesser, std::string_view greater);
};

}  // namespace com::github::doevelopper::premisses::semver::test

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_VERSIONTEST_HPP
