/*******************************************************************
 * @file   VersionConstraintTest.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Comprehensive tests for Version constraint system
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_VERSIONCONSTRAINTTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_VERSIONCONSTRAINTTEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/premisses/semver/VersionConstraint.hpp>
#include <com/github/doevelopper/premisses/logging/LogMacros.hpp>

namespace com::github::doevelopper::premisses::semver::test
{

/**
 * @brief Test fixture for VersionConstraint class
 *
 * Tests comprehensive constraint parsing and matching including:
 * - Exact version constraints
 * - Comparison operators (>, >=, <, <=, =)
 * - Caret ranges (^x.y.z)
 * - Tilde ranges (~x.y.z)
 * - Wildcard ranges (*, x.*, x.y.*)
 * - Compound constraints (AND, OR)
 * - Edge cases and error handling
 */
class VersionConstraintTest : public ::testing::Test
{
    LOG4CXX_DECLARE_STATIC_TEST_LOGGER

public:
    VersionConstraintTest();
    VersionConstraintTest(const VersionConstraintTest&) = delete;
    VersionConstraintTest(VersionConstraintTest&&) = delete;
    VersionConstraintTest& operator=(const VersionConstraintTest&) = delete;
    VersionConstraintTest& operator=(VersionConstraintTest&&) = delete;
    ~VersionConstraintTest() override;

    void SetUp() override;
    void TearDown() override;

protected:
    // Helper methods for common test patterns
    void assertSatisfies(const VersionConstraint& constraint, std::string_view versionString);
    void assertDoesNotSatisfy(const VersionConstraint& constraint, std::string_view versionString);

    void assertConstraintSatisfies(std::string_view constraintString,
                                   std::string_view versionString);
    void assertConstraintDoesNotSatisfy(std::string_view constraintString,
                                        std::string_view versionString);
};

}  // namespace com::github::doevelopper::premisses::semver::test

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_VERSIONCONSTRAINTTEST_HPP
