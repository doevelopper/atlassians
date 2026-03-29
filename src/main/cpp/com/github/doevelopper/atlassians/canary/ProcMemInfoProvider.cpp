#include "ProcMemInfoProvider.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <sys/stat.h>

namespace com::github::doevelopper::atlassians::canary
{
    log4cxx::LoggerPtr ProcMemInfoProvider::logger =
        log4cxx::Logger::getLogger("com.github.doevelopper.premisses.canary.ProcMemInfoProvider");

    ProcMemInfoProvider::ProcMemInfoProvider() noexcept
        : m_proc_path("/proc/meminfo")
    {
        LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
    }

    ProcMemInfoProvider::ProcMemInfoProvider(std::string proc_path) noexcept
        : m_proc_path(std::move(proc_path))
    {
        LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " - Custom path: " << m_proc_path);
    }

    auto ProcMemInfoProvider::isAvailable() const noexcept -> bool
    {
        struct stat buffer;
        return (stat(m_proc_path.c_str(), &buffer) == 0);
    }

    auto ProcMemInfoProvider::getName() const noexcept -> std::string_view
    {
        return "ProcMemInfoProvider";
    }

    auto ProcMemInfoProvider::validate() const noexcept -> bool
    {
        if (!isAvailable())
        {
            LOG4CXX_WARN(logger, "Memory info file not accessible: " << m_proc_path);
            return false;
        }

        std::ifstream file(m_proc_path);
        if (!file.is_open())
        {
            LOG4CXX_ERROR(logger, "Failed to open memory info file: " << m_proc_path);
            return false;
        }

        std::string line;
        bool has_content = static_cast<bool>(std::getline(file, line));
        return has_content && !line.empty();
    }

    auto ProcMemInfoProvider::getMemoryInfo() const noexcept -> std::optional<MemoryInfo>
    {
        try
        {
            std::ifstream file(m_proc_path);
            if (!file.is_open())
            {
                LOG4CXX_ERROR(logger, "Failed to open: " << m_proc_path);
                return std::nullopt;
            }

            MemoryInfo info;
            std::string line;

            while (std::getline(file, line))
            {
                auto pos = line.find(':');
                if (pos == std::string::npos)
                {
                    continue;
                }

                auto key = trim(line.substr(0, pos));
                auto value = trim(line.substr(pos + 1));

                if (key == "MemTotal")
                {
                    info.total_ram_kb = parseMemoryValue(value);
                }
                else if (key == "MemFree")
                {
                    info.free_ram_kb = parseMemoryValue(value);
                }
                else if (key == "MemAvailable")
                {
                    info.available_ram_kb = parseMemoryValue(value);
                }
                else if (key == "Buffers")
                {
                    info.buffers_kb = parseMemoryValue(value);
                }
                else if (key == "Cached")
                {
                    info.cached_kb = parseMemoryValue(value);
                }
                else if (key == "SwapCached")
                {
                    info.swap_cached_kb = parseMemoryValue(value);
                }
                else if (key == "SwapTotal")
                {
                    info.swap_total_kb = parseMemoryValue(value);
                }
                else if (key == "SwapFree")
                {
                    info.swap_free_kb = parseMemoryValue(value);
                }
                else if (key == "Dirty")
                {
                    info.dirty_kb = parseMemoryValue(value);
                }
                else if (key == "Writeback")
                {
                    info.writeback_kb = parseMemoryValue(value);
                }
                else if (key == "Slab")
                {
                    info.slab_kb = parseMemoryValue(value);
                }
            }

            LOG4CXX_INFO(logger, "Memory info: Total=" << info.total_ram_kb
                                << "KB, Available=" << info.available_ram_kb << "KB");
            return info;
        }
        catch (const std::exception& ex)
        {
            LOG4CXX_ERROR(logger, "Exception reading memory info: " << ex.what());
            return std::nullopt;
        }
    }

    auto ProcMemInfoProvider::parseMemoryValue(const std::string& value) noexcept -> std::uint64_t
    {
        try
        {
            // Parse "123456 kB" format
            std::istringstream iss(value);
            std::uint64_t num;
            iss >> num;
            return num;
        }
        catch (...)
        {
            return 0;
        }
    }

    auto ProcMemInfoProvider::trim(const std::string& str) noexcept -> std::string
    {
        auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
            return std::isspace(ch);
        });
        auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
            return std::isspace(ch);
        }).base();

        return (start < end) ? std::string(start, end) : std::string();
    }

} // namespace com::github::doevelopper::atlassians::canary
