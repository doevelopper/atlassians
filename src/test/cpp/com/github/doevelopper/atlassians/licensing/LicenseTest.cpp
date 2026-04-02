/**
 * @file   LicenseTest.cpp
 * @brief  Unit tests for License entity implementation
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/licensing/LicenseTest.hpp>

using namespace com::github::doevelopper::atlassians::licensing;
using namespace com::github::doevelopper::atlassians::licensing::test;

log4cxx::LoggerPtr LicenseTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.atlassians.licensing.test.LicenseTest"));

LicenseTest::LicenseTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

LicenseTest::~LicenseTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void LicenseTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void LicenseTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto LicenseTest::createValidLicense() const -> License
{
    return License::builder()
        .withLicenseKey("TEST-LICENSE-KEY-12345")
        .withProductId("PREMISSES-PRO")
        .withType(LicenseType::Commercial)
        .withLicenseeName("Test User")
        .withLicenseeEmail("test@example.com")
        .expiresInDays(30)
        .issuedNow()
        .withFeature("FEATURE_A")
        .withFeature("FEATURE_B")
        .withDigitalSignature("mock-signature")
        .build();
}

auto LicenseTest::createExpiredLicense() const -> License
{
    auto pastDate = std::chrono::system_clock::now() - std::chrono::hours(24 * 10);

    return License::builder()
        .withLicenseKey("EXPIRED-LICENSE-KEY")
        .withProductId("PREMISSES-PRO")
        .withType(LicenseType::Trial)
        .withExpirationDate(pastDate)
        .issuedNow()
        .withDigitalSignature("mock-signature")
        .build();
}

auto LicenseTest::createLicenseWithFeatures(const std::vector<std::string>& features) const -> License
{
    auto builder = License::builder();
    builder.withLicenseKey("FEATURE-LICENSE-KEY")
           .withProductId("PREMISSES-PRO")
           .withType(LicenseType::Enterprise)
           .expiresInDays(365)
           .issuedNow()
           .withDigitalSignature("mock-signature");

    for (const auto& feature : features)
    {
        builder.withFeature(feature);
    }

    return builder.build();
}

// ============================================
// License Builder Tests
// ============================================

TEST_F(LicenseTest, Builder_CreateValidLicense_Success)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = License::builder()
                       .withLicenseKey("TEST-KEY")
                       .withProductId("TEST-PRODUCT")
                       .withType(LicenseType::Commercial)
                       .expiresInDays(30)
                       .build();

    EXPECT_EQ(license.getLicenseKey(), "TEST-KEY");
    EXPECT_EQ(license.getProductId(), "TEST-PRODUCT");
    EXPECT_EQ(license.getType(), LicenseType::Commercial);
    EXPECT_TRUE(license.isValid());
}

TEST_F(LicenseTest, Builder_MissingLicenseKey_ThrowsException)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    EXPECT_THROW(
        {
            License::builder()
                .withProductId("TEST-PRODUCT")
                .withType(LicenseType::Commercial)
                .build();
        },
        LicenseInvalidException);
}

TEST_F(LicenseTest, Builder_MissingProductId_ThrowsException)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    EXPECT_THROW(
        {
            License::builder()
                .withLicenseKey("TEST-KEY")
                .withType(LicenseType::Commercial)
                .build();
        },
        LicenseInvalidException);
}

TEST_F(LicenseTest, Builder_MissingType_ThrowsException)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    EXPECT_THROW(
        {
            License::builder()
                .withLicenseKey("TEST-KEY")
                .withProductId("TEST-PRODUCT")
                .build();
        },
        LicenseInvalidException);
}

TEST_F(LicenseTest, Builder_TryBuild_ReturnsNulloptOnError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto result = License::builder()
                      .withLicenseKey("TEST-KEY")
                      // Missing product ID and type
                      .tryBuild();

    EXPECT_FALSE(result.has_value());
}

// ============================================
// License Properties Tests
// ============================================

TEST_F(LicenseTest, License_GetProperties_ReturnsCorrectValues)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createValidLicense();

    EXPECT_EQ(license.getLicenseKey(), "TEST-LICENSE-KEY-12345");
    EXPECT_EQ(license.getProductId(), "PREMISSES-PRO");
    EXPECT_EQ(license.getType(), LicenseType::Commercial);
    EXPECT_EQ(license.getLicenseeName(), "Test User");
    EXPECT_EQ(license.getLicenseeEmail(), "test@example.com");
}

TEST_F(LicenseTest, License_Features_CorrectlyStored)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createValidLicense();

    EXPECT_TRUE(license.hasFeature("FEATURE_A"));
    EXPECT_TRUE(license.hasFeature("FEATURE_B"));
    EXPECT_FALSE(license.hasFeature("FEATURE_C"));
}

TEST_F(LicenseTest, License_MultipleFeatures_AllAvailable)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    std::vector<std::string> features = {"F1", "F2", "F3", "F4", "F5"};
    auto license                       = createLicenseWithFeatures(features);

    for (const auto& feature : features)
    {
        EXPECT_TRUE(license.hasFeature(feature)) << "Feature " << feature << " should be available";
    }

    EXPECT_EQ(license.getEnabledFeatures().size(), features.size());
}

// ============================================
// License Expiration Tests
// ============================================

TEST_F(LicenseTest, License_NotExpired_IsValidTrue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createValidLicense();

    EXPECT_FALSE(license.isExpired());
    EXPECT_TRUE(license.isValid());
}

TEST_F(LicenseTest, License_Expired_IsExpiredTrue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createExpiredLicense();

    EXPECT_TRUE(license.isExpired());
    EXPECT_FALSE(license.isValid());
}

TEST_F(LicenseTest, License_DaysUntilExpiration_CorrectValue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createValidLicense();
    int days     = license.getDaysUntilExpiration();

    // Should be approximately 30 days (allowing for test timing)
    EXPECT_GE(days, 29);
    EXPECT_LE(days, 31);
}

TEST_F(LicenseTest, License_ExpiredDaysAgo_NegativeDays)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createExpiredLicense();
    int days     = license.getDaysUntilExpiration();

    EXPECT_LT(days, 0);
}

TEST_F(LicenseTest, License_NoExpirationDate_Perpetual)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = License::builder()
                       .withLicenseKey("PERPETUAL-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Enterprise)
                       .issuedNow()
                       .withDigitalSignature("sig")
                       // No expiration date set
                       .build();

    EXPECT_FALSE(license.isExpired());
    EXPECT_GT(license.getDaysUntilExpiration(), 365 * 100); // Essentially "forever"
}

// ============================================
// Hardware Binding Tests
// ============================================

TEST_F(LicenseTest, License_WithHardwareId_IsHardwareBound)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = License::builder()
                       .withLicenseKey("HW-BOUND-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Commercial)
                       .withHardwareId("ABCD1234")
                       .expiresInDays(30)
                       .build();

    EXPECT_TRUE(license.isHardwareBound());
    EXPECT_TRUE(license.getHardwareId().has_value());
    EXPECT_EQ(license.getHardwareId().value(), "ABCD1234");
}

TEST_F(LicenseTest, License_WithoutHardwareId_NotHardwareBound)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createValidLicense();

    EXPECT_FALSE(license.isHardwareBound());
    EXPECT_FALSE(license.getHardwareId().has_value());
}

// ============================================
// Concurrent Users Tests
// ============================================

TEST_F(LicenseTest, License_WithConcurrencyLimit_HasLimit)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = License::builder()
                       .withLicenseKey("CONCURRENT-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Enterprise)
                       .withMaxConcurrentUsers(50)
                       .expiresInDays(30)
                       .build();

    EXPECT_TRUE(license.hasConcurrencyLimit());
    EXPECT_TRUE(license.getMaxConcurrentUsers().has_value());
    EXPECT_EQ(license.getMaxConcurrentUsers().value(), 50);
}

TEST_F(LicenseTest, License_WithoutConcurrencyLimit_NoLimit)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createValidLicense();

    EXPECT_FALSE(license.hasConcurrencyLimit());
    EXPECT_FALSE(license.getMaxConcurrentUsers().has_value());
}

// ============================================
// Rule of Five Tests
// ============================================

TEST_F(LicenseTest, License_CopyConstructor_CreatesIndependentCopy)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto original = createValidLicense();
    License copy(original);

    EXPECT_EQ(copy.getLicenseKey(), original.getLicenseKey());
    EXPECT_EQ(copy.getProductId(), original.getProductId());
    EXPECT_EQ(copy.getType(), original.getType());
    EXPECT_EQ(copy, original);
}

TEST_F(LicenseTest, License_CopyAssignment_CreatesIndependentCopy)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto original = createValidLicense();
    License copy  = License::createEmpty();
    copy          = original;

    EXPECT_EQ(copy.getLicenseKey(), original.getLicenseKey());
    EXPECT_EQ(copy, original);
}

TEST_F(LicenseTest, License_MoveConstructor_TransfersOwnership)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto original       = createValidLicense();
    auto expectedKey    = std::string(original.getLicenseKey());
    License moved(std::move(original));

    EXPECT_EQ(moved.getLicenseKey(), expectedKey);
    EXPECT_TRUE(moved.isValid());
}

TEST_F(LicenseTest, License_MoveAssignment_TransfersOwnership)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto original       = createValidLicense();
    auto expectedKey    = std::string(original.getLicenseKey());
    License moved       = License::createEmpty();
    moved               = std::move(original);

    EXPECT_EQ(moved.getLicenseKey(), expectedKey);
    EXPECT_TRUE(moved.isValid());
}

// ============================================
// Comparison Tests
// ============================================

TEST_F(LicenseTest, License_SameKey_AreEqual)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license1 = createValidLicense();
    auto license2 = createValidLicense();

    EXPECT_EQ(license1, license2);
    EXPECT_FALSE(license1 != license2);
}

TEST_F(LicenseTest, License_DifferentKey_AreNotEqual)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license1 = createValidLicense();
    auto license2 = createExpiredLicense();

    EXPECT_NE(license1, license2);
    EXPECT_FALSE(license1 == license2);
}

// ============================================
// ToString Tests
// ============================================

TEST_F(LicenseTest, License_ToString_ContainsKeyInfo)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license    = createValidLicense();
    std::string str = license.toString();

    EXPECT_NE(str.find("License["), std::string::npos);
    EXPECT_NE(str.find("PREMISSES-PRO"), std::string::npos);
    EXPECT_NE(str.find("Commercial"), std::string::npos);
}

// ============================================
// License Type Conversion Tests
// ============================================

TEST_F(LicenseTest, LicenseTypeToString_AllTypesConverted)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    EXPECT_EQ(licenseTypeToString(LicenseType::Trial), "Trial");
    EXPECT_EQ(licenseTypeToString(LicenseType::Commercial), "Commercial");
    EXPECT_EQ(licenseTypeToString(LicenseType::Enterprise), "Enterprise");
    EXPECT_EQ(licenseTypeToString(LicenseType::Educational), "Educational");
    EXPECT_EQ(licenseTypeToString(LicenseType::doevelopper), "doevelopper");
    EXPECT_EQ(licenseTypeToString(LicenseType::Unknown), "Unknown");
}

TEST_F(LicenseTest, StringToLicenseType_AllTypesParsed)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    EXPECT_EQ(stringToLicenseType("Trial"), LicenseType::Trial);
    EXPECT_EQ(stringToLicenseType("Commercial"), LicenseType::Commercial);
    EXPECT_EQ(stringToLicenseType("Enterprise"), LicenseType::Enterprise);
    EXPECT_EQ(stringToLicenseType("Educational"), LicenseType::Educational);
    EXPECT_EQ(stringToLicenseType("doevelopper"), LicenseType::doevelopper);
    EXPECT_EQ(stringToLicenseType("Invalid"), LicenseType::Unknown);
}

// ============================================
// Metadata Tests
// ============================================

TEST_F(LicenseTest, License_WithMetadata_StoresCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = License::builder()
                       .withLicenseKey("META-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Commercial)
                       .expiresInDays(30)
                       .withMetadata("company", "ACME Corp")
                       .withMetadata("department", "Engineering")
                       .build();

    const auto& metadata = license.getMetadata();
    EXPECT_EQ(metadata.size(), 2);
    EXPECT_EQ(metadata[0].first, "company");
    EXPECT_EQ(metadata[0].second, "ACME Corp");
}
