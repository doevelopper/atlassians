#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_PROCCPUINFOPROVIDER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_PROCCPUINFOPROVIDER_HPP

#include <memory>
#include <string>
#include <fstream>

#include "ISystemInfoProvider.hpp"
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

/**
 * @brief Provider for CPU information from /proc/cpuinfo
 *
 * Reads and parses /proc/cpuinfo to extract CPU details.
 * Demonstrates:
 * - Strategy pattern (implements ISystemInfoProvider interface)
 * - RAII for file handling
 * - Single Responsibility Principle (only handles CPU info)
 */

namespace com::github::doevelopper::atlassians::canary
{
    class ProcCpuInfoProvider : public ICpuInfoProvider
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        /**
         * @brief Construct CPU info provider with default path
         */
        explicit ProcCpuInfoProvider() noexcept;

        /**
         * @brief Construct CPU info provider with custom path (for testing)
         * @param proc_path Path to proc filesystem
         */
        explicit ProcCpuInfoProvider(std::string proc_path) noexcept;

        ~ProcCpuInfoProvider() noexcept override = default;

        // Rule of five - no copying, allow moving
        ProcCpuInfoProvider(const ProcCpuInfoProvider&) = delete;
        auto operator=(const ProcCpuInfoProvider&) -> ProcCpuInfoProvider& = delete;
        ProcCpuInfoProvider(ProcCpuInfoProvider&&) noexcept = default;
        auto operator=(ProcCpuInfoProvider&&) noexcept -> ProcCpuInfoProvider& = default;

        // ISystemInfoProvider interface
        [[nodiscard]] auto isAvailable() const noexcept -> bool override;
        [[nodiscard]] auto getName() const noexcept -> std::string_view override;
        [[nodiscard]] auto validate() const noexcept -> bool override;

        // ICpuInfoProvider interface
        [[nodiscard]] auto getCpuInfo() const noexcept -> std::optional<std::vector<CpuInfo>> override;
        [[nodiscard]] auto getCoreCount() const noexcept -> std::optional<std::uint32_t> override;

    private:
        std::string m_proc_path;

        /**
         * @brief Parse a single CPU entry from proc data
         * @param lines Lines of text for one processor
         * @return Parsed CPU info
         */
        [[nodiscard]] auto parseCpuEntry(const std::vector<std::string>& lines) const noexcept -> std::optional<CpuInfo>;

        /**
         * @brief Trim whitespace from string
         */
        [[nodiscard]] static auto trim(const std::string& str) noexcept -> std::string;

        /**
         * @brief Split string by delimiter
         */
        [[nodiscard]] static auto split(const std::string& str, char delimiter) noexcept -> std::vector<std::string>;
    };

} // namespace com::github::doevelopper::atlassians::canary

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_PROCCPUINFOPROVIDER_HPP
