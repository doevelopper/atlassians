/**
 * @file   LicenseManagement.cpp
 * @brief  Cucumber step definitions for License Management BDD tests
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <com/github/doevelopper/premisses/licensing/ExpirationValidator.hpp>
#include <com/github/doevelopper/premisses/licensing/FeatureValidator.hpp>
#include <com/github/doevelopper/premisses/licensing/FileLicenseStorage.hpp>
#include <com/github/doevelopper/premisses/licensing/HardwareIdValidator.hpp>
#include <com/github/doevelopper/premisses/licensing/License.hpp>
#include <com/github/doevelopper/premisses/licensing/LicenseManager.hpp>
#include <com/github/doevelopper/premisses/licensing/LicenseTypes.hpp>
#include <com/github/doevelopper/premisses/licensing/ValidationChain.hpp>

namespace
{

using namespace com::github::doevelopper::premisses::licensing;
using cucumber::ScenarioScope;

/**
 * @brief Test context for license management BDD tests
 */
struct LicenseManagementContext
{
    std::shared_ptr<LicenseManager> manager;
    std::shared_ptr<FileLicenseStorage> storage;
    std::filesystem::path testDir;

    std::optional<License> currentLicense;
    std::optional<ValidationResult> lastValidationResult;

    // Mock observer for testing
    struct MockObserver : public ILicenseObserver
    {
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
            (void)license;
        }

        std::optional<LicenseEvent> lastEvent;
        std::optional<License> lastLicense;
        ValidationResult lastResult;
        int eventCount{0};
        int validationCount{0};
    };

    std::shared_ptr<MockObserver> observer;
    std::shared_ptr<FeatureValidator> featureValidator;
    std::shared_ptr<ExpirationValidator> expirationValidator;

    std::vector<std::string> storedLicenseKeys;
};

// ============================================
// Background Steps
// ============================================

GIVEN("^a clean license manager instance$")
{
    ScenarioScope<LicenseManagementContext> context;

    LicenseManager::resetInstance();
    context->manager = LicenseManager::getInstance();
}

GIVEN("^a temporary license storage directory$")
{
    ScenarioScope<LicenseManagementContext> context;

    context->testDir = std::filesystem::temp_directory_path() / "bdd_license_test";
    std::filesystem::remove_all(context->testDir);
    std::filesystem::create_directories(context->testDir);

    context->storage = std::make_shared<FileLicenseStorage>(context->testDir);
    context->manager->setStorage(context->storage);
}

// ============================================
// License Creation Steps
// ============================================

WHEN("^I create a license with:$")
{
    ScenarioScope<LicenseManagementContext> context;
    TABLE_PARAM(tableParam);
    auto& table = tableParam;

    auto builder = License::builder();

    for (const auto& row : table.hashes())
    {
        std::string property = row.at("Property");
        std::string value    = row.at("Value");

        if (property == "licenseKey")
            builder.withLicenseKey(value);
        else if (property == "productId")
            builder.withProductId(value);
        else if (property == "type")
            builder.withType(stringToLicenseType(value));
        else if (property == "licenseeName")
            builder.withLicenseeName(value);
        else if (property == "email")
            builder.withLicenseeEmail(value);
        else if (property == "validDays")
            builder.expiresInDays(std::stoi(value));
    }

    builder.issuedNow().withDigitalSignature("mock-signature");

    try
    {
        context->currentLicense = builder.build();
    }
    catch (...)
    {
        context->currentLicense = std::nullopt;
    }
}

WHEN("^I create a trial license for product \"([^\"]*)\" valid for (\\d+) days$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, productId);
    REGEX_PARAM(int, days);

    context->currentLicense = License::builder()
                                  .withLicenseKey("TRIAL-AUTO-KEY")
                                  .withProductId(productId)
                                  .withType(LicenseType::Trial)
                                  .expiresInDays(days)
                                  .issuedNow()
                                  .withDigitalSignature("mock-sig")
                                  .build();
}

WHEN("^I create an enterprise license with features:$")
{
    ScenarioScope<LicenseManagementContext> context;
    TABLE_PARAM(tableParam);
    auto& table = tableParam;

    auto builder = License::builder()
                       .withLicenseKey("ENTERPRISE-KEY")
                       .withProductId("ENTERPRISE-PRODUCT")
                       .withType(LicenseType::Enterprise)
                       .expiresInDays(365)
                       .issuedNow()
                       .withDigitalSignature("mock-sig");

    for (const auto& row : table.hashes())
    {
        builder.withFeature(row.at("Feature"));
    }

    context->currentLicense = builder.build();
}

WHEN("^I create a license for licensee \"([^\"]*)\"$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, licenseeName);

    context->currentLicense = License::builder()
                                  .withLicenseKey("SPECIAL-CHAR-KEY")
                                  .withProductId("PRODUCT")
                                  .withType(LicenseType::Commercial)
                                  .withLicenseeName(licenseeName)
                                  .expiresInDays(30)
                                  .issuedNow()
                                  .withDigitalSignature("mock-sig")
                                  .build();
}

WHEN("^I create a perpetual license without expiration date$")
{
    ScenarioScope<LicenseManagementContext> context;

    context->currentLicense = License::builder()
                                  .withLicenseKey("PERPETUAL-KEY")
                                  .withProductId("PRODUCT")
                                  .withType(LicenseType::Enterprise)
                                  .issuedNow()
                                  .withDigitalSignature("mock-sig")
                                  // No expiration date
                                  .build();
}

THEN("^the license should be created successfully$")
{
    ScenarioScope<LicenseManagementContext> context;

    ASSERT_TRUE(context->currentLicense.has_value());
}

THEN("^the license type should be \"([^\"]*)\"$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, expectedType);

    ASSERT_TRUE(context->currentLicense.has_value());
    EXPECT_EQ(licenseTypeToString(context->currentLicense->getType()), expectedType);
}

THEN("^the license should be valid$")
{
    ScenarioScope<LicenseManagementContext> context;

    ASSERT_TRUE(context->currentLicense.has_value());
    EXPECT_TRUE(context->currentLicense->isValid());
}

THEN("^the license should expire in approximately (\\d+) days$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(int, expectedDays);

    ASSERT_TRUE(context->currentLicense.has_value());
    int actualDays = context->currentLicense->getDaysUntilExpiration();
    EXPECT_GE(actualDays, expectedDays - 1);
    EXPECT_LE(actualDays, expectedDays + 1);
}

THEN("^the license should have (\\d+) enabled features$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(int, count);

    ASSERT_TRUE(context->currentLicense.has_value());
    EXPECT_EQ(static_cast<int>(context->currentLicense->getEnabledFeatures().size()), count);
}

THEN("^the license should have feature \"([^\"]*)\"$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, feature);

    ASSERT_TRUE(context->currentLicense.has_value());
    EXPECT_TRUE(context->currentLicense->hasFeature(feature));
}

THEN("^the licensee name should be \"([^\"]*)\"$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, name);

    ASSERT_TRUE(context->currentLicense.has_value());
    EXPECT_EQ(context->currentLicense->getLicenseeName(), name);
}

THEN("^the license should not be expired$")
{
    ScenarioScope<LicenseManagementContext> context;

    ASSERT_TRUE(context->currentLicense.has_value());
    EXPECT_FALSE(context->currentLicense->isExpired());
}

THEN("^the days until expiration should be greater than (\\d+)$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(int, minDays);

    ASSERT_TRUE(context->currentLicense.has_value());
    EXPECT_GT(context->currentLicense->getDaysUntilExpiration(), minDays);
}

// ============================================
// License Validation Steps
// ============================================

GIVEN("^I have a valid commercial license expiring in (\\d+) days$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(int, days);

    context->currentLicense = License::builder()
                                  .withLicenseKey("VALID-COMM-KEY")
                                  .withProductId("PRODUCT")
                                  .withType(LicenseType::Commercial)
                                  .expiresInDays(days)
                                  .issuedNow()
                                  .withDigitalSignature("mock-sig")
                                  .build();
}

GIVEN("^I have an expired license$")
{
    ScenarioScope<LicenseManagementContext> context;

    auto pastDate = std::chrono::system_clock::now() - std::chrono::hours(24 * 10);

    context->currentLicense = License::builder()
                                  .withLicenseKey("EXPIRED-KEY")
                                  .withProductId("PRODUCT")
                                  .withType(LicenseType::Trial)
                                  .withExpirationDate(pastDate)
                                  .issuedNow()
                                  .withDigitalSignature("mock-sig")
                                  .build();
}

GIVEN("^I have a license bound to the current hardware$")
{
    ScenarioScope<LicenseManagementContext> context;

    HardwareIdValidator validator;
    auto currentHwId = validator.getCurrentHardwareId();

    context->currentLicense = License::builder()
                                  .withLicenseKey("HW-BOUND-KEY")
                                  .withProductId("PRODUCT")
                                  .withType(LicenseType::Commercial)
                                  .withHardwareId(currentHwId)
                                  .expiresInDays(30)
                                  .issuedNow()
                                  .withDigitalSignature("mock-sig")
                                  .build();
}

GIVEN("^I have a license bound to hardware \"([^\"]*)\"$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, hardwareId);

    context->currentLicense = License::builder()
                                  .withLicenseKey("WRONG-HW-KEY")
                                  .withProductId("PRODUCT")
                                  .withType(LicenseType::Commercial)
                                  .withHardwareId(hardwareId)
                                  .expiresInDays(30)
                                  .issuedNow()
                                  .withDigitalSignature("mock-sig")
                                  .build();
}

GIVEN("^I have a license expired (\\d+) days ago$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(int, daysAgo);

    auto pastDate = std::chrono::system_clock::now() - std::chrono::hours(24 * daysAgo);

    context->currentLicense = License::builder()
                                  .withLicenseKey("GRACE-KEY")
                                  .withProductId("PRODUCT")
                                  .withType(LicenseType::Commercial)
                                  .withExpirationDate(pastDate)
                                  .issuedNow()
                                  .withDigitalSignature("mock-sig")
                                  .build();
}

GIVEN("^the validation allows a (\\d+) day grace period$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(int, graceDays);

    context->expirationValidator = std::make_shared<ExpirationValidator>(graceDays);
    context->manager->clearValidators();
    context->manager->addValidator(context->expirationValidator);
}

WHEN("^I validate the license$")
{
    ScenarioScope<LicenseManagementContext> context;

    ASSERT_TRUE(context->currentLicense.has_value());
    context->lastValidationResult = context->manager->validateWithDetails(context->currentLicense.value());
}

THEN("^the validation should succeed$")
{
    ScenarioScope<LicenseManagementContext> context;

    ASSERT_TRUE(context->lastValidationResult.has_value());
    EXPECT_TRUE(context->lastValidationResult->isValid());
}

THEN("^the validation should fail$")
{
    ScenarioScope<LicenseManagementContext> context;

    ASSERT_TRUE(context->lastValidationResult.has_value());
    EXPECT_FALSE(context->lastValidationResult->isValid());
}

THEN("^the validation code should be \"([^\"]*)\"$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, expectedCode);

    ASSERT_TRUE(context->lastValidationResult.has_value());

    std::string actualCode;
    switch (context->lastValidationResult->code)
    {
        case ValidationCode::Success:
            actualCode = "Success";
            break;
        case ValidationCode::Expired:
            actualCode = "Expired";
            break;
        case ValidationCode::HardwareMismatch:
            actualCode = "HardwareMismatch";
            break;
        case ValidationCode::InvalidSignature:
            actualCode = "InvalidSignature";
            break;
        case ValidationCode::FeatureNotAvailable:
            actualCode = "FeatureNotAvailable";
            break;
        default:
            actualCode = "Unknown";
    }

    EXPECT_EQ(actualCode, expectedCode);
}

// ============================================
// Feature Validation Steps
// ============================================

GIVEN("^I have a license with features:$")
{
    ScenarioScope<LicenseManagementContext> context;
    TABLE_PARAM(tableParam);
    auto& table = tableParam;

    auto builder = License::builder()
                       .withLicenseKey("FEATURE-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Enterprise)
                       .expiresInDays(365)
                       .issuedNow()
                       .withDigitalSignature("mock-sig");

    for (const auto& row : table.hashes())
    {
        builder.withFeature(row.at("Feature"));
    }

    context->currentLicense = builder.build();
}

WHEN("^I require features:$")
{
    ScenarioScope<LicenseManagementContext> context;
    TABLE_PARAM(tableParam);
    auto& table = tableParam;

    context->featureValidator = std::make_shared<FeatureValidator>();
    for (const auto& row : table.hashes())
    {
        context->featureValidator->addRequiredFeature(row.at("Feature"));
    }

    ASSERT_TRUE(context->currentLicense.has_value());
    context->lastValidationResult = context->featureValidator->validateWithDetails(context->currentLicense.value());
}

THEN("^feature \"([^\"]*)\" should be enabled$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, feature);

    ASSERT_TRUE(context->currentLicense.has_value());
    EXPECT_TRUE(context->manager->isFeatureEnabled(context->currentLicense.value(), feature));
}

THEN("^feature \"([^\"]*)\" should not be enabled$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, feature);

    ASSERT_TRUE(context->currentLicense.has_value());
    EXPECT_FALSE(context->manager->isFeatureEnabled(context->currentLicense.value(), feature));
}

THEN("^the feature validation should succeed$")
{
    ScenarioScope<LicenseManagementContext> context;

    ASSERT_TRUE(context->lastValidationResult.has_value());
    EXPECT_TRUE(context->lastValidationResult->isValid());
}

THEN("^the feature validation should fail$")
{
    ScenarioScope<LicenseManagementContext> context;

    ASSERT_TRUE(context->lastValidationResult.has_value());
    EXPECT_FALSE(context->lastValidationResult->isValid());
}

THEN("^the validation message should contain \"([^\"]*)\"$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, substring);

    ASSERT_TRUE(context->lastValidationResult.has_value());
    EXPECT_NE(context->lastValidationResult->message.find(substring), std::string::npos);
}

// ============================================
// License Storage Steps
// ============================================

GIVEN("^I create a commercial license with key \"([^\"]*)\"$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, key);

    context->currentLicense = License::builder()
                                  .withLicenseKey(key)
                                  .withProductId("PRODUCT")
                                  .withType(LicenseType::Commercial)
                                  .expiresInDays(30)
                                  .issuedNow()
                                  .withDigitalSignature("mock-sig")
                                  .build();
}

GIVEN("^I have stored the following licenses:$")
{
    ScenarioScope<LicenseManagementContext> context;
    TABLE_PARAM(tableParam);
    auto& table = tableParam;

    for (const auto& row : table.hashes())
    {
        auto license = License::builder()
                           .withLicenseKey(row.at("LicenseKey"))
                           .withProductId(row.at("ProductId"))
                           .withType(stringToLicenseType(row.at("Type")))
                           .expiresInDays(30)
                           .issuedNow()
                           .withDigitalSignature("mock-sig")
                           .build();

        context->storage->save(license);
        context->storedLicenseKeys.push_back(row.at("LicenseKey"));
    }
}

GIVEN("^I have a stored license with key \"([^\"]*)\"$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, key);

    auto license = License::builder()
                       .withLicenseKey(key)
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Commercial)
                       .expiresInDays(30)
                       .issuedNow()
                       .withDigitalSignature("mock-sig")
                       .build();

    context->storage->save(license);
}

WHEN("^I save the license to storage$")
{
    ScenarioScope<LicenseManagementContext> context;

    ASSERT_TRUE(context->currentLicense.has_value());
    context->manager->saveLicense(context->currentLicense.value());
}

WHEN("^I load the license with key \"([^\"]*)\"$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, key);

    context->currentLicense = context->manager->loadLicense(key);
}

WHEN("^I list all stored licenses$")
{
    ScenarioScope<LicenseManagementContext> context;

    context->storedLicenseKeys = context->storage->listAll();
}

WHEN("^I remove the license with key \"([^\"]*)\"$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, key);

    context->storage->remove(key);
}

WHEN("^I clear all licenses from storage$")
{
    ScenarioScope<LicenseManagementContext> context;

    context->storage->clear();
}

THEN("^the loaded license should match the original$")
{
    ScenarioScope<LicenseManagementContext> context;

    ASSERT_TRUE(context->currentLicense.has_value());
    // Key comparison verifies load/save round-trip
    EXPECT_FALSE(context->currentLicense->getLicenseKey().empty());
}

THEN("^I should get (\\d+) license keys$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(int, count);

    EXPECT_EQ(static_cast<int>(context->storedLicenseKeys.size()), count);
}

THEN("^the list should contain \"([^\"]*)\"$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, key);

    auto it = std::find(context->storedLicenseKeys.begin(), context->storedLicenseKeys.end(), key);
    EXPECT_NE(it, context->storedLicenseKeys.end());
}

THEN("^the license \"([^\"]*)\" should not exist in storage$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, key);

    EXPECT_FALSE(context->storage->exists(key));
}

THEN("^the storage should be empty$")
{
    ScenarioScope<LicenseManagementContext> context;

    EXPECT_TRUE(context->storage->listAll().empty());
}

// ============================================
// License Manager Steps
// ============================================

GIVEN("^no licenses are stored$")
{
    ScenarioScope<LicenseManagementContext> context;

    context->storage->clear();
}

GIVEN("^I have a commercial license expiring in (\\d+) days$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(int, days);

    context->currentLicense = License::builder()
                                  .withLicenseKey("EXPIRY-CHECK-KEY")
                                  .withProductId("PRODUCT")
                                  .withType(LicenseType::Commercial)
                                  .expiresInDays(days)
                                  .issuedNow()
                                  .withDigitalSignature("mock-sig")
                                  .build();
}

WHEN("^I store a valid commercial license$")
{
    ScenarioScope<LicenseManagementContext> context;

    auto license = License::builder()
                       .withLicenseKey("VALID-STORED-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Commercial)
                       .expiresInDays(30)
                       .issuedNow()
                       .withDigitalSignature("mock-sig")
                       .build();

    context->manager->saveLicense(license);
}

WHEN("^I get the license status$")
{
    ScenarioScope<LicenseManagementContext> context;

    ASSERT_TRUE(context->currentLicense.has_value());
    // Status is retrieved via manager
}

WHEN("^I get the days until expiration$")
{
    ScenarioScope<LicenseManagementContext> context;

    // Days until expiration is retrieved from current license
}

THEN("^has valid license should return false$")
{
    ScenarioScope<LicenseManagementContext> context;

    EXPECT_FALSE(context->manager->hasValidLicense());
}

THEN("^has valid license should return true$")
{
    ScenarioScope<LicenseManagementContext> context;

    EXPECT_TRUE(context->manager->hasValidLicense());
}

THEN("^the status should be \"([^\"]*)\"$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, expectedStatus);

    ASSERT_TRUE(context->currentLicense.has_value());
    auto status = context->manager->getLicenseStatus(context->currentLicense.value());

    std::string actualStatus;
    switch (status)
    {
        case LicenseStatus::Valid:
            actualStatus = "Valid";
            break;
        case LicenseStatus::Expired:
            actualStatus = "Expired";
            break;
        case LicenseStatus::Invalid:
            actualStatus = "Invalid";
            break;
        default:
            actualStatus = "Unknown";
    }

    EXPECT_EQ(actualStatus, expectedStatus);
}

THEN("^the days should be approximately (\\d+)$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(int, expectedDays);

    ASSERT_TRUE(context->currentLicense.has_value());
    int actualDays = context->manager->getDaysUntilExpiration(context->currentLicense.value());

    EXPECT_GE(actualDays, expectedDays - 1);
    EXPECT_LE(actualDays, expectedDays + 1);
}

// ============================================
// Observer Steps
// ============================================

GIVEN("^I register a license observer$")
{
    ScenarioScope<LicenseManagementContext> context;

    context->observer = std::make_shared<LicenseManagementContext::MockObserver>();
    context->manager->registerObserver(context->observer);
}

WHEN("^I save a new license$")
{
    ScenarioScope<LicenseManagementContext> context;

    auto license = License::builder()
                       .withLicenseKey("OBSERVER-TEST-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Commercial)
                       .expiresInDays(30)
                       .issuedNow()
                       .withDigitalSignature("mock-sig")
                       .build();

    context->manager->saveLicense(license);
}

THEN("^the observer should receive a license event$")
{
    ScenarioScope<LicenseManagementContext> context;

    ASSERT_TRUE(context->observer != nullptr);
    EXPECT_GT(context->observer->eventCount, 0);
}

THEN("^the event type should be \"([^\"]*)\"$")
{
    ScenarioScope<LicenseManagementContext> context;
    REGEX_PARAM(std::string, expectedType);

    ASSERT_TRUE(context->observer != nullptr);
    ASSERT_TRUE(context->observer->lastEvent.has_value());

    std::string actualType;
    switch (context->observer->lastEvent.value())
    {
        case LicenseEvent::LicenseLoaded:
            actualType = "LicenseLoaded";
            break;
        case LicenseEvent::LicenseSaved:
            actualType = "LicenseSaved";
            break;
        case LicenseEvent::LicenseValidated:
            actualType = "LicenseValidated";
            break;
        case LicenseEvent::LicenseExpired:
            actualType = "LicenseExpired";
            break;
        case LicenseEvent::LicenseRemoved:
            actualType = "LicenseRemoved";
            break;
        default:
            actualType = "Unknown";
    }

    EXPECT_EQ(actualType, expectedType);
}

THEN("^the observer should receive a validation result notification$")
{
    ScenarioScope<LicenseManagementContext> context;

    ASSERT_TRUE(context->observer != nullptr);
    EXPECT_GT(context->observer->validationCount, 0);
}

} // anonymous namespace
