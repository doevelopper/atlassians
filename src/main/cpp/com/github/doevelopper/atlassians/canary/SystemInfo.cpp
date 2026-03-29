#include "SystemInfo.hpp"
#include "ProcCpuInfoProvider.hpp"
#include "ProcMemInfoProvider.hpp"
#include "ProcLoadAvgProvider.hpp"
#include "ProcUptimeProvider.hpp"

namespace com::github::doevelopper::atlassians::canary
{
    log4cxx::LoggerPtr SystemInfo::logger =
        log4cxx::Logger::getLogger("com.github.doevelopper.premisses.canary.SystemInfo");

    struct SystemInfo::Impl
    {
        std::unique_ptr<ICpuInfoProvider> cpu_provider;
        std::unique_ptr<IMemoryInfoProvider> memory_provider;
        std::unique_ptr<ILoadAverageProvider> load_provider;
        std::unique_ptr<IUptimeProvider> uptime_provider;

        Impl(
            std::unique_ptr<ICpuInfoProvider> cpu,
            std::unique_ptr<IMemoryInfoProvider> memory,
            std::unique_ptr<ILoadAverageProvider> load,
            std::unique_ptr<IUptimeProvider> uptime
        )
            : cpu_provider(std::move(cpu))
            , memory_provider(std::move(memory))
            , load_provider(std::move(load))
            , uptime_provider(std::move(uptime))
        {
        }
    };

    SystemInfo::SystemInfo()
        : pImpl(std::make_unique<Impl>(
            std::make_unique<ProcCpuInfoProvider>(),
            std::make_unique<ProcMemInfoProvider>(),
            std::make_unique<ProcLoadAvgProvider>(),
            std::make_unique<ProcUptimeProvider>()
        ))
    {
        LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
        LOG4CXX_INFO(logger, "SystemInfo initialized with default providers");
    }

    SystemInfo::SystemInfo(
        std::unique_ptr<ICpuInfoProvider> cpu_provider,
        std::unique_ptr<IMemoryInfoProvider> memory_provider,
        std::unique_ptr<ILoadAverageProvider> load_provider,
        std::unique_ptr<IUptimeProvider> uptime_provider
    )
        : pImpl(std::make_unique<Impl>(
            std::move(cpu_provider),
            std::move(memory_provider),
            std::move(load_provider),
            std::move(uptime_provider)
        ))
    {
        LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
        LOG4CXX_INFO(logger, "SystemInfo initialized with custom providers");
    }

    SystemInfo::~SystemInfo() = default;

    SystemInfo::SystemInfo(SystemInfo&&) noexcept = default;

    auto SystemInfo::operator=(SystemInfo&&) noexcept -> SystemInfo& = default;

    auto SystemInfo::getSystemSnapshot() const noexcept -> std::optional<SystemSnapshot>
    {
        try
        {
            SystemSnapshot snapshot;

            // Gather all system information
            if (pImpl->cpu_provider)
            {
                auto cpu_info = pImpl->cpu_provider->getCpuInfo();
                if (cpu_info.has_value())
                {
                    snapshot.cpu_info = cpu_info.value();
                }
            }

            if (pImpl->memory_provider)
            {
                auto mem_info = pImpl->memory_provider->getMemoryInfo();
                if (mem_info.has_value())
                {
                    snapshot.memory_info = mem_info.value();
                }
            }

            if (pImpl->load_provider)
            {
                auto load_avg = pImpl->load_provider->getLoadAverage();
                if (load_avg.has_value())
                {
                    snapshot.load_average = load_avg.value();
                }
            }

            if (pImpl->uptime_provider)
            {
                auto uptime = pImpl->uptime_provider->getUptime();
                if (uptime.has_value())
                {
                    snapshot.uptime = uptime.value();
                }
            }

            LOG4CXX_DEBUG(logger, "System snapshot created successfully");
            return snapshot;
        }
        catch (const std::exception& ex)
        {
            LOG4CXX_ERROR(logger, "Failed to create system snapshot: " << ex.what());
            return std::nullopt;
        }
    }

    auto SystemInfo::getCpuInfo() const noexcept -> std::optional<std::vector<CpuInfo>>
    {
        if (!pImpl || !pImpl->cpu_provider)
        {
            return std::nullopt;
        }
        return pImpl->cpu_provider->getCpuInfo();
    }

    auto SystemInfo::getMemoryInfo() const noexcept -> std::optional<MemoryInfo>
    {
        if (!pImpl || !pImpl->memory_provider)
        {
            return std::nullopt;
        }
        return pImpl->memory_provider->getMemoryInfo();
    }

    auto SystemInfo::getLoadAverage() const noexcept -> std::optional<LoadAverage>
    {
        if (!pImpl || !pImpl->load_provider)
        {
            return std::nullopt;
        }
        return pImpl->load_provider->getLoadAverage();
    }

    auto SystemInfo::getUptime() const noexcept -> std::optional<SystemUptime>
    {
        if (!pImpl || !pImpl->uptime_provider)
        {
            return std::nullopt;
        }
        return pImpl->uptime_provider->getUptime();
    }

    auto SystemInfo::areProvidersAvailable() const noexcept -> bool
    {
        if (!pImpl)
        {
            return false;
        }

        bool all_available = true;

        if (pImpl->cpu_provider)
        {
            all_available &= pImpl->cpu_provider->isAvailable();
        }
        if (pImpl->memory_provider)
        {
            all_available &= pImpl->memory_provider->isAvailable();
        }
        if (pImpl->load_provider)
        {
            all_available &= pImpl->load_provider->isAvailable();
        }
        if (pImpl->uptime_provider)
        {
            all_available &= pImpl->uptime_provider->isAvailable();
        }

        return all_available;
    }

    auto SystemInfo::validateProviders() const noexcept -> bool
    {
        if (!pImpl)
        {
            return false;
        }

        bool all_valid = true;

        if (pImpl->cpu_provider)
        {
            all_valid &= pImpl->cpu_provider->validate();
        }
        if (pImpl->memory_provider)
        {
            all_valid &= pImpl->memory_provider->validate();
        }
        if (pImpl->load_provider)
        {
            all_valid &= pImpl->load_provider->validate();
        }
        if (pImpl->uptime_provider)
        {
            all_valid &= pImpl->uptime_provider->validate();
        }

        return all_valid;
    }

    auto SystemInfo::getUnavailableProviders() const noexcept -> std::vector<std::string>
    {
        std::vector<std::string> unavailable;

        if (!pImpl)
        {
            unavailable.push_back("SystemInfo not initialized");
            return unavailable;
        }

        if (pImpl->cpu_provider && !pImpl->cpu_provider->isAvailable())
        {
            unavailable.push_back(std::string(pImpl->cpu_provider->getName()));
        }
        if (pImpl->memory_provider && !pImpl->memory_provider->isAvailable())
        {
            unavailable.push_back(std::string(pImpl->memory_provider->getName()));
        }
        if (pImpl->load_provider && !pImpl->load_provider->isAvailable())
        {
            unavailable.push_back(std::string(pImpl->load_provider->getName()));
        }
        if (pImpl->uptime_provider && !pImpl->uptime_provider->isAvailable())
        {
            unavailable.push_back(std::string(pImpl->uptime_provider->getName()));
        }

        return unavailable;
    }

} // namespace com::github::doevelopper::atlassians::canary
