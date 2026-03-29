#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMINFOTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMINFOTEST_HPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <com/github/doevelopper/premisses/canary/SystemInfo.hpp>
#include <com/github/doevelopper/premisses/canary/ISystemInfoProvider.hpp>

namespace com::github::doevelopper::premisses::canary::test
{
    /**
     * @brief Mock CPU info provider for testing
     */
    class MockCpuInfoProvider : public ICpuInfoProvider
    {
    public:
        MOCK_METHOD(bool, isAvailable, (), (const, noexcept, override));
        MOCK_METHOD(std::string_view, getName, (), (const, noexcept, override));
        MOCK_METHOD(bool, validate, (), (const, noexcept, override));
        MOCK_METHOD((std::optional<std::vector<CpuInfo>>), getCpuInfo, (), (const, noexcept, override));
        MOCK_METHOD((std::optional<std::uint32_t>), getCoreCount, (), (const, noexcept, override));
    };

    /**
     * @brief Mock memory info provider for testing
     */
    class MockMemoryInfoProvider : public IMemoryInfoProvider
    {
    public:
        MOCK_METHOD(bool, isAvailable, (), (const, noexcept, override));
        MOCK_METHOD(std::string_view, getName, (), (const, noexcept, override));
        MOCK_METHOD(bool, validate, (), (const, noexcept, override));
        MOCK_METHOD((std::optional<MemoryInfo>), getMemoryInfo, (), (const, noexcept, override));
    };

    /**
     * @brief Mock load average provider for testing
     */
    class MockLoadAverageProvider : public ILoadAverageProvider
    {
    public:
        MOCK_METHOD(bool, isAvailable, (), (const, noexcept, override));
        MOCK_METHOD(std::string_view, getName, (), (const, noexcept, override));
        MOCK_METHOD(bool, validate, (), (const, noexcept, override));
        MOCK_METHOD((std::optional<LoadAverage>), getLoadAverage, (), (const, noexcept, override));
    };

    /**
     * @brief Mock uptime provider for testing
     */
    class MockUptimeProvider : public IUptimeProvider
    {
    public:
        MOCK_METHOD(bool, isAvailable, (), (const, noexcept, override));
        MOCK_METHOD(std::string_view, getName, (), (const, noexcept, override));
        MOCK_METHOD(bool, validate, (), (const, noexcept, override));
        MOCK_METHOD((std::optional<SystemUptime>), getUptime, (), (const, noexcept, override));
    };

    /**
     * @brief Test fixture for SystemInfo unit tests
     */
    class SystemInfoTest : public ::testing::Test
    {
    protected:
        void SetUp() override;
        void TearDown() override;

        std::unique_ptr<SystemInfo> m_system_info;
        std::unique_ptr<MockCpuInfoProvider> m_mock_cpu_provider;
        std::unique_ptr<MockMemoryInfoProvider> m_mock_memory_provider;
        std::unique_ptr<MockLoadAverageProvider> m_mock_load_provider;
        std::unique_ptr<MockUptimeProvider> m_mock_uptime_provider;
    };

} // namespace com::github::doevelopper::premisses::canary::test

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMINFOTEST_HPP
