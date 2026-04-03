#include "ProcCpuInfoProvider.hpp"

#include <sstream>
#include <algorithm>
#include <cctype>
#include <sys/stat.h>
#include <unistd.h>

namespace com::github::doevelopper::atlassians::canary
{
    log4cxx::LoggerPtr ProcCpuInfoProvider::logger =
        log4cxx::Logger::getLogger("com.github.doevelopper.premisses.canary.ProcCpuInfoProvider");

    ProcCpuInfoProvider::ProcCpuInfoProvider() noexcept
        : m_proc_path("/proc/cpuinfo")
    {
        LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
    }

    ProcCpuInfoProvider::ProcCpuInfoProvider(std::string proc_path) noexcept
        : m_proc_path(std::move(proc_path))
    {
        LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " - Custom path: " << m_proc_path);
    }

    auto ProcCpuInfoProvider::isAvailable() const noexcept -> bool
    {
        struct stat buffer;
        return (stat(m_proc_path.c_str(), &buffer) == 0);
    }

    auto ProcCpuInfoProvider::getName() const noexcept -> std::string_view
    {
        return "ProcCpuInfoProvider";
    }

    auto ProcCpuInfoProvider::validate() const noexcept -> bool
    {
        if (!isAvailable())
        {
            LOG4CXX_WARN(logger, "CPU info file not accessible: " << m_proc_path);
            return false;
        }

        // Try to read at least one line
        std::ifstream file(m_proc_path);
        if (!file.is_open())
        {
            LOG4CXX_ERROR(logger, "Failed to open CPU info file: " << m_proc_path);
            return false;
        }

        std::string line;
        bool has_content = static_cast<bool>(std::getline(file, line));
        return has_content && !line.empty();
    }

    auto ProcCpuInfoProvider::getCpuInfo() const noexcept -> std::optional<std::vector<CpuInfo>>
    {
        try
        {
            std::ifstream file(m_proc_path);
            if (!file.is_open())
            {
                LOG4CXX_ERROR(logger, "Failed to open: " << m_proc_path);
                return std::nullopt;
            }

            std::vector<CpuInfo> cpu_list;
            std::vector<std::string> current_cpu_lines;
            std::string line;

            while (std::getline(file, line))
            {
                if (line.empty())
                {
                    // Empty line marks end of CPU entry
                    if (!current_cpu_lines.empty())
                    {
                        auto cpu_info = parseCpuEntry(current_cpu_lines);
                        if (cpu_info.has_value())
                        {
                            cpu_list.push_back(cpu_info.value());
                        }
                        current_cpu_lines.clear();
                    }
                }
                else
                {
                    current_cpu_lines.push_back(line);
                }
            }

            // Handle last CPU entry if file doesn't end with empty line
            if (!current_cpu_lines.empty())
            {
                auto cpu_info = parseCpuEntry(current_cpu_lines);
                if (cpu_info.has_value())
                {
                    cpu_list.push_back(cpu_info.value());
                }
            }

            LOG4CXX_INFO(logger, "Successfully read info for " << cpu_list.size() << " CPU(s)");
            return cpu_list;
        }
        catch (const std::exception& ex)
        {
            LOG4CXX_ERROR(logger, "Exception reading CPU info: " << ex.what());
            return std::nullopt;
        }
    }

    auto ProcCpuInfoProvider::getCoreCount() const noexcept -> std::optional<std::uint32_t>
    {
        auto cpu_info = getCpuInfo();
        if (!cpu_info.has_value())
        {
            return std::nullopt;
        }

        // Count unique core IDs
        std::uint32_t max_cores = 0;
        for (const auto& cpu : cpu_info.value())
        {
            if (cpu.cpu_cores > max_cores)
            {
                max_cores = cpu.cpu_cores;
            }
        }

        return max_cores > 0 ? std::optional<std::uint32_t>(max_cores)
                             : std::optional<std::uint32_t>(static_cast<std::uint32_t>(cpu_info.value().size()));
    }

    auto ProcCpuInfoProvider::parseCpuEntry(const std::vector<std::string>& lines) const noexcept -> std::optional<CpuInfo>
    {
        try
        {
            CpuInfo info;

            for (const auto& line : lines)
            {
                auto pos = line.find(':');
                if (pos == std::string::npos)
                {
                    continue;
                }

                auto key = trim(line.substr(0, pos));
                auto value = trim(line.substr(pos + 1));

                if (key == "processor")
                {
                    info.processor_id = static_cast<std::uint32_t>(std::stoul(value));
                }
                else if (key == "vendor_id")
                {
                    info.vendor_id = value;
                }
                else if (key == "cpu family")
                {
                    info.cpu_family = static_cast<std::uint32_t>(std::stoul(value));
                }
                else if (key == "model")
                {
                    info.model = static_cast<std::uint32_t>(std::stoul(value));
                }
                else if (key == "model name")
                {
                    info.model_name = value;
                }
                else if (key == "stepping")
                {
                    info.stepping = static_cast<std::uint32_t>(std::stoul(value));
                }
                else if (key == "cpu MHz")
                {
                    info.cpu_mhz = std::stod(value);
                }
                else if (key == "cache size")
                {
                    // Parse "6144 KB" format
                    std::istringstream iss(value);
                    iss >> info.cache_size_kb;
                }
                else if (key == "physical id")
                {
                    info.physical_id = static_cast<std::uint32_t>(std::stoul(value));
                }
                else if (key == "siblings")
                {
                    info.siblings = static_cast<std::uint32_t>(std::stoul(value));
                }
                else if (key == "core id")
                {
                    info.core_id = static_cast<std::uint32_t>(std::stoul(value));
                }
                else if (key == "cpu cores")
                {
                    info.cpu_cores = static_cast<std::uint32_t>(std::stoul(value));
                }
                else if (key == "flags")
                {
                    info.flags = split(value, ' ');
                }
                else if (key == "bogomips")
                {
                    info.bogomips = value;
                }
            }

            return info;
        }
        catch (const std::exception& ex)
        {
            LOG4CXX_ERROR(logger, "Failed to parse CPU entry: " << ex.what());
            return std::nullopt;
        }
    }

    auto ProcCpuInfoProvider::trim(const std::string& str) noexcept -> std::string
    {
        auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
            return std::isspace(ch);
        });
        auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
            return std::isspace(ch);
        }).base();

        return (start < end) ? std::string(start, end) : std::string();
    }

    auto ProcCpuInfoProvider::split(const std::string& str, char delimiter) noexcept -> std::vector<std::string>
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream token_stream(str);

        while (std::getline(token_stream, token, delimiter))
        {
            auto trimmed = trim(token);
            if (!trimmed.empty())
            {
                tokens.push_back(trimmed);
            }
        }

        return tokens;
    }

} // namespace com::github::doevelopper::atlassians::canary
