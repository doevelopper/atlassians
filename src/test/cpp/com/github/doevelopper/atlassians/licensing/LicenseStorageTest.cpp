/**
 * @file   LicenseStorageTest.cpp
 * @brief  Unit tests for License Storage implementations
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/licensing/LicenseStorageTest.hpp>
#include <fstream>

using namespace com::github::doevelopper::atlassians::licensing;
using namespace com::github::doevelopper::atlassians::licensing::test;

log4cxx::LoggerPtr LicenseStorageTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.atlassians.licensing.test.LicenseStorageTest"));

LicenseStorageTest::LicenseStorageTest()
    : m_testDir(std::filesystem::temp_directory_path() / "license_storage_test")
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

LicenseStorageTest::~LicenseStorageTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void LicenseStorageTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Create fresh test directory
    std::error_code ec;
    std::filesystem::remove_all(m_testDir, ec);
    std::filesystem::create_directories(m_testDir);

    m_storage = std::make_shared<FileLicenseStorage>(m_testDir);
}

void LicenseStorageTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Clean up test directory
    std::error_code ec;
    std::filesystem::remove_all(m_testDir, ec);
}

auto LicenseStorageTest::createTestLicense(std::string_view key) const -> License
{
    return License::builder()
        .withLicenseKey(key)
        .withProductId("TEST-PRODUCT")
        .withType(LicenseType::Commercial)
        .expiresInDays(30)
        .issuedNow()
        .withFeature("FEATURE_A")
        .withFeature("FEATURE_B")
        .withLicenseeName("Test User")
        .withLicenseeEmail("test@example.com")
        .withDigitalSignature("mock-signature")
        .build();
}

// ============================================
// FileLicenseStorage Basic Tests
// ============================================

TEST_F(LicenseStorageTest, Constructor_CreatesDirectory)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    std::filesystem::path newDir = m_testDir / "new_subdir";
    EXPECT_FALSE(std::filesystem::exists(newDir));

    FileLicenseStorage storage(newDir);

    EXPECT_TRUE(std::filesystem::exists(newDir));
}

TEST_F(LicenseStorageTest, Save_CreateFile_Success)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createTestLicense("SAVE-TEST-KEY");

    EXPECT_NO_THROW(m_storage->save(license));

    // Verify file exists
    auto filePath = m_testDir / "SAVE-TEST-KEY.lic";
    EXPECT_TRUE(std::filesystem::exists(filePath));
}

TEST_F(LicenseStorageTest, SaveAndLoad_DataIntegrity)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto original = createTestLicense("INTEGRITY-KEY");

    m_storage->save(original);
    auto loadedOpt = m_storage->load("INTEGRITY-KEY");

    ASSERT_TRUE(loadedOpt.has_value());
    auto& loaded = loadedOpt.value();

    EXPECT_EQ(loaded.getLicenseKey(), original.getLicenseKey());
    EXPECT_EQ(loaded.getProductId(), original.getProductId());
    EXPECT_EQ(loaded.getType(), original.getType());
    EXPECT_EQ(loaded.getLicenseeName(), original.getLicenseeName());
    EXPECT_EQ(loaded.getLicenseeEmail(), original.getLicenseeEmail());
}

TEST_F(LicenseStorageTest, Load_NonExistent_ReturnsNullopt)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto result = m_storage->load("NON-EXISTENT-KEY");
    EXPECT_FALSE(result.has_value());
}

// ============================================
// Exists Tests
// ============================================

TEST_F(LicenseStorageTest, Exists_LicensePresent_ReturnsTrue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createTestLicense("EXISTS-KEY");
    m_storage->save(license);

    EXPECT_TRUE(m_storage->exists("EXISTS-KEY"));
}

TEST_F(LicenseStorageTest, Exists_LicenseNotPresent_ReturnsFalse)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    EXPECT_FALSE(m_storage->exists("NOT-PRESENT-KEY"));
}

// ============================================
// Remove Tests
// ============================================

TEST_F(LicenseStorageTest, Remove_ExistingLicense_Success)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createTestLicense("REMOVE-KEY");
    m_storage->save(license);

    EXPECT_TRUE(m_storage->exists("REMOVE-KEY"));

    EXPECT_NO_THROW(m_storage->remove("REMOVE-KEY"));

    EXPECT_FALSE(m_storage->exists("REMOVE-KEY"));
}

TEST_F(LicenseStorageTest, Remove_NonExistent_NoThrow)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    EXPECT_NO_THROW(m_storage->remove("NON-EXISTENT"));
}

// ============================================
// ListAll Tests
// ============================================

TEST_F(LicenseStorageTest, ListAll_EmptyStorage_ReturnsEmpty)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto keys = m_storage->listAll();

    EXPECT_TRUE(keys.empty());
}

TEST_F(LicenseStorageTest, ListAll_MultipleLicenses_ReturnsAll)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    m_storage->save(createTestLicense("KEY-1"));
    m_storage->save(createTestLicense("KEY-2"));
    m_storage->save(createTestLicense("KEY-3"));

    auto keys = m_storage->listAll();

    EXPECT_EQ(keys.size(), 3);

    std::set<std::string> keySet(keys.begin(), keys.end());
    EXPECT_TRUE(keySet.count("KEY-1"));
    EXPECT_TRUE(keySet.count("KEY-2"));
    EXPECT_TRUE(keySet.count("KEY-3"));
}

// ============================================
// Clear Tests
// ============================================

TEST_F(LicenseStorageTest, Clear_RemovesAllLicenses)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    m_storage->save(createTestLicense("KEY-1"));
    m_storage->save(createTestLicense("KEY-2"));

    EXPECT_EQ(m_storage->listAll().size(), 2);

    m_storage->clear();

    EXPECT_EQ(m_storage->listAll().size(), 0);
}

// ============================================
// LoadPrimary Tests
// ============================================

TEST_F(LicenseStorageTest, LoadPrimary_SingleLicense_ReturnsIt)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = createTestLicense("PRIMARY-KEY");
    m_storage->save(license);

    auto primaryOpt = m_storage->loadPrimary();

    ASSERT_TRUE(primaryOpt.has_value());
    EXPECT_EQ(primaryOpt.value().getLicenseKey(), "PRIMARY-KEY");
}

TEST_F(LicenseStorageTest, LoadPrimary_NoLicenses_ReturnsNullopt)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto result = m_storage->loadPrimary();
    EXPECT_FALSE(result.has_value());
}

// ============================================
// Features Preservation Tests
// ============================================

TEST_F(LicenseStorageTest, SaveLoad_Features_Preserved)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = License::builder()
                       .withLicenseKey("FEATURE-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Enterprise)
                       .expiresInDays(365)
                       .issuedNow()
                       .withFeature("FEATURE_A")
                       .withFeature("FEATURE_B")
                       .withFeature("FEATURE_C")
                       .withDigitalSignature("sig")
                       .build();

    m_storage->save(license);
    auto loadedOpt = m_storage->load("FEATURE-KEY");

    ASSERT_TRUE(loadedOpt.has_value());
    auto& loaded = loadedOpt.value();

    EXPECT_TRUE(loaded.hasFeature("FEATURE_A"));
    EXPECT_TRUE(loaded.hasFeature("FEATURE_B"));
    EXPECT_TRUE(loaded.hasFeature("FEATURE_C"));
    EXPECT_EQ(loaded.getEnabledFeatures().size(), 3);
}

// ============================================
// Metadata Preservation Tests
// ============================================

TEST_F(LicenseStorageTest, SaveLoad_Metadata_Preserved)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = License::builder()
                       .withLicenseKey("META-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Commercial)
                       .expiresInDays(30)
                       .issuedNow()
                       .withMetadata("company", "ACME Corp")
                       .withMetadata("department", "Engineering")
                       .withDigitalSignature("sig")
                       .build();

    m_storage->save(license);
    auto loadedOpt = m_storage->load("META-KEY");

    ASSERT_TRUE(loadedOpt.has_value());
    const auto& metadata = loadedOpt.value().getMetadata();
    EXPECT_EQ(metadata.size(), 2);
}

// ============================================
// Optional Fields Tests
// ============================================

TEST_F(LicenseStorageTest, SaveLoad_HardwareId_Preserved)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = License::builder()
                       .withLicenseKey("HW-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Commercial)
                       .withHardwareId("HW-12345-ABCDE")
                       .expiresInDays(30)
                       .issuedNow()
                       .withDigitalSignature("sig")
                       .build();

    m_storage->save(license);
    auto loadedOpt = m_storage->load("HW-KEY");

    ASSERT_TRUE(loadedOpt.has_value());
    auto& loaded = loadedOpt.value();

    EXPECT_TRUE(loaded.getHardwareId().has_value());
    EXPECT_EQ(loaded.getHardwareId().value(), "HW-12345-ABCDE");
}

TEST_F(LicenseStorageTest, SaveLoad_MaxConcurrentUsers_Preserved)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = License::builder()
                       .withLicenseKey("CONCURRENT-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Enterprise)
                       .withMaxConcurrentUsers(100)
                       .expiresInDays(30)
                       .issuedNow()
                       .withDigitalSignature("sig")
                       .build();

    m_storage->save(license);
    auto loadedOpt = m_storage->load("CONCURRENT-KEY");

    ASSERT_TRUE(loadedOpt.has_value());
    auto& loaded = loadedOpt.value();

    EXPECT_TRUE(loaded.getMaxConcurrentUsers().has_value());
    EXPECT_EQ(loaded.getMaxConcurrentUsers().value(), 100);
}

// ============================================
// Date Preservation Tests
// ============================================

TEST_F(LicenseStorageTest, SaveLoad_Dates_Preserved)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto now      = std::chrono::system_clock::now();
    auto expDate  = now + std::chrono::hours(24 * 60);

    auto license = License::builder()
                       .withLicenseKey("DATE-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Commercial)
                       .withIssueDate(now)
                       .withExpirationDate(expDate)
                       .withDigitalSignature("sig")
                       .build();

    m_storage->save(license);
    auto loadedOpt = m_storage->load("DATE-KEY");

    ASSERT_TRUE(loadedOpt.has_value());
    auto& loaded = loadedOpt.value();

    // Check that dates are close (within 1 second)
    auto issueDiff = std::chrono::duration_cast<std::chrono::seconds>(
                         loaded.getIssueDate() - now)
                         .count();
    EXPECT_LE(std::abs(issueDiff), 1);
}

// ============================================
// Edge Cases
// ============================================

TEST_F(LicenseStorageTest, Save_SpecialCharactersInData_HandledCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license = License::builder()
                       .withLicenseKey("SPECIAL-KEY")
                       .withProductId("PRODUCT")
                       .withType(LicenseType::Commercial)
                       .withLicenseeName("John \"The Dev\" O'Brien")
                       .expiresInDays(30)
                       .issuedNow()
                       .withDigitalSignature("sig")
                       .build();

    m_storage->save(license);
    auto loadedOpt = m_storage->load("SPECIAL-KEY");

    ASSERT_TRUE(loadedOpt.has_value());
    EXPECT_EQ(loadedOpt.value().getLicenseeName(), "John \"The Dev\" O'Brien");
}

TEST_F(LicenseStorageTest, Save_Overwrite_Success)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto license1 = License::builder()
                        .withLicenseKey("OVERWRITE-KEY")
                        .withProductId("PRODUCT-1")
                        .withType(LicenseType::Trial)
                        .expiresInDays(7)
                        .issuedNow()
                        .withDigitalSignature("sig1")
                        .build();

    auto license2 = License::builder()
                        .withLicenseKey("OVERWRITE-KEY")
                        .withProductId("PRODUCT-2")
                        .withType(LicenseType::Commercial)
                        .expiresInDays(365)
                        .issuedNow()
                        .withDigitalSignature("sig2")
                        .build();

    m_storage->save(license1);
    m_storage->save(license2);

    auto loadedOpt = m_storage->load("OVERWRITE-KEY");

    ASSERT_TRUE(loadedOpt.has_value());
    auto& loaded = loadedOpt.value();

    EXPECT_EQ(loaded.getProductId(), "PRODUCT-2");
    EXPECT_EQ(loaded.getType(), LicenseType::Commercial);
}

// ============================================
// Thread Safety Tests
// ============================================

TEST_F(LicenseStorageTest, ConcurrentSaveLoad_ThreadSafe)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    const int numThreads     = 10;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};

    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([this, i, &successCount]() {
            try
            {
                std::string key = "THREAD-KEY-" + std::to_string(i);
                auto license    = createTestLicense(key);
                m_storage->save(license);
                auto loadedOpt = m_storage->load(key);
                if (loadedOpt.has_value() && loadedOpt.value().getLicenseKey() == key)
                {
                    successCount++;
                }
            }
            catch (...)
            {
                // Test should not throw
            }
        });
    }

    for (auto& t : threads)
    {
        t.join();
    }

    EXPECT_EQ(successCount, numThreads);
}
