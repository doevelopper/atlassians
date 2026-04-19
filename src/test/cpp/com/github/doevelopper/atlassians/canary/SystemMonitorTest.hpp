#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMMONITORTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMMONITORTEST_HPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <com/github/doevelopper/atlassians/canary/SystemMonitor.hpp>
#include <com/github/doevelopper/atlassians/canary/ISystemObserver.hpp>

namespace com::github::doevelopper::atlassians::canary::test
{
    /**
     * @brief Mock observer for testing
     */
    class MockSystemObserver : public ISystemObserver
    {
    public:
        MOCK_METHOD(void, onSystemUpdate, (const SystemSnapshot&), (override));
        MOCK_METHOD(void, onCpuThresholdExceeded, (double, double), (override));
        MOCK_METHOD(void, onMemoryThresholdExceeded, (double, double), (override));
        MOCK_METHOD(void, onSwapThresholdExceeded, (double, double), (override));
        MOCK_METHOD(void, onLoadAverageExceeded, (double, double), (override));
        MOCK_METHOD(void, onMonitoringError, (const std::string&), (override));
    };

    /**
     * @brief Test fixture for SystemMonitor unit tests
     */
    class SystemMonitorTest : public ::testing::Test
    {
    protected:
        void SetUp() override;
        void TearDown() override;

        std::shared_ptr<SystemInfo> m_system_info;
        std::unique_ptr<SystemMonitor> m_monitor;
        std::shared_ptr<MockSystemObserver> m_mock_observer;
    };

} // namespace com::github::doevelopper::atlassians::canary::test

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMMONITORTEST_HPP
