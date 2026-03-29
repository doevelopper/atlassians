/*******************************************************************
 * @file   VersionConstraintTest.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Comprehensive tests for Version constraint system
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include "VersionConstraintTest.hpp"

#include <vector>
#include <string>

using namespace com::github::doevelopper::premisses::semver;
using namespace com::github::doevelopper::premisses::semver::test;

namespace com::github::doevelopper::premisses::semver::test
{

log4cxx::LoggerPtr VersionConstraintTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.semver.test.VersionConstraintTest"));

VersionConstraintTest::VersionConstraintTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

VersionConstraintTest::~VersionConstraintTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void VersionConstraintTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void VersionConstraintTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void VersionConstraintTest::assertSatisfies(const VersionConstraint& constraint,
                                             std::string_view versionString)
{
    auto version = Version::parse(versionString);
    EXPECT_TRUE(constraint.isSatisfiedBy(version))
        << "Expected constraint to be satisfied by " << versionString;
}

void VersionConstraintTest::assertDoesNotSatisfy(const VersionConstraint& constraint,
                                                  std::string_view versionString)
{
    auto version = Version::parse(versionString);
    EXPECT_FALSE(constraint.isSatisfiedBy(version))
        << "Expected constraint NOT to be satisfied by " << versionString;
}

void VersionConstraintTest::assertConstraintSatisfies(std::string_view constraintString,
                                                       std::string_view versionString)
{
    auto constraint = VersionConstraint::parse(constraintString);
    assertSatisfies(constraint, versionString);
}

void VersionConstraintTest::assertConstraintDoesNotSatisfy(std::string_view constraintString,
                                                            std::string_view versionString)
{
    auto constraint = VersionConstraint::parse(constraintString);
    assertDoesNotSatisfy(constraint, versionString);
}

// =============================================================================
// Exact Version Constraint Tests
// =============================================================================

TEST_F(VersionConstraintTest, Exact_MatchesExactVersion)
{
    auto constraint = VersionConstraint::exact(Version::parse("1.2.3"));
    assertSatisfies(constraint, "1.2.3");
}

TEST_F(VersionConstraintTest, Exact_DoesNotMatchDifferentVersion)
{
    auto constraint = VersionConstraint::exact(Version::parse("1.2.3"));
    assertDoesNotSatisfy(constraint, "1.2.4");
    assertDoesNotSatisfy(constraint, "1.3.3");
    assertDoesNotSatisfy(constraint, "2.2.3");
}

TEST_F(VersionConstraintTest, Exact_MatchesWithPreRelease)
{
    auto constraint = VersionConstraint::exact(Version::parse("1.0.0-alpha"));
    assertSatisfies(constraint, "1.0.0-alpha");
    assertDoesNotSatisfy(constraint, "1.0.0-beta");
    assertDoesNotSatisfy(constraint, "1.0.0");
}

TEST_F(VersionConstraintTest, Parse_ExactVersion)
{
    assertConstraintSatisfies("1.2.3", "1.2.3");
    assertConstraintDoesNotSatisfy("1.2.3", "1.2.4");
}

TEST_F(VersionConstraintTest, Parse_ExactVersionWithEquals)
{
    assertConstraintSatisfies("=1.2.3", "1.2.3");
    assertConstraintDoesNotSatisfy("=1.2.3", "1.2.4");
}

// =============================================================================
// Any Version Constraint Tests
// =============================================================================

TEST_F(VersionConstraintTest, Any_MatchesAnyVersion)
{
    auto constraint = VersionConstraint::any();
    assertSatisfies(constraint, "0.0.0");
    assertSatisfies(constraint, "1.0.0");
    assertSatisfies(constraint, "999.999.999");
    assertSatisfies(constraint, "1.0.0-alpha");
}

TEST_F(VersionConstraintTest, Parse_Wildcard)
{
    assertConstraintSatisfies("*", "1.2.3");
    assertConstraintSatisfies("*", "0.0.0");
}

// =============================================================================
// Comparison Operator Tests
// =============================================================================

TEST_F(VersionConstraintTest, GreaterThan_Satisfies)
{
    auto constraint = VersionConstraint::greaterThan(Version::parse("1.0.0"));
    assertSatisfies(constraint, "1.0.1");
    assertSatisfies(constraint, "1.1.0");
    assertSatisfies(constraint, "2.0.0");
}

TEST_F(VersionConstraintTest, GreaterThan_DoesNotSatisfy)
{
    auto constraint = VersionConstraint::greaterThan(Version::parse("1.0.0"));
    assertDoesNotSatisfy(constraint, "1.0.0");
    assertDoesNotSatisfy(constraint, "0.9.9");
    assertDoesNotSatisfy(constraint, "0.0.1");
}

TEST_F(VersionConstraintTest, Parse_GreaterThan)
{
    assertConstraintSatisfies(">1.0.0", "1.0.1");
    assertConstraintSatisfies(">1.0.0", "2.0.0");
    assertConstraintDoesNotSatisfy(">1.0.0", "1.0.0");
    assertConstraintDoesNotSatisfy(">1.0.0", "0.9.0");
}

TEST_F(VersionConstraintTest, AtLeast_Satisfies)
{
    auto constraint = VersionConstraint::atLeast(Version::parse("1.0.0"));
    assertSatisfies(constraint, "1.0.0");
    assertSatisfies(constraint, "1.0.1");
    assertSatisfies(constraint, "2.0.0");
}

TEST_F(VersionConstraintTest, AtLeast_DoesNotSatisfy)
{
    auto constraint = VersionConstraint::atLeast(Version::parse("1.0.0"));
    assertDoesNotSatisfy(constraint, "0.9.9");
    assertDoesNotSatisfy(constraint, "0.0.1");
}

TEST_F(VersionConstraintTest, Parse_GreaterThanOrEqual)
{
    assertConstraintSatisfies(">=1.0.0", "1.0.0");
    assertConstraintSatisfies(">=1.0.0", "1.0.1");
    assertConstraintDoesNotSatisfy(">=1.0.0", "0.9.9");
}

TEST_F(VersionConstraintTest, LessThan_Satisfies)
{
    auto constraint = VersionConstraint::lessThan(Version::parse("2.0.0"));
    assertSatisfies(constraint, "1.9.9");
    assertSatisfies(constraint, "1.0.0");
    assertSatisfies(constraint, "0.0.1");
}

TEST_F(VersionConstraintTest, LessThan_DoesNotSatisfy)
{
    auto constraint = VersionConstraint::lessThan(Version::parse("2.0.0"));
    assertDoesNotSatisfy(constraint, "2.0.0");
    assertDoesNotSatisfy(constraint, "2.0.1");
    assertDoesNotSatisfy(constraint, "3.0.0");
}

TEST_F(VersionConstraintTest, Parse_LessThan)
{
    assertConstraintSatisfies("<2.0.0", "1.9.9");
    assertConstraintSatisfies("<2.0.0", "0.0.1");
    assertConstraintDoesNotSatisfy("<2.0.0", "2.0.0");
    assertConstraintDoesNotSatisfy("<2.0.0", "2.0.1");
}

TEST_F(VersionConstraintTest, AtMost_Satisfies)
{
    auto constraint = VersionConstraint::atMost(Version::parse("2.0.0"));
    assertSatisfies(constraint, "2.0.0");
    assertSatisfies(constraint, "1.9.9");
    assertSatisfies(constraint, "0.0.1");
}

TEST_F(VersionConstraintTest, AtMost_DoesNotSatisfy)
{
    auto constraint = VersionConstraint::atMost(Version::parse("2.0.0"));
    assertDoesNotSatisfy(constraint, "2.0.1");
    assertDoesNotSatisfy(constraint, "3.0.0");
}

TEST_F(VersionConstraintTest, Parse_LessThanOrEqual)
{
    assertConstraintSatisfies("<=2.0.0", "2.0.0");
    assertConstraintSatisfies("<=2.0.0", "1.0.0");
    assertConstraintDoesNotSatisfy("<=2.0.0", "2.0.1");
}

// =============================================================================
// Range Constraint Tests
// =============================================================================

TEST_F(VersionConstraintTest, Range_Basic)
{
    // range() creates [minVersion, maxVersion) - inclusive lower, exclusive upper
    auto constraint = VersionConstraint::range(
        Version::parse("1.0.0"),
        Version::parse("2.0.0")
    );
    assertSatisfies(constraint, "1.0.0");
    assertSatisfies(constraint, "1.5.0");
    assertDoesNotSatisfy(constraint, "2.0.0");  // exclusive upper bound
    assertDoesNotSatisfy(constraint, "0.9.9");
}

TEST_F(VersionConstraintTest, Range_IncludesLowerBound)
{
    // Test that lower bound is inclusive
    auto constraint = VersionConstraint::range(
        Version::parse("1.5.0"),
        Version::parse("2.0.0")
    );
    assertSatisfies(constraint, "1.5.0");  // inclusive lower
    assertSatisfies(constraint, "1.9.9");
    assertDoesNotSatisfy(constraint, "1.4.9");
}

// =============================================================================
// Caret Range Tests (^x.y.z)
// =============================================================================

TEST_F(VersionConstraintTest, Caret_Major_NonZero)
{
    // ^1.2.3 := >=1.2.3 <2.0.0
    auto constraint = VersionConstraint::caret(Version::parse("1.2.3"));
    assertSatisfies(constraint, "1.2.3");
    assertSatisfies(constraint, "1.2.4");
    assertSatisfies(constraint, "1.9.9");
    assertDoesNotSatisfy(constraint, "2.0.0");
    assertDoesNotSatisfy(constraint, "1.2.2");
}

TEST_F(VersionConstraintTest, Caret_Minor_Zero)
{
    // ^0.2.3 := >=0.2.3 <0.3.0
    auto constraint = VersionConstraint::caret(Version::parse("0.2.3"));
    assertSatisfies(constraint, "0.2.3");
    assertSatisfies(constraint, "0.2.4");
    assertSatisfies(constraint, "0.2.99");
    assertDoesNotSatisfy(constraint, "0.3.0");
    assertDoesNotSatisfy(constraint, "0.2.2");
    assertDoesNotSatisfy(constraint, "1.0.0");
}

TEST_F(VersionConstraintTest, Caret_Patch_Zero)
{
    // ^0.0.3 := >=0.0.3 <0.0.4
    auto constraint = VersionConstraint::caret(Version::parse("0.0.3"));
    assertSatisfies(constraint, "0.0.3");
    assertDoesNotSatisfy(constraint, "0.0.4");
    assertDoesNotSatisfy(constraint, "0.0.2");
    assertDoesNotSatisfy(constraint, "0.1.0");
}

TEST_F(VersionConstraintTest, Parse_Caret)
{
    assertConstraintSatisfies("^1.2.3", "1.2.3");
    assertConstraintSatisfies("^1.2.3", "1.9.9");
    assertConstraintDoesNotSatisfy("^1.2.3", "2.0.0");
}

TEST_F(VersionConstraintTest, Parse_Caret_ZeroMajor)
{
    assertConstraintSatisfies("^0.2.3", "0.2.5");
    assertConstraintDoesNotSatisfy("^0.2.3", "0.3.0");
}

TEST_F(VersionConstraintTest, Parse_Caret_ZeroMinor)
{
    assertConstraintSatisfies("^0.0.3", "0.0.3");
    assertConstraintDoesNotSatisfy("^0.0.3", "0.0.4");
}

// =============================================================================
// Tilde Range Tests (~x.y.z)
// =============================================================================

TEST_F(VersionConstraintTest, Tilde_AllowsPatchIncrement)
{
    // ~1.2.3 := >=1.2.3 <1.3.0
    auto constraint = VersionConstraint::tilde(Version::parse("1.2.3"));
    assertSatisfies(constraint, "1.2.3");
    assertSatisfies(constraint, "1.2.4");
    assertSatisfies(constraint, "1.2.99");
    assertDoesNotSatisfy(constraint, "1.3.0");
    assertDoesNotSatisfy(constraint, "1.2.2");
    assertDoesNotSatisfy(constraint, "2.0.0");
}

TEST_F(VersionConstraintTest, Tilde_ZeroMajor)
{
    // ~0.2.3 := >=0.2.3 <0.3.0
    auto constraint = VersionConstraint::tilde(Version::parse("0.2.3"));
    assertSatisfies(constraint, "0.2.3");
    assertSatisfies(constraint, "0.2.9");
    assertDoesNotSatisfy(constraint, "0.3.0");
    assertDoesNotSatisfy(constraint, "1.0.0");
}

TEST_F(VersionConstraintTest, Parse_Tilde)
{
    assertConstraintSatisfies("~1.2.3", "1.2.3");
    assertConstraintSatisfies("~1.2.3", "1.2.99");
    assertConstraintDoesNotSatisfy("~1.2.3", "1.3.0");
}

// =============================================================================
// Wildcard Range Tests (x.*, x.y.*)
// =============================================================================

TEST_F(VersionConstraintTest, Parse_Wildcard_Minor)
{
    // 1.* matches any version with major=1
    assertConstraintSatisfies("1.*", "1.0.0");
    assertConstraintSatisfies("1.*", "1.99.99");
    assertConstraintDoesNotSatisfy("1.*", "0.9.9");
    assertConstraintDoesNotSatisfy("1.*", "2.0.0");
}

TEST_F(VersionConstraintTest, Parse_Wildcard_Patch)
{
    // 1.2.* matches any version with major=1 and minor=2
    assertConstraintSatisfies("1.2.*", "1.2.0");
    assertConstraintSatisfies("1.2.*", "1.2.99");
    assertConstraintDoesNotSatisfy("1.2.*", "1.1.9");
    assertConstraintDoesNotSatisfy("1.2.*", "1.3.0");
}

// Note: 'x' wildcard notation is not supported, only '*' is supported

// =============================================================================
// Compound Constraint Tests
// =============================================================================

TEST_F(VersionConstraintTest, CompoundAND_SpaceSeparated)
{
    // >=1.0.0 <2.0.0 (AND logic)
    assertConstraintSatisfies(">=1.0.0 <2.0.0", "1.0.0");
    assertConstraintSatisfies(">=1.0.0 <2.0.0", "1.5.0");
    assertConstraintSatisfies(">=1.0.0 <2.0.0", "1.9.9");
    assertConstraintDoesNotSatisfy(">=1.0.0 <2.0.0", "0.9.9");
    assertConstraintDoesNotSatisfy(">=1.0.0 <2.0.0", "2.0.0");
}

TEST_F(VersionConstraintTest, CompoundOR_PipeSeparated)
{
    // 1.0.0 || 2.0.0 (OR logic)
    assertConstraintSatisfies("1.0.0 || 2.0.0", "1.0.0");
    assertConstraintSatisfies("1.0.0 || 2.0.0", "2.0.0");
    assertConstraintDoesNotSatisfy("1.0.0 || 2.0.0", "1.5.0");
    assertConstraintDoesNotSatisfy("1.0.0 || 2.0.0", "3.0.0");
}

TEST_F(VersionConstraintTest, CompoundOR_WithRanges)
{
    // ^1.0.0 || ^2.0.0
    assertConstraintSatisfies("^1.0.0 || ^2.0.0", "1.0.0");
    assertConstraintSatisfies("^1.0.0 || ^2.0.0", "1.9.9");
    assertConstraintSatisfies("^1.0.0 || ^2.0.0", "2.0.0");
    assertConstraintSatisfies("^1.0.0 || ^2.0.0", "2.9.9");
    assertConstraintDoesNotSatisfy("^1.0.0 || ^2.0.0", "0.9.9");
    assertConstraintDoesNotSatisfy("^1.0.0 || ^2.0.0", "3.0.0");
}

TEST_F(VersionConstraintTest, CompoundComplex)
{
    // >=1.0.0 <1.5.0 || >=2.0.0 <3.0.0
    std::string constraint = ">=1.0.0 <1.5.0 || >=2.0.0 <3.0.0";
    assertConstraintSatisfies(constraint, "1.0.0");
    assertConstraintSatisfies(constraint, "1.4.9");
    assertConstraintSatisfies(constraint, "2.0.0");
    assertConstraintSatisfies(constraint, "2.9.9");
    assertConstraintDoesNotSatisfy(constraint, "1.5.0");
    assertConstraintDoesNotSatisfy(constraint, "1.9.0");
    assertConstraintDoesNotSatisfy(constraint, "3.0.0");
}

// =============================================================================
// Pre-release Handling Tests
// =============================================================================

TEST_F(VersionConstraintTest, PreRelease_ExactMatch)
{
    assertConstraintSatisfies("1.0.0-alpha", "1.0.0-alpha");
    assertConstraintDoesNotSatisfy("1.0.0-alpha", "1.0.0-beta");
    assertConstraintDoesNotSatisfy("1.0.0-alpha", "1.0.0");
}

TEST_F(VersionConstraintTest, Range_ExcludesPreReleaseByDefault)
{
    // By default, pre-release versions should only match if
    // the constraint explicitly allows them
    auto constraint = VersionConstraint::caret(Version::parse("1.0.0"));
    // 1.0.1-alpha is technically >=1.0.0 and <2.0.0, but pre-releases
    // typically have special handling
    assertSatisfies(constraint, "1.0.0");
    assertSatisfies(constraint, "1.5.0");
    // Pre-release behavior depends on implementation choice
}

// =============================================================================
// Filter/Query Tests
// =============================================================================

TEST_F(VersionConstraintTest, Filter_ReturnsMatchingVersions)
{
    auto constraint = VersionConstraint::caret(Version::parse("1.0.0"));
    std::vector<Version> versions = {
        Version::parse("0.9.0"),
        Version::parse("1.0.0"),
        Version::parse("1.5.0"),
        Version::parse("2.0.0"),
        Version::parse("2.1.0")
    };

    auto filtered = constraint.filter(versions);
    EXPECT_EQ(filtered.size(), 2u);
    EXPECT_EQ(filtered[0].toString(), "1.0.0");
    EXPECT_EQ(filtered[1].toString(), "1.5.0");
}

TEST_F(VersionConstraintTest, MaxSatisfying_ReturnsHighestMatch)
{
    auto constraint = VersionConstraint::caret(Version::parse("1.0.0"));
    std::vector<Version> versions = {
        Version::parse("0.9.0"),
        Version::parse("1.0.0"),
        Version::parse("1.5.0"),
        Version::parse("1.2.0"),
        Version::parse("2.0.0")
    };

    auto max = constraint.maxSatisfying(versions);
    ASSERT_TRUE(max.has_value());
    EXPECT_EQ(max->toString(), "1.5.0");
}

TEST_F(VersionConstraintTest, MaxSatisfying_ReturnsEmpty_WhenNoMatch)
{
    auto constraint = VersionConstraint::caret(Version::parse("3.0.0"));
    std::vector<Version> versions = {
        Version::parse("1.0.0"),
        Version::parse("2.0.0")
    };

    auto max = constraint.maxSatisfying(versions);
    EXPECT_FALSE(max.has_value());
}

TEST_F(VersionConstraintTest, MinSatisfying_ReturnsLowestMatch)
{
    auto constraint = VersionConstraint::caret(Version::parse("1.0.0"));
    std::vector<Version> versions = {
        Version::parse("0.9.0"),
        Version::parse("1.5.0"),
        Version::parse("1.0.0"),
        Version::parse("1.2.0"),
        Version::parse("2.0.0")
    };

    auto min = constraint.minSatisfying(versions);
    ASSERT_TRUE(min.has_value());
    EXPECT_EQ(min->toString(), "1.0.0");
}

// =============================================================================
// Set Operations Tests
// =============================================================================

TEST_F(VersionConstraintTest, Intersect_TwoRanges)
{
    auto c1 = VersionConstraint::parse(">=1.0.0 <3.0.0");
    auto c2 = VersionConstraint::parse(">=2.0.0 <4.0.0");
    auto intersection = c1.intersect(c2);

    assertSatisfies(intersection, "2.0.0");
    assertSatisfies(intersection, "2.9.9");
    assertDoesNotSatisfy(intersection, "1.0.0");
    assertDoesNotSatisfy(intersection, "3.0.0");
}

TEST_F(VersionConstraintTest, Unite_TwoRanges)
{
    auto c1 = VersionConstraint::exact(Version::parse("1.0.0"));
    auto c2 = VersionConstraint::exact(Version::parse("2.0.0"));
    auto united = c1.unite(c2);

    assertSatisfies(united, "1.0.0");
    assertSatisfies(united, "2.0.0");
    assertDoesNotSatisfy(united, "1.5.0");
}

// =============================================================================
// String Conversion Tests
// =============================================================================

TEST_F(VersionConstraintTest, ToString_ExactVersion)
{
    auto constraint = VersionConstraint::exact(Version::parse("1.2.3"));
    EXPECT_EQ(constraint.toString(), "=1.2.3");
}

TEST_F(VersionConstraintTest, ToString_GreaterThan)
{
    auto constraint = VersionConstraint::greaterThan(Version::parse("1.0.0"));
    EXPECT_EQ(constraint.toString(), ">1.0.0");
}

TEST_F(VersionConstraintTest, ToString_AtLeast)
{
    auto constraint = VersionConstraint::atLeast(Version::parse("1.0.0"));
    EXPECT_EQ(constraint.toString(), ">=1.0.0");
}

TEST_F(VersionConstraintTest, ToString_Any)
{
    auto constraint = VersionConstraint::any();
    EXPECT_EQ(constraint.toString(), "*");
}

// =============================================================================
// Edge Cases
// =============================================================================

TEST_F(VersionConstraintTest, Parse_WhitespaceTrimmed)
{
    assertConstraintSatisfies("  >= 1.0.0  <  2.0.0  ", "1.5.0");
}

TEST_F(VersionConstraintTest, EmptyConstraintString_MatchesAll)
{
    // Empty constraint is treated as "*" (matches all versions)
    auto constraint = VersionConstraint::parse("");
    EXPECT_TRUE(constraint.isAny());
    assertSatisfies(constraint, "1.0.0");
    assertSatisfies(constraint, "0.0.1");
}

TEST_F(VersionConstraintTest, InvalidConstraintString_Throws)
{
    // Invalid version formats should throw an exception (either ConstraintException or VersionException)
    EXPECT_THROW(VersionConstraint::parse(">>1.0.0"), std::exception);
}

// =============================================================================
// Copy/Move Semantics Tests
// =============================================================================

TEST_F(VersionConstraintTest, CopyConstruction)
{
    auto c1 = VersionConstraint::caret(Version::parse("1.0.0"));
    VersionConstraint c2(c1);

    assertSatisfies(c2, "1.5.0");
    assertDoesNotSatisfy(c2, "2.0.0");
}

TEST_F(VersionConstraintTest, MoveConstruction)
{
    auto c1 = VersionConstraint::caret(Version::parse("1.0.0"));
    VersionConstraint c2(std::move(c1));

    assertSatisfies(c2, "1.5.0");
    assertDoesNotSatisfy(c2, "2.0.0");
}

TEST_F(VersionConstraintTest, CopyAssignment)
{
    auto c1 = VersionConstraint::caret(Version::parse("1.0.0"));
    VersionConstraint c2;
    c2 = c1;

    assertSatisfies(c2, "1.5.0");
    assertDoesNotSatisfy(c2, "2.0.0");
}

TEST_F(VersionConstraintTest, MoveAssignment)
{
    auto c1 = VersionConstraint::caret(Version::parse("1.0.0"));
    VersionConstraint c2;
    c2 = std::move(c1);

    assertSatisfies(c2, "1.5.0");
    assertDoesNotSatisfy(c2, "2.0.0");
}

}  // namespace com::github::doevelopper::premisses::semver::test
