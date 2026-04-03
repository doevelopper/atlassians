#include "SystemInfoTest.hpp"

using namespace com::github::doevelopper::atlassians::canary;
using namespace com::github::doevelopper::atlassians::canary::test;
using ::testing::Return;
using ::testing::_;

void SystemInfoTest::SetUp()
{
    // Initialize mock providers for fixture tests
    m_mock_cpu_provider = std::make_unique<MockCpuInfoProvider>();
    m_mock_memory_provider = std::make_unique<MockMemoryInfoProvider>();
    m_mock_load_provider = std::make_unique<MockLoadAverageProvider>();
    m_mock_uptime_provider = std::make_unique<MockUptimeProvider>();
}

void SystemInfoTest::TearDown()
{
    m_system_info.reset();
    m_mock_cpu_provider.reset();
    m_mock_memory_provider.reset();
    m_mock_load_provider.reset();
    m_mock_uptime_provider.reset();
}

// ============================================
// Constructor Tests
// ============================================

TEST_F(SystemInfoTest, Constructor_WithDefaultProviders_CreatesValidInstance)
{
    // Arrange & Act
    SystemInfo system_info;

    // Assert
    EXPECT_TRUE(system_info.areProvidersAvailable());
}

TEST_F(SystemInfoTest, Constructor_WithCustomProviders_CreatesValidInstance)
{
    // Arrange
    auto cpu_provider = std::make_unique<MockCpuInfoProvider>();
    auto memory_provider = std::make_unique<MockMemoryInfoProvider>();
    auto load_provider = std::make_unique<MockLoadAverageProvider>();
    auto uptime_provider = std::make_unique<MockUptimeProvider>();
    
    EXPECT_CALL(*cpu_provider, isAvailable())
        .WillOnce(Return(true));
    EXPECT_CALL(*memory_provider, isAvailable())
        .WillOnce(Return(true));
    EXPECT_CALL(*load_provider, isAvailable())
        .WillOnce(Return(true));
    EXPECT_CALL(*uptime_provider, isAvailable())
        .WillOnce(Return(true));

    // Act
    m_system_info = std::make_unique<SystemInfo>(
        std::move(cpu_provider),
        std::move(memory_provider),
        std::move(load_provider),
        std::move(uptime_provider)
    );

    // Assert
    EXPECT_TRUE(m_system_info->areProvidersAvailable());
}

// ============================================
// CPU Info Tests
// ============================================

TEST_F(SystemInfoTest, GetCpuInfo_WhenProviderReturnsData_ReturnsData)
{
    // Arrange
    CpuInfo cpu_info;
    cpu_info.processor_id = 0;
    cpu_info.model_name = "Test CPU";
    cpu_info.cpu_mhz = 2400.0;
    cpu_info.cpu_cores = 4;

    std::vector<CpuInfo> cpu_list = {cpu_info};

    auto cpu_provider = std::make_unique<MockCpuInfoProvider>();
    EXPECT_CALL(*cpu_provider, getCpuInfo())
        .WillOnce(Return(cpu_list));

    m_system_info = std::make_unique<SystemInfo>(
        std::move(cpu_provider),
        std::make_unique<MockMemoryInfoProvider>(),
        std::make_unique<MockLoadAverageProvider>(),
        std::make_unique<MockUptimeProvider>()
    );

    // Act
    auto result = m_system_info->getCpuInfo();

    // Assert
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->size(), 1);
    EXPECT_EQ(result->at(0).processor_id, 0);
    EXPECT_EQ(result->at(0).model_name, "Test CPU");
    EXPECT_DOUBLE_EQ(result->at(0).cpu_mhz, 2400.0);
    EXPECT_EQ(result->at(0).cpu_cores, 4);
}

TEST_F(SystemInfoTest, GetCpuInfo_WhenProviderReturnsNullopt_ReturnsNullopt)
{
    // Arrange
    auto cpu_provider = std::make_unique<MockCpuInfoProvider>();
    EXPECT_CALL(*cpu_provider, getCpuInfo())
        .WillOnce(Return(std::nullopt));

    m_system_info = std::make_unique<SystemInfo>(
        std::move(cpu_provider),
        std::make_unique<MockMemoryInfoProvider>(),
        std::make_unique<MockLoadAverageProvider>(),
        std::make_unique<MockUptimeProvider>()
    );

    // Act
    auto result = m_system_info->getCpuInfo();

    // Assert
    EXPECT_FALSE(result.has_value());
}

// ============================================
// Memory Info Tests
// ============================================

TEST_F(SystemInfoTest, GetMemoryInfo_WhenProviderReturnsData_ReturnsData)
{
    // Arrange
    MemoryInfo mem_info;
    mem_info.total_ram_kb = 16777216;  // 16 GB
    mem_info.available_ram_kb = 8388608;  // 8 GB
    mem_info.free_ram_kb = 4194304;  // 4 GB

    auto memory_provider = std::make_unique<MockMemoryInfoProvider>();
    EXPECT_CALL(*memory_provider, getMemoryInfo())
        .WillOnce(Return(mem_info));

    m_system_info = std::make_unique<SystemInfo>(
        std::make_unique<MockCpuInfoProvider>(),
        std::move(memory_provider),
        std::make_unique<MockLoadAverageProvider>(),
        std::make_unique<MockUptimeProvider>()
    );

    // Act
    auto result = m_system_info->getMemoryInfo();

    // Assert
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->total_ram_kb, 16777216);
    EXPECT_EQ(result->available_ram_kb, 8388608);
    EXPECT_NEAR(result->getUsedPercentage(), 50.0, 0.1);
}

// ============================================
// Load Average Tests
// ============================================

TEST_F(SystemInfoTest, GetLoadAverage_WhenProviderReturnsData_ReturnsData)
{
    // Arrange
    LoadAverage load;
    load.load_1min = 1.5;
    load.load_5min = 1.2;
    load.load_15min = 1.0;
    load.running_processes = 2;
    load.total_processes = 250;
auto load_provider = std::make_unique<MockLoadAverageProvider>();
    EXPECT_CALL(*load_provider, getLoadAverage())
        .WillOnce(Return(load));

    m_system_info = std::make_unique<SystemInfo>(
        std::make_unique<MockCpuInfoProvider>(),
        std::make_unique<MockMemoryInfoProvider>(),
        std::move(load_provider),        std::make_unique<MockUptimeProvider>()
    );

    // Act
    auto result = m_system_info->getLoadAverage();

    // Assert    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->load_1min, 1.5);
    EXPECT_DOUBLE_EQ(result->load_5min, 1.2);
    EXPECT_DOUBLE_EQ(result->load_15min, 1.0);
    EXPECT_EQ(result->running_processes, 2);
    EXPECT_EQ(result->total_processes, 250);
}

// ============================================
// Uptime Tests
// ============================================

TEST_F(SystemInfoTest, GetUptime_WhenProviderReturnsData_ReturnsData)
{
    // Arrange
    SystemUptime uptime;
    uptime.uptime_seconds = 86400.0;  // 1 day
    uptime.idle_time_seconds = 43200.0;  // 12 hours
    auto uptime_provider = std::make_unique<MockUptimeProvider>();
    EXPECT_CALL(*uptime_provider, getUptime())
        .WillOnce(Return(uptime));

    m_system_info = std::make_unique<SystemInfo>(
        std::make_unique<MockCpuInfoProvider>(),
        std::make_unique<MockMemoryInfoProvider>(),
        std::make_unique<MockLoadAverageProvider>(),
        std::move(uptime_provider)
    );

    // Act
    auto result = m_system_info->getUptime();

    // Assert
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result->uptime_seconds, 86400.0);
    EXPECT_DOUBLE_EQ(result->idle_time_seconds, 43200.0);
}

// ============================================
// System Snapshot Tests
// ============================================

TEST_F(SystemInfoTest, GetSystemSnapshot_WhenAllProvidersReturnData_ReturnsCompleteSnapshot)
{
    // Arrange
    CpuInfo cpu_info;
    cpu_info.model_name = "Test CPU";
    std::vector<CpuInfo> cpu_list = {cpu_info};

    MemoryInfo mem_info;
    mem_info.total_ram_kb = 16777216;

    LoadAverage load;
    load.load_1min = 1.5;

    SystemUptime uptime;
    uptime.uptime_seconds = 86400.0;  // 1 day
    uptime.idle_time_seconds = 43200.0;  // 12 hours

    auto cpu_provider = std::make_unique<MockCpuInfoProvider>();
    auto memory_provider = std::make_unique<MockMemoryInfoProvider>();
    auto load_provider = std::make_unique<MockLoadAverageProvider>();
    auto uptime_provider = std::make_unique<MockUptimeProvider>();

    EXPECT_CALL(*cpu_provider, getCpuInfo())
        .WillOnce(Return(cpu_list));
    EXPECT_CALL(*memory_provider, getMemoryInfo())
        .WillOnce(Return(mem_info));
    EXPECT_CALL(*load_provider, getLoadAverage())
        .WillOnce(Return(load));
    EXPECT_CALL(*uptime_provider, getUptime())
        .WillOnce(Return(uptime));

    m_system_info = std::make_unique<SystemInfo>(
        std::move(cpu_provider),
        std::move(memory_provider),
        std::move(load_provider),
        std::move(uptime_provider)
    );

    // Act
    auto result = m_system_info->getSystemSnapshot();

    // Assert
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->cpu_info.size(), 1);
    EXPECT_EQ(result->cpu_info[0].model_name, "Test CPU");
    EXPECT_EQ(result->memory_info.total_ram_kb, 16777216);
    EXPECT_DOUBLE_EQ(result->load_average.load_1min, 1.5);
    EXPECT_DOUBLE_EQ(result->uptime.uptime_seconds, 86400.0);
}

// ============================================
// Provider Validation Tests
// ============================================

TEST_F(SystemInfoTest, AreProvidersAvailable_WhenAllAvailable_ReturnsTrue)
{
    // Arrange
    // Get raw pointers before setting expectations (since we'll move the unique_ptrs)
    auto* cpu_provider = m_mock_cpu_provider.get();
    auto* memory_provider = m_mock_memory_provider.get();
    auto* load_provider = m_mock_load_provider.get();
    auto* uptime_provider = m_mock_uptime_provider.get();

    EXPECT_CALL(*cpu_provider, isAvailable())
        .WillOnce(Return(true));
    EXPECT_CALL(*memory_provider, isAvailable())
        .WillOnce(Return(true));
    EXPECT_CALL(*load_provider, isAvailable())
        .WillOnce(Return(true));
    EXPECT_CALL(*uptime_provider, isAvailable())
        .WillOnce(Return(true));

    m_system_info = std::make_unique<SystemInfo>(
        std::move(m_mock_cpu_provider),
        std::move(m_mock_memory_provider),
        std::move(m_mock_load_provider),
        std::move(m_mock_uptime_provider)
    );

    // Act
    bool result = m_system_info->areProvidersAvailable();

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(SystemInfoTest, AreProvidersAvailable_WhenOneProviderUnavailable_ReturnsFalse)
{
    // Arrange
    auto cpu_provider = std::make_unique<MockCpuInfoProvider>();
    auto memory_provider = std::make_unique<MockMemoryInfoProvider>();

    EXPECT_CALL(*cpu_provider, isAvailable())
        .WillOnce(Return(true));
    EXPECT_CALL(*memory_provider, isAvailable())
        .WillOnce(Return(false));  // One provider unavailable

    m_system_info = std::make_unique<SystemInfo>(
        std::move(cpu_provider),
        std::move(memory_provider),
        std::make_unique<MockLoadAverageProvider>(),
        std::make_unique<MockUptimeProvider>()
    );

    // Act
    bool result = m_system_info->areProvidersAvailable();

    // Assert
    EXPECT_FALSE(result);
}
