#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_PROCMEMINFOPROVIDER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_PROCMEMINFOPROVIDER_HPP

#include <string>

#include "ISystemInfoProvider.hpp"
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

/**
 * @brief Provider for memory information from /proc/meminfo
 *
 * Reads and parses /proc/meminfo to extract memory usage details.
 */

namespace com::github::doevelopper::atlassians::canary
{
    class ProcMemInfoProvider : public IMemoryInfoProvider
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        explicit ProcMemInfoProvider() noexcept;
        explicit ProcMemInfoProvider(std::string proc_path) noexcept;

        ~ProcMemInfoProvider() noexcept override = default;

        ProcMemInfoProvider(const ProcMemInfoProvider&) = delete;
        auto operator=(const ProcMemInfoProvider&) -> ProcMemInfoProvider& = delete;
        ProcMemInfoProvider(ProcMemInfoProvider&&) noexcept = default;
        auto operator=(ProcMemInfoProvider&&) noexcept -> ProcMemInfoProvider& = default;

        [[nodiscard]] auto isAvailable() const noexcept -> bool override;
        [[nodiscard]] auto getName() const noexcept -> std::string_view override;
        [[nodiscard]] auto validate() const noexcept -> bool override;

        [[nodiscard]] auto getMemoryInfo() const noexcept -> std::optional<MemoryInfo> override;

    private:
        std::string m_proc_path;

        [[nodiscard]] static auto parseMemoryValue(const std::string& value) noexcept -> std::uint64_t;
        [[nodiscard]] static auto trim(const std::string& str) noexcept -> std::string;
    };

} // namespace com::github::doevelopper::atlassians::canary

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_PROCMEMINFOPROVIDER_HPP
