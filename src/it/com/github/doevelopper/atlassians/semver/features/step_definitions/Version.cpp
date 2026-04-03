
#include <stack>
#include <stdexcept>
#include <string>
#include <memory>
#include <variant>
#include <optional>
#include <cmath>

#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <com/github/doevelopper/premisses/semver/Version.hpp>
#include <com/github/doevelopper/premisses/semver/VersionConstraint.hpp>
#include <com/github/doevelopper/premisses/logging/LogMacros.hpp>

// using cucumber::ScenarioScope;
using namespace cucumber;
using namespace com::github::doevelopper::premisses::semver;

// ============================================================================
// Context Structure - Shared state between steps
// ============================================================================

struct VersionContext {
    std::optional<Version> currentVersion;
    std::optional<Version> firstVersion;
    std::optional<Version> secondVersion;
    std::optional<Version> resultVersion;
    std::vector<Version> versionList;
    bool parseSuccess{false};
    std::string errorMessage;
    std::string errorType;

    // For constraint testing
    std::optional<VersionConstraint> currentConstraint;
    bool constraintSatisfied{false};

    void reset() {
        currentVersion.reset();
        firstVersion.reset();
        secondVersion.reset();
        resultVersion.reset();
        versionList.clear();
        parseSuccess = false;
        errorMessage.clear();
        errorType.clear();
        currentConstraint.reset();
        constraintSatisfied = false;
    }
};

// // Global context (or use ScenarioScope in cucumber-cpp)
// static VersionContext ctx;

// // ============================================================================
// // Background Steps
// // ============================================================================

// GIVEN("^the SemVer 2\\.0\\.0 specification$") {
//     ScenarioScope<VersionContext> context;
//     context->reset();
//     LOG_DEBUG("Starting new scenario with SemVer 2.0.0 specification");
// }

// // ============================================================================
// // Version Parsing Steps
// // ============================================================================

// WHEN("^I parse the version string \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string versionStr = REGEX_PARAM(std::string, 1);

//     LOG_DEBUG("Parsing version string: " << versionStr);

//     try {
//         context->currentVersion = Version::parse(versionStr);
//         context->parseSuccess = context->currentVersion.has_value();

//         if (!context->parseSuccess) {
//             LOG_WARNING("Version parsing returned nullopt for: " << versionStr);
//         }
//     } catch (const std::exception& e) {
//         context->parseSuccess = false;
//         context->errorMessage = e.what();
//         LOG_ERROR("Exception during parsing: " << e.what());
//     }
// }

// THEN("^the parsing should succeed$") {
//     ScenarioScope<VersionContext> context;
//     EXPECT_TRUE(context->parseSuccess)
//         << "Expected parsing to succeed but it failed";
//     EXPECT_TRUE(context->currentVersion.has_value())
//         << "Expected version to have a value";
// }

// THEN("^the parsing should fail$") {
//     ScenarioScope<VersionContext> context;
//     EXPECT_FALSE(context->parseSuccess)
//         << "Expected parsing to fail but it succeeded";
//     EXPECT_FALSE(context->currentVersion.has_value())
//         << "Expected no version value";
// }

// THEN("^the major version should be (\\d+)$") {
//     ScenarioScope<VersionContext> context;
//     const int expectedMajor = REGEX_PARAM(int, 1);

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version available to check major version";
//     EXPECT_EQ(context->currentVersion->major(), expectedMajor)
//         << "Major version mismatch";
// }

// THEN("^the minor version should be (\\d+)$") {
//     ScenarioScope<VersionContext> context;
//     const int expectedMinor = REGEX_PARAM(int, 1);

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version available to check minor version";
//     EXPECT_EQ(context->currentVersion->minor(), expectedMinor)
//         << "Minor version mismatch";
// }

// THEN("^the patch version should be (\\d+)$") {
//     ScenarioScope<VersionContext> context;
//     const int expectedPatch = REGEX_PARAM(int, 1);

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version available to check patch version";
//     EXPECT_EQ(context->currentVersion->patch(), expectedPatch)
//         << "Patch version mismatch";
// }

// THEN("^the prerelease identifier should be \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string expectedPrerelease = REGEX_PARAM(std::string, 1);

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version available to check prerelease";
//     EXPECT_EQ(context->currentVersion->prerelease(), expectedPrerelease)
//         << "Prerelease identifier mismatch";
// }

// THEN("^there should be no prerelease identifier$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version available";
//     EXPECT_TRUE(context->currentVersion->prerelease().empty())
//         << "Expected no prerelease identifier";
//     EXPECT_FALSE(context->currentVersion->hasPrerelease())
//         << "hasPrerelease() should return false";
// }

// THEN("^the build metadata should be \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string expectedBuild = REGEX_PARAM(std::string, 1);

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version available to check build metadata";
//     EXPECT_EQ(context->currentVersion->buildMetadata(), expectedBuild)
//         << "Build metadata mismatch";
// }

// THEN("^there should be no build metadata$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version available";
//     EXPECT_TRUE(context->currentVersion->buildMetadata().empty())
//         << "Expected no build metadata";
//     EXPECT_FALSE(context->currentVersion->hasBuildMetadata())
//         << "hasBuildMetadata() should return false";
// }

// THEN("^the error should indicate \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string expectedError = REGEX_PARAM(std::string, 1);

//     // Store error type for validation
//     context->errorType = expectedError;

//     // This is informational - actual error checking depends on implementation
//     LOG_INFO("Expected error type: " << expectedError);
// }

// THEN("^the version should be marked as initial development$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version available";
//     EXPECT_TRUE(context->currentVersion->isInitialDevelopment())
//         << "Version should be marked as initial development (0.y.z)";
// }

// THEN("^the version should be marked as stable$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version available";
//     EXPECT_TRUE(context->currentVersion->isStable())
//         << "Version should be marked as stable (>= 1.0.0)";
// }

// // ============================================================================
// // Version Comparison Steps
// // ============================================================================

// GIVEN("^a version \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string versionStr = REGEX_PARAM(std::string, 1);

//     auto version = Version::parse(versionStr);
//     ASSERT_TRUE(version.has_value())
//         << "Failed to parse version: " << versionStr;

//     if (!context->firstVersion.has_value()) {
//         context->firstVersion = version;
//         LOG_DEBUG("Set first version: " << versionStr);
//     } else {
//         context->secondVersion = version;
//         LOG_DEBUG("Set second version: " << versionStr);
//     }
// }

// WHEN("^I compare the versions$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->firstVersion.has_value())
//         << "First version not set";
//     ASSERT_TRUE(context->secondVersion.has_value())
//         << "Second version not set";

//     LOG_DEBUG("Comparing " << context->firstVersion->toString()
//               << " with " << context->secondVersion->toString());
// }

// THEN("^\"([^\"]*)\" should be less than \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string v1Str = REGEX_PARAM(std::string, 1);
//     const std::string v2Str = REGEX_PARAM(std::string, 2);

//     auto v1 = Version::parse(v1Str);
//     auto v2 = Version::parse(v2Str);

//     ASSERT_TRUE(v1.has_value() && v2.has_value())
//         << "Failed to parse versions for comparison";

//     EXPECT_TRUE(*v1 < *v2)
//         << v1Str << " should be less than " << v2Str;
//     EXPECT_FALSE(*v1 >= *v2)
//         << v1Str << " should not be greater or equal to " << v2Str;
// }

// THEN("^the versions should be equal$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->firstVersion.has_value())
//         << "First version not set";
//     ASSERT_TRUE(context->secondVersion.has_value())
//         << "Second version not set";

//     EXPECT_TRUE(*context->firstVersion == *context->secondVersion)
//         << context->firstVersion->toString() << " should equal "
//         << context->secondVersion->toString();
//     EXPECT_FALSE(*context->firstVersion < *context->secondVersion)
//         << "Versions should not have ordering";
//     EXPECT_FALSE(*context->firstVersion > *context->secondVersion)
//         << "Versions should not have ordering";
// }

// GIVEN("^the following versions in order:$") {
//     ScenarioScope<VersionContext> context;

//     // This step expects a table with "version" column
//     // cucumber-cpp will call this for the table header
//     context->versionList.clear();
// }

// // Table row handler for version list
// GIVEN("^the following versions in order:$", "version") {
//     ScenarioScope<VersionContext> context;
//     const std::string versionStr = REGEX_PARAM(std::string, 1);

//     auto version = Version::parse(versionStr);
//     ASSERT_TRUE(version.has_value())
//         << "Failed to parse version in table: " << versionStr;

//     context->versionList.push_back(*version);
//     LOG_DEBUG("Added version to list: " << versionStr);
// }

// THEN("^each version should be less than the next$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_GE(context->versionList.size(), 2u)
//         << "Need at least 2 versions to compare";

//     for (size_t i = 0; i < context->versionList.size() - 1; ++i) {
//         const auto& current = context->versionList[i];
//         const auto& next = context->versionList[i + 1];

//         EXPECT_TRUE(current < next)
//             << "Version " << current.toString()
//             << " should be less than " << next.toString()
//             << " at position " << i;
//     }

//     LOG_INFO("Verified precedence chain of " << context->versionList.size()
//              << " versions");
// }

// // ============================================================================
// // Version Constraint Steps
// // ============================================================================

// GIVEN("^a constraint \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string constraintStr = REGEX_PARAM(std::string, 1);

//     context->currentConstraint = VersionConstraint::parse(constraintStr);
//     ASSERT_TRUE(context->currentConstraint.has_value())
//         << "Failed to parse constraint: " << constraintStr;

//     LOG_DEBUG("Set constraint: " << constraintStr);
// }

// WHEN("^I check if version \"([^\"]*)\" satisfies the constraint$") {
//     ScenarioScope<VersionContext> context;
//     const std::string versionStr = REGEX_PARAM(std::string, 1);

//     auto version = Version::parse(versionStr);
//     ASSERT_TRUE(version.has_value())
//         << "Failed to parse version: " << versionStr;

//     ASSERT_TRUE(context->currentConstraint.has_value())
//         << "No constraint set";

//     context->constraintSatisfied =
//         context->currentConstraint->satisfies(*version);

//     LOG_DEBUG("Constraint satisfaction for " << versionStr << ": "
//               << (context->constraintSatisfied ? "true" : "false"));
// }

// THEN("^the result should be (true|false)$") {
//     ScenarioScope<VersionContext> context;
//     const std::string expectedResult = REGEX_PARAM(std::string, 1);
//     const bool expected = (expectedResult == "true");

//     EXPECT_EQ(context->constraintSatisfied, expected)
//         << "Expected constraint satisfaction to be " << expectedResult;
// }

// // ============================================================================
// // Version Operation Steps
// // ============================================================================

// WHEN("^I increment the major version$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version set to increment";

//     context->resultVersion = context->currentVersion->nextMajor();
//     LOG_DEBUG("Incremented major: " << context->resultVersion->toString());
// }

// WHEN("^I increment the minor version$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version set to increment";

//     context->resultVersion = context->currentVersion->nextMinor();
//     LOG_DEBUG("Incremented minor: " << context->resultVersion->toString());
// }

// WHEN("^I increment the patch version$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version set to increment";

//     context->resultVersion = context->currentVersion->nextPatch();
//     LOG_DEBUG("Incremented patch: " << context->resultVersion->toString());
// }

// WHEN("^I add prerelease identifier \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string prerelease = REGEX_PARAM(std::string, 1);

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version set";

//     context->resultVersion = context->currentVersion->withPrerelease(prerelease);
//     LOG_DEBUG("Added prerelease: " << context->resultVersion->toString());
// }

// WHEN("^I add build metadata \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string buildMetadata = REGEX_PARAM(std::string, 1);

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version set";

//     context->resultVersion =
//         context->currentVersion->withBuildMetadata(buildMetadata);
//     LOG_DEBUG("Added build metadata: " << context->resultVersion->toString());
// }

// WHEN("^I remove the prerelease identifier$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version set";

//     context->resultVersion = context->currentVersion->withoutPrerelease();
//     LOG_DEBUG("Removed prerelease: " << context->resultVersion->toString());
// }

// WHEN("^I remove the build metadata$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version set";

//     context->resultVersion = context->currentVersion->withoutBuildMetadata();
//     LOG_DEBUG("Removed build metadata: " << context->resultVersion->toString());
// }

// THEN("^the resulting version should be \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string expectedVersion = REGEX_PARAM(std::string, 1);

//     ASSERT_TRUE(context->resultVersion.has_value())
//         << "No result version available";

//     EXPECT_EQ(context->resultVersion->toString(), expectedVersion)
//         << "Result version mismatch";
// }

// THEN("^the original version should remain \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string expectedVersion = REGEX_PARAM(std::string, 1);

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No current version available";

//     EXPECT_EQ(context->currentVersion->toString(), expectedVersion)
//         << "Original version was modified (should be immutable)";
// }

// THEN("^a new version object should be created$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No current version";
//     ASSERT_TRUE(context->resultVersion.has_value())
//         << "No result version";

//     // Check that they are different objects (by comparing addresses)
//     const void* currentAddr = &(*context->currentVersion);
//     const void* resultAddr = &(*context->resultVersion);

//     EXPECT_NE(currentAddr, resultAddr)
//         << "Version operations should create new objects (immutability)";
// }

// // ============================================================================
// // Version Query Steps
// // ============================================================================

// WHEN("^I query if it is a prerelease$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version available";

//     context->constraintSatisfied = context->currentVersion->isPrerelease();
// }

// WHEN("^I query if it is stable$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version available";

//     context->constraintSatisfied = context->currentVersion->isStable();
// }

// WHEN("^I query if it is initial development$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version available";

//     context->constraintSatisfied =
//         context->currentVersion->isInitialDevelopment();
// }

// // ============================================================================
// // String Conversion Steps
// // ============================================================================

// GIVEN("^a version created from \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string versionStr = REGEX_PARAM(std::string, 1);

//     context->currentVersion = Version::parse(versionStr);
//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "Failed to create version from: " << versionStr;
// }

// WHEN("^I convert it to string$") {
//     ScenarioScope<VersionContext> context;

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version to convert";

//     // String conversion is implicit via toString()
//     // The actual string will be checked in the THEN step
// }

// THEN("^the string should be \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string expected = REGEX_PARAM(std::string, 1);

//     ASSERT_TRUE(context->currentVersion.has_value())
//         << "No version available";

//     EXPECT_EQ(context->currentVersion->toString(), expected)
//         << "String representation mismatch";
// }

// // ============================================================================
// // Validation Steps
// // ============================================================================

// WHEN("^I validate major=(\\d+) minor=(\\d+) patch=(\\d+)$") {
//     ScenarioScope<VersionContext> context;
//     const int major = REGEX_PARAM(int, 1);
//     const int minor = REGEX_PARAM(int, 2);
//     const int patch = REGEX_PARAM(int, 3);

//     try {
//         Version v(major, minor, patch);
//         context->parseSuccess = true;
//         context->currentVersion = v;
//     } catch (const std::exception& e) {
//         context->parseSuccess = false;
//         context->errorMessage = e.what();
//         LOG_DEBUG("Validation failed: " << e.what());
//     }
// }

// THEN("^the validation should (succeed|fail)$") {
//     ScenarioScope<VersionContext> context;
//     const std::string expectedResult = REGEX_PARAM(std::string, 1);
//     const bool shouldSucceed = (expectedResult == "succeed");

//     EXPECT_EQ(context->parseSuccess, shouldSucceed)
//         << "Validation result mismatch";
// }

// WHEN("^I validate prerelease identifier \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string prerelease = REGEX_PARAM(std::string, 1);

//     try {
//         Version v(1, 0, 0, prerelease);
//         context->parseSuccess = true;
//     } catch (const std::exception&) {
//         context->parseSuccess = false;
//     }
// }

// WHEN("^I validate build metadata \"([^\"]*)\"$") {
//     ScenarioScope<VersionContext> context;
//     const std::string buildMetadata = REGEX_PARAM(std::string, 1);

//     try {
//         Version v(1, 0, 0, "", buildMetadata);
//         context->parseSuccess = true;
//     } catch (const std::exception&) {
//         context->parseSuccess = false;
//     }
// }

// WHEN("^I validate version string \"([^\"]*)\" against SemVer regex$") {
//     ScenarioScope<VersionContext> context;
//     const std::string versionStr = REGEX_PARAM(std::string, 1);

//     context->parseSuccess = Version::isValidVersionString(versionStr);
// }

// // THEN("^the version string validation should (succeed|fail)$") {
// //     ScenarioScope<VersionContext> context;
// //     const std::string expectedResult = REGEX_PARAM(std::string, 1);
// //     const bool shouldSucceed = (expectedResult == "succeed");

// //     EXPECT_EQ(context->parseSuccess, shouldSucceed)
// //         << "Version string validation result mismatch";
// // }
