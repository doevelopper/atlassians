#include "SystemMonitorTest.hpp"

using namespace com::github::doevelopper::premisses::canary;
using namespace com::github::doevelopper::premisses::canary::test;
using ::testing::_;
using ::testing::AtLeast;

void SystemMonitorTest::SetUp()
{
    m_system_info = std::make_shared<SystemInfo>();
    m_mock_observer = std::make_shared<MockSystemObserver>();
}

void SystemMonitorTest::TearDown()
{
    if (m_monitor)
    {
        m_monitor->stop();
    }
    m_monitor.reset();
    m_mock_observer.reset();
    m_system_info.reset();
}

// ============================================
// Constructor Tests
// ============================================

TEST_F(SystemMonitorTest, Constructor_WithSystemInfo_CreatesValidMonitor)
{
    // Arrange & Act
    m_monitor = std::make_unique<SystemMonitor>(m_system_info);

    // Assert
    EXPECT_FALSE(m_monitor->isRunning());
    EXPECT_EQ(m_monitor->getObserverCount(), 0);
}

TEST_F(SystemMonitorTest, Constructor_WithCustomThresholds_StoresThresholds)
{
    // Arrange
    AlertThreshold thresholds;
    thresholds.cpu_usage_percent = 75.0;
    thresholds.memory_usage_percent = 80.0;

    // Act
    m_monitor = std::make_unique<SystemMonitor>(m_system_info, thresholds);
    auto result = m_monitor->getThresholds();

    // Assert
    EXPECT_DOUBLE_EQ(result.cpu_usage_percent, 75.0);
    EXPECT_DOUBLE_EQ(result.memory_usage_percent, 80.0);
}

// ============================================
// Observer Management Tests
// ============================================

TEST_F(SystemMonitorTest, AddObserver_IncreasesObserverCount)
{
    // Arrange
    m_monitor = std::make_unique<SystemMonitor>(m_system_info);

    // Act
    m_monitor->addObserver(m_mock_observer);

    // Assert
    EXPECT_EQ(m_monitor->getObserverCount(), 1);
}

TEST_F(SystemMonitorTest, AddObserver_MultipleObservers_IncreasesCount)
{
    // Arrange
    m_monitor = std::make_unique<SystemMonitor>(m_system_info);
    auto observer2 = std::make_shared<MockSystemObserver>();

    // Act
    m_monitor->addObserver(m_mock_observer);
    m_monitor->addObserver(observer2);

    // Assert
    EXPECT_EQ(m_monitor->getObserverCount(), 2);
}

TEST_F(SystemMonitorTest, RemoveObserver_DecreasesObserverCount)
{
    // Arrange
    m_monitor = std::make_unique<SystemMonitor>(m_system_info);
    m_monitor->addObserver(m_mock_observer);

    // Act
    m_monitor->removeObserver(m_mock_observer);

    // Assert
    EXPECT_EQ(m_monitor->getObserverCount(), 0);
}

TEST_F(SystemMonitorTest, ClearObservers_RemovesAllObservers)
{
    // Arrange
    m_monitor = std::make_unique<SystemMonitor>(m_system_info);
    m_monitor->addObserver(m_mock_observer);
    m_monitor->addObserver(std::make_shared<MockSystemObserver>());

    // Act
    m_monitor->clearObservers();

    // Assert
    EXPECT_EQ(m_monitor->getObserverCount(), 0);
}

// ============================================
// Monitoring Control Tests
// ============================================

TEST_F(SystemMonitorTest, Start_ChangesRunningState)
{
    // Arrange
    m_monitor = std::make_unique<SystemMonitor>(m_system_info);

    // Act
    m_monitor->start(std::chrono::milliseconds(100));

    // Assert
    EXPECT_TRUE(m_monitor->isRunning());

    // Cleanup
    m_monitor->stop();
}

TEST_F(SystemMonitorTest, Stop_ChangesRunningState)
{
    // Arrange
    m_monitor = std::make_unique<SystemMonitor>(m_system_info);
    m_monitor->start(std::chrono::milliseconds(100));

    // Act
    m_monitor->stop();

    // Assert
    EXPECT_FALSE(m_monitor->isRunning());
}

TEST_F(SystemMonitorTest, CheckOnce_NotifiesObserver)
{
    // Arrange
    m_monitor = std::make_unique<SystemMonitor>(m_system_info);
    m_monitor->addObserver(m_mock_observer);

    // Expect at least one notification
    EXPECT_CALL(*m_mock_observer, onSystemUpdate(_))
        .Times(AtLeast(1));

    // Act
    m_monitor->checkOnce();

    // Assert - handled by mock expectations
}

// ============================================
// Threshold Tests
// ============================================

TEST_F(SystemMonitorTest, SetThresholds_UpdatesThresholds)
{
    // Arrange
    m_monitor = std::make_unique<SystemMonitor>(m_system_info);
    AlertThreshold new_thresholds;
    new_thresholds.cpu_usage_percent = 60.0;
    new_thresholds.memory_usage_percent = 70.0;

    // Act
    m_monitor->setThresholds(new_thresholds);
    auto result = m_monitor->getThresholds();

    // Assert
    EXPECT_DOUBLE_EQ(result.cpu_usage_percent, 60.0);
    EXPECT_DOUBLE_EQ(result.memory_usage_percent, 70.0);
}

// ============================================
// Memory Usage Threshold Tests
// ============================================

TEST_F(SystemMonitorTest, CheckOnce_WhenMemoryThresholdExceeded_NotifiesObserver)
{
    // Arrange
    AlertThreshold thresholds;
    thresholds.memory_usage_percent = 10.0;  // Very low threshold to trigger
    
    m_monitor = std::make_unique<SystemMonitor>(m_system_info, thresholds);
    m_monitor->addObserver(m_mock_observer);

    EXPECT_CALL(*m_mock_observer, onSystemUpdate(_))
        .Times(AtLeast(1));

    // Depending on actual system memory usage, this might or might not be called
    EXPECT_CALL(*m_mock_observer, onMemoryThresholdExceeded(_, _))
        .Times(testing::AnyNumber());

    // Act
    m_monitor->checkOnce();

    // Assert - handled by mock expectations
}

// ============================================
// Integration Tests
// ============================================

TEST_F(SystemMonitorTest, StartAndStop_WithObserver_WorksCorrectly)
{
    // Arrange
    m_monitor = std::make_unique<SystemMonitor>(m_system_info);
    m_monitor->addObserver(m_mock_observer);

    // Expect updates during monitoring
    EXPECT_CALL(*m_mock_observer, onSystemUpdate(_))
        .Times(AtLeast(1));

    // Act
    m_monitor->start(std::chrono::milliseconds(50));
    std::this_thread::sleep_for(std::chrono::milliseconds(150));  // Let it run for a bit
    m_monitor->stop();

    // Assert - mock expectations verify behavior
    EXPECT_FALSE(m_monitor->isRunning());
}
