#include "ProcLoadAvgProvider.hpp"

#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace com::github::doevelopper::atlassians::canary
{
    log4cxx::LoggerPtr ProcLoadAvgProvider::logger =
        log4cxx::Logger::getLogger("com.github.doevelopper.premisses.canary.ProcLoadAvgProvider");

    ProcLoadAvgProvider::ProcLoadAvgProvider() noexcept
        : m_proc_path("/proc/loadavg")
    {
        LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
    }

    ProcLoadAvgProvider::ProcLoadAvgProvider(std::string proc_path) noexcept
        : m_proc_path(std::move(proc_path))
    {
        LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " - Custom path: " << m_proc_path);
    }

    auto ProcLoadAvgProvider::isAvailable() const noexcept -> bool
    {
        struct stat buffer;
        return (stat(m_proc_path.c_str(), &buffer) == 0);
    }

    auto ProcLoadAvgProvider::getName() const noexcept -> std::string_view
    {
        return "ProcLoadAvgProvider";
    }

    auto ProcLoadAvgProvider::validate() const noexcept -> bool
    {
        if (!isAvailable())
        {
            LOG4CXX_WARN(logger, "Load average file not accessible: " << m_proc_path);
            return false;
        }

        std::ifstream file(m_proc_path);
        if (!file.is_open())
        {
            LOG4CXX_ERROR(logger, "Failed to open load average file: " << m_proc_path);
            return false;
        }

        std::string line;
        bool has_content = static_cast<bool>(std::getline(file, line));
        return has_content && !line.empty();
    }

    auto ProcLoadAvgProvider::getLoadAverage() const noexcept -> std::optional<LoadAverage>
    {
        try
        {
            std::ifstream file(m_proc_path);
            if (!file.is_open())
            {
                LOG4CXX_ERROR(logger, "Failed to open: " << m_proc_path);
                return std::nullopt;
            }

            LoadAverage load;
            std::string process_info;
            char separator;

            // Format: 0.52 0.58 0.59 1/613 12345
            file >> load.load_1min >> load.load_5min >> load.load_15min >> process_info >> load.last_pid;

            // Parse "1/613" format
            std::istringstream iss(process_info);
            iss >> load.running_processes >> separator >> load.total_processes;

            LOG4CXX_INFO(logger, "Load average: " << load.load_1min << ", "
                               << load.load_5min << ", " << load.load_15min);
            return load;
        }
        catch (const std::exception& ex)
        {
            LOG4CXX_ERROR(logger, "Exception reading load average: " << ex.what());
            return std::nullopt;
        }
    }

} // namespace com::github::doevelopper::atlassians::canary
