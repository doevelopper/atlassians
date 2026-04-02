/**
 * @file   LicenseValidatorTest.cpp
 * @brief  Unit tests for license validators implementation
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/licensing/LicenseValidatorTest.hpp>

using namespace com::github::doevelopper::atlassians::licensing;
using namespace com::github::doevelopper::atlassians::licensing::test;

log4cxx::LoggerPtr LicenseValidatorTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.atlassians.licensing.test.LicenseValidatorTest"));

LicenseValidatorTest::LicenseValidatorTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

LicenseValidatorTest::~LicenseValidatorTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void LicenseValidatorTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void LicenseValidatorTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto LicenseValidatorTest::createValidLicense() const -> License
{
    return License::builder()
        .withLicenseKey("VALID-TEST-KEY")
        .withProductId("TEST-PRODUCT")
        .withType(LicenseType::Commercial)
        .expiresInDays(30)
        .issuedNow()
        .withDigitalSignature("mock-sig")
        .build();
}

auto LicenseValidatorTest::createExpiredLicense() const -> License
{
    auto pastDate = std::chrono::system_clock::now() - std::chrono::hours(24 * 10);

    return License::builder()
        .withLicenseKey("EXPIRED-TEST-KEY")
        .withProductId("TEST-PRODUCT")
        .withType(LicenseType::Trial)
        .withExpirationDate(pastDate)
        .issuedNow()
        .withDigitalSignature("mock-sig")
        .build();
}

auto LicenseValidatorTest::createHardwareBoundLicense(std::string_view hardwareId) const -> License
{
    return License::builder()
        .withLicenseKey("HW-BOUND-KEY")
        .withProductId("TEST-PRODUCT")
        .withType(LicenseType::Commercial)
        .withHardwareId(hardwareId)
        .expiresInDays(30)
        .issuedNow()
        .withDigitalSignature("mock-sig")
        .build();
}

auto LicenseValidatorTest::createLicenseWithFeatures(const std::vector<std::string>& features) const -> License
{
    auto builder = License::builder();
    builder.withLicenseKey("FEATURE-KEY")
            .withProductId("TEST-PRODUCT")
            .withType(LicenseType::Enterprise)
            .expiresInDays(30)
            .issuedNow()
            .withDigitalSignature("mock-sig");

    for (const auto& f : features)
    {
        builder.withFeature(f);
    }

    return builder.build();
}

// ============================================
// ExpirationValidator Tests
// ============================================

TEST_F(LicenseValidatorTest, ExpirationValidator_ValidLicense_ReturnsSuccess)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    ExpirationValidator validator;
    auto license = createValidLicense();

    EXPECT_TRUE(validator.validate(license));

    auto result = validator.validateWithDetails(license);
    EXPECT_TRUE(result.isValid());
    EXPECT_EQ(result.code, ValidationCode::Success);
}

TEST_F(LicenseValidatorTest, ExpirationValidator_ExpiredLicense_ReturnsExpired)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    ExpirationValidator validator;
    auto license = createExpiredLicense();

    EXPECT_FALSE(validator.validate(license));

    auto result = validator.validateWithDetails(license);
    EXPECT_FALSE(result.isValid());
    EXPECT_EQ(result.code, ValidationCode::Expired);
}

TEST_F(LicenseValidatorTest, ExpirationValidator_WithGracePeriod_ExpiredButInGrace)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Create a license expired 3 days ago
    auto expiredDate = std::chrono::system_clock::now() - std::chrono::hours(24 * 3);
    auto license     = License::builder()
                        .withLicenseKey("GRACE-KEY")
                        .withProductId("PRODUCT")
                        .withType(LicenseType::Commercial)
                        .withExpirationDate(expiredDate)
                        .issuedNow()
                        .withDigitalSignature("sig")
                        .build();

    // Validator with 7-day grace period
    ExpirationValidator validator(7);

    EXPECT_TRUE(validator.validate(license));

    auto result = validator.validateWithDetails(license);
    EXPECT_TRUE(result.isValid());
    // Result should indicate grace period in details
    EXPECT_TRUE(result.details.has_value());
}

TEST_F(LicenseValidatorTest, ExpirationValidator_PerpetualLicense_AlwaysValid)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = License::builder()
                        .withLicenseKey("PERPETUAL-KEY")
                        .withProductId("PRODUCT")
                        .withType(LicenseType::Enterprise)
                        .issuedNow()
                        .withDigitalSignature("sig")
                       // No expiration date
                        .build();

    ExpirationValidator validator;

    EXPECT_TRUE(validator.validate(license));
}

TEST_F(LicenseValidatorTest, ExpirationValidator_ExpiringInWarningPeriod_ReturnsSuccessWithWarning)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = License::builder()
                        .withLicenseKey("EXPIRING-KEY")
                        .withProductId("PRODUCT")
                        .withType(LicenseType::Commercial)
                        .expiresInDays(15) // Within 30-day warning
                        .issuedNow()
                        .withDigitalSignature("sig")
                        .build();

    ExpirationValidator validator;
    auto result = validator.validateWithDetails(license);

    EXPECT_TRUE(result.isValid());
    EXPECT_TRUE(result.details.has_value());
}

// ============================================
// HardwareIdValidator Tests
// ============================================

TEST_F(LicenseValidatorTest, HardwareIdValidator_NonBoundLicense_ReturnsSuccess)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    HardwareIdValidator validator;
    auto license = createValidLicense(); // Not hardware-bound

    EXPECT_TRUE(validator.validate(license));
}

TEST_F(LicenseValidatorTest, HardwareIdValidator_MatchingHardwareId_ReturnsSuccess)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Create validator with custom hardware ID generator
    std::string testHardwareId = "TEST-HW-ID-12345";
    HardwareIdValidator validator([testHardwareId]() { return testHardwareId; });

    auto license = createHardwareBoundLicense(testHardwareId);

    EXPECT_TRUE(validator.validate(license));
}

TEST_F(LicenseValidatorTest, HardwareIdValidator_MismatchedHardwareId_ReturnsFailure)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Create validator with custom hardware ID generator
    HardwareIdValidator validator([]() { return "ACTUAL-HW-ID"; });

    auto license = createHardwareBoundLicense("DIFFERENT-HW-ID");

    EXPECT_FALSE(validator.validate(license));

    auto result = validator.validateWithDetails(license);
    EXPECT_EQ(result.code, ValidationCode::HardwareMismatch);
}

TEST_F(LicenseValidatorTest, HardwareIdValidator_GetCurrentHardwareId_ReturnsNonEmpty)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    HardwareIdValidator validator;
    auto hwId = validator.getCurrentHardwareId();

    EXPECT_FALSE(hwId.empty());
}

// ============================================
// FeatureValidator Tests
// ============================================

TEST_F(LicenseValidatorTest, FeatureValidator_NoRequiredFeatures_ReturnsSuccess)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    FeatureValidator validator;
    auto license = createValidLicense();

    EXPECT_TRUE(validator.validate(license));
}

TEST_F(LicenseValidatorTest, FeatureValidator_AllFeaturesPresent_ReturnsSuccess)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    FeatureValidator validator;
    validator.addRequiredFeature("FEATURE_A");
    validator.addRequiredFeature("FEATURE_B");

    auto license = createLicenseWithFeatures({"FEATURE_A", "FEATURE_B", "FEATURE_C"});

    EXPECT_TRUE(validator.validate(license));
}

TEST_F(LicenseValidatorTest, FeatureValidator_MissingFeature_ReturnsFailure)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    FeatureValidator validator;
    validator.addRequiredFeature("FEATURE_A");
    validator.addRequiredFeature("FEATURE_D"); // Not in license

    auto license = createLicenseWithFeatures({"FEATURE_A", "FEATURE_B"});

    EXPECT_FALSE(validator.validate(license));

    auto result = validator.validateWithDetails(license);
    EXPECT_EQ(result.code, ValidationCode::FeatureNotAvailable);
    EXPECT_NE(result.message.find("FEATURE_D"), std::string::npos);
}

TEST_F(LicenseValidatorTest, FeatureValidator_ClearRequiredFeatures_WorksCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    FeatureValidator validator;
    validator.addRequiredFeature("FEATURE_A");
    validator.clearRequiredFeatures();

    auto license = createValidLicense(); // No features

    EXPECT_TRUE(validator.validate(license));
}

TEST_F(LicenseValidatorTest, FeatureValidator_GetRequiredFeatures_ReturnsAll)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    FeatureValidator validator;
    validator.addRequiredFeature("FEATURE_A");
    validator.addRequiredFeature("FEATURE_B");

    auto features = validator.getRequiredFeatures();

    EXPECT_EQ(features.size(), 2);
    EXPECT_TRUE(validator.isFeatureRequired("FEATURE_A"));
    EXPECT_TRUE(validator.isFeatureRequired("FEATURE_B"));
    EXPECT_FALSE(validator.isFeatureRequired("FEATURE_C"));
}

// ============================================
// ValidationChain Tests
// ============================================

TEST_F(LicenseValidatorTest, ValidationChain_EmptyChain_ReturnsSuccess)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    ValidationChain chain;
    auto license = createValidLicense();

    EXPECT_TRUE(chain.validate(license));
}

TEST_F(LicenseValidatorTest, ValidationChain_AllValidatorsPass_ReturnsSuccess)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    ValidationChain chain;
    chain.addValidator(std::make_shared<ExpirationValidator>());
    chain.addValidator(std::make_shared<HardwareIdValidator>());

    auto license = createValidLicense();

    EXPECT_TRUE(chain.validate(license));
}

TEST_F(LicenseValidatorTest, ValidationChain_OneValidatorFails_ReturnsFailure)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    ValidationChain chain;
    chain.addValidator(std::make_shared<ExpirationValidator>());

    auto license = createExpiredLicense();

    EXPECT_FALSE(chain.validate(license));

    auto result = chain.validateWithDetails(license);
    EXPECT_EQ(result.code, ValidationCode::Expired);
}

TEST_F(LicenseValidatorTest, ValidationChain_ValidateAll_ReturnsAllResults)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    ValidationChain chain;
    chain.addValidator(std::make_shared<ExpirationValidator>());
    chain.addValidator(std::make_shared<FeatureValidator>());

    auto license   = createValidLicense();
    auto results = chain.validateAll(license);

    EXPECT_EQ(results.size(), 2);
}

TEST_F(LicenseValidatorTest, ValidationChain_SortByPriority_OrdersCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    ValidationChain chain;

    // Add in reverse priority order
    auto featureValidator = std::make_shared<FeatureValidator>();     // Priority 30
    auto hwValidator      = std::make_shared<HardwareIdValidator>(); // Priority 20
    auto expValidator     = std::make_shared<ExpirationValidator>(); // Priority 10

    chain.addValidator(featureValidator);
    chain.addValidator(hwValidator);
    chain.addValidator(expValidator);

    chain.sortByPriority();

    EXPECT_EQ(chain.getValidatorCount(), 3);
}

TEST_F(LicenseValidatorTest, ValidationChain_RemoveValidator_RemovesCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    ValidationChain chain;
    chain.addValidator(std::make_shared<ExpirationValidator>());
    chain.addValidator(std::make_shared<FeatureValidator>());

    EXPECT_EQ(chain.getValidatorCount(), 2);

    bool removed = chain.removeValidator("ExpirationValidator");
    EXPECT_TRUE(removed);
    EXPECT_EQ(chain.getValidatorCount(), 1);

    removed = chain.removeValidator("NonExistent");
    EXPECT_FALSE(removed);
}
