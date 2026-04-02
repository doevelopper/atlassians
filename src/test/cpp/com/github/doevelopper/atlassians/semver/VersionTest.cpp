/*******************************************************************
 * @file   VersionTest.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Comprehensive tests for Semantic Versioning 2.0.0 Version class
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include "VersionTest.hpp"

#include <array>
#include <vector>
#include <string>

using namespace com::github::doevelopper::atlassians::semver;
using namespace com::github::doevelopper::atlassians::semver::test;

namespace com::github::doevelopper::atlassians::semver::test
{

log4cxx::LoggerPtr VersionTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.atlassians.semver.test.VersionTest"));

VersionTest::VersionTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

VersionTest::~VersionTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void VersionTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void VersionTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void VersionTest::assertVersionEquals(const Version& v,
                                       std::uint32_t major,
                                       std::uint32_t minor,
                                       std::uint32_t patch,
                                       std::string_view preRelease,
                                       std::string_view buildMetadata)
{
    EXPECT_EQ(v.major(), major);
    EXPECT_EQ(v.minor(), minor);
    EXPECT_EQ(v.patch(), patch);
    EXPECT_EQ(v.preRelease(), preRelease);
    EXPECT_EQ(v.buildMetadata(), buildMetadata);
}

void VersionTest::assertVersionParsesTo(std::string_view versionString,
                                         std::uint32_t major,
                                         std::uint32_t minor,
                                         std::uint32_t patch,
                                         std::string_view preRelease,
                                         std::string_view buildMetadata)
{
    auto v = Version::parse(versionString);
    assertVersionEquals(v, major, minor, patch, preRelease, buildMetadata);
}

void VersionTest::assertParsingFails(std::string_view versionString)
{
    auto result = Version::tryParse(versionString);
    EXPECT_FALSE(result.has_value()) << "Expected parsing to fail for: " << versionString;
    EXPECT_THROW(Version::parse(versionString), VersionException);
}

void VersionTest::assertPrecedence(std::string_view lesser, std::string_view greater)
{
    auto vLesser = Version::parse(lesser);
    auto vGreater = Version::parse(greater);

    EXPECT_LT(vLesser, vGreater) << lesser << " should be less than " << greater;
    EXPECT_GT(vGreater, vLesser) << greater << " should be greater than " << lesser;
    EXPECT_LE(vLesser, vGreater);
    EXPECT_GE(vGreater, vLesser);
    EXPECT_NE(vLesser, vGreater);
}

// =============================================================================
// Basic Construction Tests
// =============================================================================

TEST_F(VersionTest, DefaultConstruction_CreatesVersion_0_0_0)
{
    Version v;
    assertVersionEquals(v, 0, 0, 0);
}

TEST_F(VersionTest, ConstructWithMajorMinorPatch)
{
    Version v(1, 2, 3);
    assertVersionEquals(v, 1, 2, 3);
}

TEST_F(VersionTest, ConstructWithPreRelease)
{
    Version v(1, 0, 0, "alpha");
    assertVersionEquals(v, 1, 0, 0, "alpha");
}

TEST_F(VersionTest, ConstructWithBuildMetadata)
{
    Version v(1, 0, 0, "", "build.123");
    assertVersionEquals(v, 1, 0, 0, "", "build.123");
}

TEST_F(VersionTest, ConstructWithPreReleaseAndBuildMetadata)
{
    Version v(1, 0, 0, "alpha.1", "build.456");
    assertVersionEquals(v, 1, 0, 0, "alpha.1", "build.456");
}

// =============================================================================
// Parsing Tests - Valid Versions
// =============================================================================

TEST_F(VersionTest, Parse_SimpleVersion)
{
    assertVersionParsesTo("1.2.3", 1, 2, 3);
}

TEST_F(VersionTest, Parse_Version_0_0_0)
{
    assertVersionParsesTo("0.0.0", 0, 0, 0);
}

TEST_F(VersionTest, Parse_LargeVersionNumbers)
{
    assertVersionParsesTo("999999999.888888888.777777777",
                          999999999, 888888888, 777777777);
}

TEST_F(VersionTest, Parse_WithPreRelease_Alpha)
{
    assertVersionParsesTo("1.0.0-alpha", 1, 0, 0, "alpha");
}

TEST_F(VersionTest, Parse_WithPreRelease_AlphaNumeric)
{
    assertVersionParsesTo("1.0.0-alpha.1", 1, 0, 0, "alpha.1");
}

TEST_F(VersionTest, Parse_WithPreRelease_MultipleIdentifiers)
{
    assertVersionParsesTo("1.0.0-alpha.beta.gamma", 1, 0, 0, "alpha.beta.gamma");
}

TEST_F(VersionTest, Parse_WithPreRelease_NumericOnly)
{
    assertVersionParsesTo("1.0.0-0.3.7", 1, 0, 0, "0.3.7");
}

TEST_F(VersionTest, Parse_WithPreRelease_ContainingHyphen)
{
    assertVersionParsesTo("1.0.0-x-y-z", 1, 0, 0, "x-y-z");
}

TEST_F(VersionTest, Parse_WithBuildMetadata)
{
    assertVersionParsesTo("1.0.0+20130313144700", 1, 0, 0, "", "20130313144700");
}

TEST_F(VersionTest, Parse_WithBuildMetadata_AlphaNumeric)
{
    assertVersionParsesTo("1.0.0+build.123", 1, 0, 0, "", "build.123");
}

TEST_F(VersionTest, Parse_WithPreReleaseAndBuildMetadata)
{
    assertVersionParsesTo("1.0.0-beta+exp.sha.5114f85",
                          1, 0, 0, "beta", "exp.sha.5114f85");
}

TEST_F(VersionTest, Parse_WithPreReleaseAndBuildMetadata_Complex)
{
    assertVersionParsesTo("1.0.0-alpha.1+build.2024.01.15",
                          1, 0, 0, "alpha.1", "build.2024.01.15");
}

TEST_F(VersionTest, Parse_VersionWithLeadingV_ShouldFail)
{
    // Per strict SemVer, 'v' prefix is not part of the spec
    assertParsingFails("v1.0.0");
}

// =============================================================================
// Parsing Tests - Invalid Versions
// =============================================================================

TEST_F(VersionTest, Parse_EmptyString_ShouldFail)
{
    assertParsingFails("");
}

TEST_F(VersionTest, Parse_OnlyMajor_ShouldFail)
{
    assertParsingFails("1");
}

TEST_F(VersionTest, Parse_OnlyMajorMinor_ShouldFail)
{
    assertParsingFails("1.2");
}

TEST_F(VersionTest, Parse_LeadingZeroInMajor_ShouldFail)
{
    assertParsingFails("01.2.3");
}

TEST_F(VersionTest, Parse_LeadingZeroInMinor_ShouldFail)
{
    assertParsingFails("1.02.3");
}

TEST_F(VersionTest, Parse_LeadingZeroInPatch_ShouldFail)
{
    assertParsingFails("1.2.03");
}

TEST_F(VersionTest, Parse_LeadingZeroInPreRelease_Numeric_ShouldFail)
{
    assertParsingFails("1.0.0-01");
}

TEST_F(VersionTest, Parse_NegativeNumbers_ShouldFail)
{
    assertParsingFails("-1.0.0");
    assertParsingFails("1.-2.0");
    assertParsingFails("1.0.-3");
}

TEST_F(VersionTest, Parse_NonNumericVersionParts_ShouldFail)
{
    assertParsingFails("a.b.c");
    assertParsingFails("1.2.c");
    assertParsingFails("1.b.3");
    assertParsingFails("a.2.3");
}

TEST_F(VersionTest, Parse_EmptyPreReleaseIdentifier_ShouldFail)
{
    assertParsingFails("1.0.0-");
    assertParsingFails("1.0.0-alpha.");
    assertParsingFails("1.0.0-.alpha");
    assertParsingFails("1.0.0-alpha..beta");
}

TEST_F(VersionTest, Parse_EmptyBuildMetadata_ShouldFail)
{
    assertParsingFails("1.0.0+");
    assertParsingFails("1.0.0-alpha+");
}

TEST_F(VersionTest, Parse_InvalidCharactersInPreRelease_ShouldFail)
{
    assertParsingFails("1.0.0-alpha!");
    assertParsingFails("1.0.0-alpha@beta");
    assertParsingFails("1.0.0-alpha#1");
}

TEST_F(VersionTest, Parse_WhitespaceInVersion_ShouldFail)
{
    assertParsingFails(" 1.0.0");
    assertParsingFails("1.0.0 ");
    assertParsingFails("1. 0.0");
    assertParsingFails("1.0.0-alpha beta");
}

TEST_F(VersionTest, Parse_FourParts_ShouldFail)
{
    assertParsingFails("1.2.3.4");
}

// =============================================================================
// Comparison Tests - Basic
// =============================================================================

TEST_F(VersionTest, Equality_SameVersion)
{
    auto v1 = Version::parse("1.2.3");
    auto v2 = Version::parse("1.2.3");
    EXPECT_EQ(v1, v2);
    EXPECT_FALSE(v1 != v2);
}

TEST_F(VersionTest, Equality_DifferentVersions)
{
    auto v1 = Version::parse("1.2.3");
    auto v2 = Version::parse("1.2.4");
    EXPECT_NE(v1, v2);
    EXPECT_FALSE(v1 == v2);
}

TEST_F(VersionTest, Comparison_MajorDifference)
{
    assertPrecedence("1.0.0", "2.0.0");
}

TEST_F(VersionTest, Comparison_MinorDifference)
{
    assertPrecedence("1.1.0", "1.2.0");
}

TEST_F(VersionTest, Comparison_PatchDifference)
{
    assertPrecedence("1.0.1", "1.0.2");
}

// =============================================================================
// Precedence Tests - Pre-release versions
// =============================================================================

TEST_F(VersionTest, Precedence_PreReleaseHasLowerPrecedenceThanNormal)
{
    // SemVer spec: When major, minor, and patch are equal, a pre-release
    // version has lower precedence than a normal version
    assertPrecedence("1.0.0-alpha", "1.0.0");
}

TEST_F(VersionTest, Precedence_AlphanumericComparison)
{
    // "identifiers with only digits are compared numerically and identifiers
    // with letters or hyphens are compared lexically in ASCII sort order"
    assertPrecedence("1.0.0-alpha", "1.0.0-alpha.1");
}

TEST_F(VersionTest, Precedence_NumericVsAlphanumericIdentifiers)
{
    // "Numeric identifiers always have lower precedence than non-numeric identifiers"
    assertPrecedence("1.0.0-1", "1.0.0-alpha");
}

TEST_F(VersionTest, Precedence_NumericIdentifiersComparedNumerically)
{
    // "Identifiers consisting of only digits are compared numerically"
    assertPrecedence("1.0.0-1", "1.0.0-2");
    assertPrecedence("1.0.0-2", "1.0.0-11");  // 2 < 11 numerically
}

TEST_F(VersionTest, Precedence_LongerSetHasHigherPrecedence)
{
    // "A larger set of pre-release fields has a higher precedence than a
    // smaller set, if all the preceding identifiers are equal"
    assertPrecedence("1.0.0-alpha", "1.0.0-alpha.1");
}

TEST_F(VersionTest, Precedence_SemVerSpecExample)
{
    // From the SemVer 2.0.0 specification:
    // 1.0.0-alpha < 1.0.0-alpha.1 < 1.0.0-alpha.beta < 1.0.0-beta <
    // 1.0.0-beta.2 < 1.0.0-beta.11 < 1.0.0-rc.1 < 1.0.0

    std::vector<std::string> orderedVersions = {
        "1.0.0-alpha",
        "1.0.0-alpha.1",
        "1.0.0-alpha.beta",
        "1.0.0-beta",
        "1.0.0-beta.2",
        "1.0.0-beta.11",
        "1.0.0-rc.1",
        "1.0.0"
    };

    for (std::size_t i = 0; i < orderedVersions.size() - 1; ++i)
    {
        for (std::size_t j = i + 1; j < orderedVersions.size(); ++j)
        {
            assertPrecedence(orderedVersions[i], orderedVersions[j]);
        }
    }
}

TEST_F(VersionTest, Precedence_BuildMetadataIgnored)
{
    // SemVer spec: "Build metadata MUST be ignored when determining
    // version precedence"
    auto v1 = Version::parse("1.0.0+build1");
    auto v2 = Version::parse("1.0.0+build2");

    EXPECT_EQ(v1.compare(v2), 0) << "Build metadata should be ignored in comparison";
    EXPECT_EQ(v1, v2) << "Versions with different build metadata should be equal";
}

TEST_F(VersionTest, Precedence_PreReleaseWithDifferentBuildMetadata)
{
    auto v1 = Version::parse("1.0.0-alpha+build1");
    auto v2 = Version::parse("1.0.0-alpha+build2");

    EXPECT_EQ(v1, v2) << "Same pre-release with different build metadata should be equal";
}

// =============================================================================
// Increment Tests
// =============================================================================

TEST_F(VersionTest, IncrementMajor_Basic)
{
    auto v = Version::parse("1.2.3");
    auto incremented = v.incrementMajor();
    assertVersionEquals(incremented, 2, 0, 0);
}

TEST_F(VersionTest, IncrementMajor_ResetsMinorAndPatch)
{
    auto v = Version::parse("1.5.9");
    auto incremented = v.incrementMajor();
    EXPECT_EQ(incremented.minor(), 0);
    EXPECT_EQ(incremented.patch(), 0);
}

TEST_F(VersionTest, IncrementMajor_ClearsPreRelease)
{
    auto v = Version::parse("1.0.0-alpha");
    auto incremented = v.incrementMajor();
    EXPECT_TRUE(incremented.preRelease().empty());
}

TEST_F(VersionTest, IncrementMajor_ClearsBuildMetadata)
{
    auto v = Version::parse("1.0.0+build.123");
    auto incremented = v.incrementMajor();
    EXPECT_TRUE(incremented.buildMetadata().empty());
}

TEST_F(VersionTest, IncrementMinor_Basic)
{
    auto v = Version::parse("1.2.3");
    auto incremented = v.incrementMinor();
    assertVersionEquals(incremented, 1, 3, 0);
}

TEST_F(VersionTest, IncrementMinor_ResetsPatch)
{
    auto v = Version::parse("1.2.5");
    auto incremented = v.incrementMinor();
    EXPECT_EQ(incremented.patch(), 0);
}

TEST_F(VersionTest, IncrementMinor_KeepsMajor)
{
    auto v = Version::parse("5.2.3");
    auto incremented = v.incrementMinor();
    EXPECT_EQ(incremented.major(), 5);
}

TEST_F(VersionTest, IncrementPatch_Basic)
{
    auto v = Version::parse("1.2.3");
    auto incremented = v.incrementPatch();
    assertVersionEquals(incremented, 1, 2, 4);
}

TEST_F(VersionTest, IncrementPatch_KeepsMajorAndMinor)
{
    auto v = Version::parse("3.4.5");
    auto incremented = v.incrementPatch();
    EXPECT_EQ(incremented.major(), 3);
    EXPECT_EQ(incremented.minor(), 4);
}

// =============================================================================
// Modifier Tests
// =============================================================================

TEST_F(VersionTest, WithPreRelease_AddsPreRelease)
{
    auto v = Version::parse("1.0.0");
    auto modified = v.withPreRelease("alpha");
    assertVersionEquals(modified, 1, 0, 0, "alpha");
}

TEST_F(VersionTest, WithPreRelease_ReplacesExistingPreRelease)
{
    auto v = Version::parse("1.0.0-alpha");
    auto modified = v.withPreRelease("beta");
    EXPECT_EQ(modified.preRelease(), "beta");
}

TEST_F(VersionTest, WithPreRelease_ClearsWithEmptyString)
{
    auto v = Version::parse("1.0.0-alpha");
    auto modified = v.withPreRelease("");
    EXPECT_TRUE(modified.preRelease().empty());
}

TEST_F(VersionTest, WithPreRelease_PreservesBuildMetadata)
{
    auto v = Version::parse("1.0.0+build.123");
    auto modified = v.withPreRelease("alpha");
    EXPECT_EQ(modified.buildMetadata(), "build.123");
}

TEST_F(VersionTest, WithBuildMetadata_AddsBuildMetadata)
{
    auto v = Version::parse("1.0.0");
    auto modified = v.withBuildMetadata("build.123");
    assertVersionEquals(modified, 1, 0, 0, "", "build.123");
}

TEST_F(VersionTest, WithBuildMetadata_ReplacesExisting)
{
    auto v = Version::parse("1.0.0+build1");
    auto modified = v.withBuildMetadata("build2");
    EXPECT_EQ(modified.buildMetadata(), "build2");
}

TEST_F(VersionTest, WithBuildMetadata_PreservesPreRelease)
{
    auto v = Version::parse("1.0.0-alpha");
    auto modified = v.withBuildMetadata("build.456");
    EXPECT_EQ(modified.preRelease(), "alpha");
}

// =============================================================================
// Query Method Tests
// =============================================================================

TEST_F(VersionTest, HasPreRelease_True)
{
    auto v = Version::parse("1.0.0-alpha");
    EXPECT_TRUE(v.hasPreRelease());
}

TEST_F(VersionTest, HasPreRelease_False)
{
    auto v = Version::parse("1.0.0");
    EXPECT_FALSE(v.hasPreRelease());
}

TEST_F(VersionTest, HasBuildMetadata_True)
{
    auto v = Version::parse("1.0.0+build");
    EXPECT_TRUE(v.hasBuildMetadata());
}

TEST_F(VersionTest, HasBuildMetadata_False)
{
    auto v = Version::parse("1.0.0");
    EXPECT_FALSE(v.hasBuildMetadata());
}

TEST_F(VersionTest, IsStable_TrueForMajorGreaterThanZero)
{
    EXPECT_TRUE(Version::parse("1.0.0").isStable());
    EXPECT_TRUE(Version::parse("2.5.3").isStable());
}

TEST_F(VersionTest, IsStable_FalseForMajorZero)
{
    EXPECT_FALSE(Version::parse("0.1.0").isStable());
    EXPECT_FALSE(Version::parse("0.99.99").isStable());
}

TEST_F(VersionTest, IsStable_FalseWithPreRelease)
{
    EXPECT_FALSE(Version::parse("1.0.0-alpha").isStable());
    EXPECT_FALSE(Version::parse("2.0.0-rc.1").isStable());
}

// =============================================================================
// Stability Tests
// =============================================================================

TEST_F(VersionTest, Stability_StableVersionsHaveMajorGreaterThanZero)
{
    auto v1 = Version::parse("1.2.3");
    auto v2 = Version::parse("1.5.0");
    EXPECT_TRUE(v1.isStable());
    EXPECT_TRUE(v2.isStable());
}

TEST_F(VersionTest, Stability_PreReleaseVersionsAreNotStable)
{
    auto v1 = Version::parse("1.0.0-alpha");
    auto v2 = Version::parse("2.0.0-beta");
    EXPECT_FALSE(v1.isStable());
    EXPECT_FALSE(v2.isStable());
}

TEST_F(VersionTest, Stability_ZeroMajorVersionsAreNotStable)
{
    auto v1 = Version::parse("0.1.0");
    auto v2 = Version::parse("0.2.0");
    EXPECT_FALSE(v1.isStable());
    EXPECT_FALSE(v2.isStable());
}

// =============================================================================
// String Conversion Tests
// =============================================================================

TEST_F(VersionTest, ToString_Basic)
{
    auto v = Version::parse("1.2.3");
    EXPECT_EQ(v.toString(), "1.2.3");
}

TEST_F(VersionTest, ToString_WithPreRelease)
{
    auto v = Version::parse("1.0.0-alpha.1");
    EXPECT_EQ(v.toString(), "1.0.0-alpha.1");
}

TEST_F(VersionTest, ToString_WithBuildMetadata)
{
    auto v = Version::parse("1.0.0+build.123");
    EXPECT_EQ(v.toString(), "1.0.0+build.123");
}

TEST_F(VersionTest, ToString_WithBoth)
{
    auto v = Version::parse("1.0.0-beta+build.456");
    EXPECT_EQ(v.toString(), "1.0.0-beta+build.456");
}

TEST_F(VersionTest, RoundTrip_Parse_ToString_Parse)
{
    std::vector<std::string> testCases = {
        "0.0.0",
        "1.2.3",
        "1.0.0-alpha",
        "1.0.0-alpha.1",
        "1.0.0-alpha.beta.gamma",
        "1.0.0+build",
        "1.0.0-rc.1+build.2024"
    };

    for (const auto& original : testCases)
    {
        auto v = Version::parse(original);
        auto str = v.toString();
        auto reparsed = Version::parse(str);
        EXPECT_EQ(v, reparsed) << "Round-trip failed for: " << original;
    }
}

// =============================================================================
// Static Validation Tests
// =============================================================================

TEST_F(VersionTest, IsValid_ValidVersions)
{
    EXPECT_TRUE(Version::isValid("1.0.0"));
    EXPECT_TRUE(Version::isValid("1.0.0-alpha"));
    EXPECT_TRUE(Version::isValid("1.0.0+build"));
    EXPECT_TRUE(Version::isValid("1.0.0-rc.1+build.123"));
}

TEST_F(VersionTest, IsValid_InvalidVersions)
{
    EXPECT_FALSE(Version::isValid(""));
    EXPECT_FALSE(Version::isValid("1.0"));
    EXPECT_FALSE(Version::isValid("1.0.0.0"));
    EXPECT_FALSE(Version::isValid("v1.0.0"));
    EXPECT_FALSE(Version::isValid("01.0.0"));
}

// =============================================================================
// Copy/Move Semantics Tests
// =============================================================================

TEST_F(VersionTest, CopyConstruction)
{
    auto v1 = Version::parse("1.2.3-alpha+build");
    Version v2(v1);
    EXPECT_EQ(v1, v2);
}

TEST_F(VersionTest, CopyAssignment)
{
    auto v1 = Version::parse("1.2.3-alpha+build");
    Version v2;
    v2 = v1;
    EXPECT_EQ(v1, v2);
}

TEST_F(VersionTest, MoveConstruction)
{
    auto v1 = Version::parse("1.2.3-alpha+build");
    auto str = v1.toString();
    Version v2(std::move(v1));
    EXPECT_EQ(v2.toString(), str);
}

TEST_F(VersionTest, MoveAssignment)
{
    auto v1 = Version::parse("1.2.3-alpha+build");
    auto str = v1.toString();
    Version v2;
    v2 = std::move(v1);
    EXPECT_EQ(v2.toString(), str);
}

// =============================================================================
// Stream Operator Tests
// =============================================================================

TEST_F(VersionTest, StreamOutputOperator)
{
    auto v = Version::parse("1.0.0-alpha+build");
    std::ostringstream oss;
    oss << v;
    EXPECT_EQ(oss.str(), "1.0.0-alpha+build");
}

// =============================================================================
// Edge Cases
// =============================================================================

TEST_F(VersionTest, ZeroVersion)
{
    Version v(0, 0, 0);
    EXPECT_EQ(v.toString(), "0.0.0");
}

TEST_F(VersionTest, Compare_VersionWithItself)
{
    auto v = Version::parse("1.2.3");
    EXPECT_EQ(v.compare(v), 0);
    EXPECT_EQ(v, v);
    EXPECT_LE(v, v);
    EXPECT_GE(v, v);
    EXPECT_FALSE(v < v);
    EXPECT_FALSE(v > v);
}

TEST_F(VersionTest, PreReleaseWithHyphens)
{
    auto v = Version::parse("1.0.0-x-y-z.0");
    EXPECT_EQ(v.preRelease(), "x-y-z.0");
}

TEST_F(VersionTest, BuildMetadataWithHyphens)
{
    auto v = Version::parse("1.0.0+build-2024-01-15");
    EXPECT_EQ(v.buildMetadata(), "build-2024-01-15");
}

TEST_F(VersionTest, ToCoreString_ExcludesPreReleaseAndBuild)
{
    auto v = Version::parse("1.2.3-alpha+build");
    EXPECT_EQ(v.toCoreString(), "1.2.3");
}

}  // namespace com::github::doevelopper::atlassians::semver::test
