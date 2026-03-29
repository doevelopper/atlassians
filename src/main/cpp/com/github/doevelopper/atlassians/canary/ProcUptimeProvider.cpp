#include "ProcUptimeProvider.hpp"

#include <fstream>
#include <sys/stat.h>

namespace com::github::doevelopper::atlassians::canary
{
    log4cxx::LoggerPtr ProcUptimeProvider::logger =
        log4cxx::Logger::getLogger("com.github.doevelopper.premisses.canary.ProcUptimeProvider");

    ProcUptimeProvider::ProcUptimeProvider() noexcept
        : m_proc_path("/proc/uptime")
    {
        LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
    }

    ProcUptimeProvider::ProcUptimeProvider(std::string proc_path) noexcept
        : m_proc_path(std::move(proc_path))
    {
        LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " - Custom path: " << m_proc_path);
    }

    auto ProcUptimeProvider::isAvailable() const noexcept -> bool
    {
        struct stat buffer;
        return (stat(m_proc_path.c_str(), &buffer) == 0);
    }

    auto ProcUptimeProvider::getName() const noexcept -> std::string_view
    {
        return "ProcUptimeProvider";
    }

    auto ProcUptimeProvider::validate() const noexcept -> bool
    {
        if (!isAvailable())
        {
            LOG4CXX_WARN(logger, "Uptime file not accessible: " << m_proc_path);
            return false;
        }

        std::ifstream file(m_proc_path);
        if (!file.is_open())
        {
            LOG4CXX_ERROR(logger, "Failed to open uptime file: " << m_proc_path);
            return false;
        }

        std::string line;
        bool has_content = static_cast<bool>(std::getline(file, line));
        return has_content && !line.empty();
    }

    auto ProcUptimeProvider::getUptime() const noexcept -> std::optional<SystemUptime>
    {
        try
        {
            std::ifstream file(m_proc_path);
            if (!file.is_open())
            {
                LOG4CXX_ERROR(logger, "Failed to open: " << m_proc_path);
                return std::nullopt;
            }

            SystemUptime uptime;

            // Format: 123456.78 234567.89
            file >> uptime.uptime_seconds >> uptime.idle_time_seconds;

            LOG4CXX_INFO(logger, "System uptime: " << uptime.uptime_seconds << " seconds");
            return uptime;
        }
        catch (const std::exception& ex)
        {
            LOG4CXX_ERROR(logger, "Exception reading uptime: " << ex.what());
            return std::nullopt;
        }
    }

} // namespace com::github::doevelopper::atlassians::canary
