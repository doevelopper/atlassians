#ifndef D9F4A751_21E6_475E_ACC4_C86922B3F94B
#define D9F4A751_21E6_475E_ACC4_C86922B3F94B
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPTIMESTAMP_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPTIMESTAMP_HPP

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
class PtpTimestamp
{
public:
    PtpTimestamp() noexcept;
    PtpTimestamp(std::uint64_t seconds, std::uint32_t nanoseconds);

    std::uint64_t getSeconds() const noexcept;
    std::uint32_t getNanoseconds() const noexcept;

    std::chrono::system_clock::time_point toSystemTime() const;

    std::vector<std::byte> serialize() const;
    static PtpTimestamp deserialize(const std::vector<std::byte> &data);
    static PtpTimestamp now();

    bool operator==(const PtpTimestamp &other) const noexcept;
    bool operator!=(const PtpTimestamp &other) const noexcept;
    bool operator<(const PtpTimestamp &other) const noexcept;
    bool operator<=(const PtpTimestamp &other) const noexcept;
    bool operator>(const PtpTimestamp &other) const noexcept;
    bool operator>=(const PtpTimestamp &other) const noexcept;

    std::string toString() const;

private:
    void normalize() noexcept;

    std::uint64_t m_seconds;
    std::uint32_t m_nanoseconds;
};
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPTIMESTAMP_HPP


#endif /* D9F4A751_21E6_475E_ACC4_C86922B3F94B */
