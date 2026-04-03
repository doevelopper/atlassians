#ifndef D8B8F627_06B8_433F_AFDD_5E19FFAC5818
#define D8B8F627_06B8_433F_AFDD_5E19FFAC5818
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPTYPES_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPTYPES_HPP

#include <cstdint>
#include <string>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
enum class ClockType
{
    Ordinary,
    Boundary,
    Transparent
};

enum class TransparentClockMode
{
    EndToEnd,
    PeerToPeer
};

enum class MessageType : std::uint8_t
{
    Sync = 0x00,
    FollowUp = 0x08,
    DelayReq = 0x01,
    DelayResp = 0x09,
    Announce = 0x0B,
    Signaling = 0x0C,
    Management = 0x0D
};

enum class PtpState
{
    Initializing,
    Listening,
    Master,
    Slave,
    Faulty
};

struct TimeOffset
{
    std::int64_t nanoseconds{0};
};

struct FrequencyOffset
{
    double partsPerMillion{0.0};
};

struct ClockConfig
{
    ClockType type{ClockType::Ordinary};
    std::string name{"ptp-clock"};
    bool masterCapable{true};
    TransparentClockMode transparentMode{TransparentClockMode::EndToEnd};
};

struct NetworkConfig
{
    std::string transport{"memory"};
};

struct ClockIdentity
{
    std::uint8_t priority1{128};
    std::uint8_t clockClass{248};
    std::uint8_t clockAccuracy{0xFE};
    std::uint16_t offsetScaledLogVariance{0xFFFF};
    std::uint8_t priority2{128};
    std::string clockId{"00-00-00-00-00-00-00-00"};
};
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPTYPES_HPP


#endif /* D8B8F627_06B8_433F_AFDD_5E19FFAC5818 */
