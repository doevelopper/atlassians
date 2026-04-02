/**
 * @file   LicenseManagerTest.cpp
 * @brief  Unit tests for LicenseManager facade implementation
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/licensing/LicenseManagerTest.hpp>

using namespace com::github::doevelopper::atlassians::licensing;
using namespace com::github::doevelopper::atlassians::licensing::test;

log4cxx::LoggerPtr LicenseManagerTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.atlassians.licensing.test.LicenseManagerTest"));

LicenseManagerTest::LicenseManagerTest()
    : m_testDir(std::filesystem::temp_directory_path() / "license_manager_test")
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

LicenseManagerTest::~LicenseManagerTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void LicenseManagerTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Reset singleton before each test
    LicenseManager::resetInstance();

    // Create fresh test directory
    std::filesystem::remove_all(m_testDir);
    std::filesystem::create_directories(m_testDir);

    // Get manager instance
    m_manager = LicenseManager::getInstance();

    // Configure with test storage
    auto storage = std::make_shared<FileLicenseStorage>(m_testDir);
    m_manager->setStorage(storage);
}

void LicenseManagerTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Reset singleton
    LicenseManager::resetInstance();

    // Clean up test directory
    std::error_code ec;
    std::filesystem::remove_all(m_testDir, ec);
}

auto LicenseManagerTest::createTestLicense() const -> License
{
    return License::builder()
        .withLicenseKey("MANAGER-TEST-KEY")
        .withProductId("TEST-PRODUCT")
        .withType(LicenseType::Commercial)
        .expiresInDays(30)
        .issuedNow()
        .withFeature("FEATURE_A")
        .withDigitalSignature("mock-sig")
        .build();
}

// ============================================
// Singleton Tests
// ============================================

TEST_F(LicenseManagerTest, GetInstance_ReturnsSameInstance)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto instance1 = LicenseManager::getInstance();
    auto instance2 = LicenseManager::getInstance();

    EXPECT_EQ(instance1.get(), instance2.get());
}

TEST_F(LicenseManagerTest, ResetInstance_CreatesNewInstance)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto instance1 = LicenseManager::getInstance();
    LicenseManager::resetInstance();
    auto instance2 = LicenseManager::getInstance();

    EXPECT_NE(instance1.get(), instance2.get());
}

// ============================================
// Storage Tests
// ============================================

TEST_F(LicenseManagerTest, SaveAndLoadLicense_Success)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createTestLicense();

    // Save
    EXPECT_NO_THROW(m_manager->saveLicense(license));

    // Load
    auto loaded = m_manager->loadLicense("MANAGER-TEST-KEY");

    EXPECT_EQ(loaded.getLicenseKey(), "MANAGER-TEST-KEY");
    EXPECT_EQ(loaded.getProductId(), "TEST-PRODUCT");
}

TEST_F(LicenseManagerTest, LoadLicense_NotFound_ThrowsException)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    EXPECT_THROW(m_manager->loadLicense("NON-EXISTENT-KEY"), LicenseStorageException);
}

TEST_F(LicenseManagerTest, LoadLicense_NoStorageConfigured_ThrowsException)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    LicenseManager::resetInstance();
    auto manager = LicenseManager::getInstance();
    // No storage configured

    EXPECT_THROW(manager->loadLicense("ANY-KEY"), LicenseStorageException);
}

// ============================================
// Validation Tests
// ============================================

TEST_F(LicenseManagerTest, Validate_ValidLicense_ReturnsTrue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createTestLicense();

    EXPECT_TRUE(m_manager->validate(license));
}

TEST_F(LicenseManagerTest, Validate_ExpiredLicense_ReturnsFalse)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto pastDate = std::chrono::system_clock::now() - std::chrono::hours(24 * 10);
    auto license  = License::builder()
                       .withLicenseKey("EXPIRED-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Trial)
                       .withExpirationDate(pastDate)
                       .issuedNow()
                       .withDigitalSignature("sig")
                       .build();

    EXPECT_FALSE(m_manager->validate(license));
}

TEST_F(LicenseManagerTest, ValidateWithDetails_ReturnsDetailedResult)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createTestLicense();
    auto result  = m_manager->validateWithDetails(license);

    EXPECT_TRUE(result.isValid());
    EXPECT_EQ(result.code, ValidationCode::Success);
}

// ============================================
// Feature Check Tests
// ============================================

TEST_F(LicenseManagerTest, IsFeatureEnabled_FeatureExists_ReturnsTrue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createTestLicense();

    EXPECT_TRUE(m_manager->isFeatureEnabled(license, "FEATURE_A"));
}

TEST_F(LicenseManagerTest, IsFeatureEnabled_FeatureNotExists_ReturnsFalse)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createTestLicense();

    EXPECT_FALSE(m_manager->isFeatureEnabled(license, "FEATURE_X"));
}

// ============================================
// Observer Tests
// ============================================

TEST_F(LicenseManagerTest, RegisterObserver_ReceivesEvents)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto observer = std::make_shared<MockLicenseObserver>();
    m_manager->registerObserver(observer);

    auto license = createTestLicense();
    m_manager->saveLicense(license);

    EXPECT_GT(observer->eventCount, 0);
}

TEST_F(LicenseManagerTest, ValidationNotifiesObserver)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto observer = std::make_shared<MockLicenseObserver>();
    m_manager->registerObserver(observer);

    auto license = createTestLicense();
    m_manager->validate(license);

    EXPECT_GT(observer->validationCount, 0);
}

TEST_F(LicenseManagerTest, UnregisterObserver_NoLongerReceivesEvents)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto observer = std::make_shared<MockLicenseObserver>();
    m_manager->registerObserver(observer);
    m_manager->unregisterObserver(observer);

    auto license = createTestLicense();
    m_manager->validate(license);

    // Should not receive events after unregistering
    EXPECT_EQ(observer->eventCount, 0);
}

// ============================================
// Validator Configuration Tests
// ============================================

TEST_F(LicenseManagerTest, ClearValidators_NoValidatorsRun)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    m_manager->clearValidators();

    // Even expired license should pass with no validators
    auto pastDate = std::chrono::system_clock::now() - std::chrono::hours(24 * 10);
    auto license  = License::builder()
                       .withLicenseKey("EXPIRED-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Trial)
                       .withExpirationDate(pastDate)
                       .issuedNow()
                       .withDigitalSignature("sig")
                       .build();

    // With no validators, validation chain returns success
    EXPECT_TRUE(m_manager->validate(license));
}

// ============================================
// Status and Information Tests
// ============================================

TEST_F(LicenseManagerTest, GetLicenseStatus_ValidLicense_ReturnsValid)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createTestLicense();
    auto status  = m_manager->getLicenseStatus(license);

    EXPECT_EQ(status, LicenseStatus::Valid);
}

TEST_F(LicenseManagerTest, GetLicenseStatus_ExpiredLicense_ReturnsExpired)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto pastDate = std::chrono::system_clock::now() - std::chrono::hours(24 * 10);
    auto license  = License::builder()
                       .withLicenseKey("EXPIRED-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Trial)
                       .withExpirationDate(pastDate)
                       .issuedNow()
                       .withDigitalSignature("sig")
                       .build();

    auto status = m_manager->getLicenseStatus(license);

    EXPECT_EQ(status, LicenseStatus::Expired);
}

TEST_F(LicenseManagerTest, GetDaysUntilExpiration_ReturnsCorrectValue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createTestLicense();
    int days     = m_manager->getDaysUntilExpiration(license);

    EXPECT_GE(days, 29);
    EXPECT_LE(days, 31);
}

TEST_F(LicenseManagerTest, GetEnabledFeatures_ReturnsAllFeatures)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license  = createTestLicense();
    auto features = m_manager->getEnabledFeatures(license);

    EXPECT_EQ(features.size(), 1);
    EXPECT_EQ(features[0], "FEATURE_A");
}

// ============================================
// HasValidLicense Tests
// ============================================

TEST_F(LicenseManagerTest, HasValidLicense_NoLicenses_ReturnsFalse)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    EXPECT_FALSE(m_manager->hasValidLicense());
}

TEST_F(LicenseManagerTest, HasValidLicense_WithValidLicense_ReturnsTrue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createTestLicense();
    m_manager->saveLicense(license);

    EXPECT_TRUE(m_manager->hasValidLicense());
}
