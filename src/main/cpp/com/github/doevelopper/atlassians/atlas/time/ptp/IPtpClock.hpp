#ifndef FD898767_D6FA_446A_BB4E_1A6DD32326D3
#define FD898767_D6FA_446A_BB4E_1A6DD32326D3
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_IPTPCLOCK_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_IPTPCLOCK_HPP

#include <memory>
#include <string>

#include <com/github/doevelopper/atlassians/atlas/time/ptp/PtpTimestamp.hpp>
#include <com/github/doevelopper/atlassians/atlas/time/ptp/PtpTypes.hpp>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
class IPtpClock
{
public:
    virtual ~IPtpClock() = default;

    virtual PtpTimestamp getCurrentTime() const = 0;
    virtual void adjustTime(const TimeOffset &offset) = 0;
    virtual void adjustFrequency(const FrequencyOffset &offset) = 0;
    virtual std::string getName() const = 0;
    virtual bool isMasterCapable() const = 0;

    static std::unique_ptr<IPtpClock> create(const ClockConfig &config);
};

class OrdinaryClock final : public IPtpClock
{
public:
    explicit OrdinaryClock(ClockConfig config);

    PtpTimestamp getCurrentTime() const override;
    void adjustTime(const TimeOffset &offset) override;
    void adjustFrequency(const FrequencyOffset &offset) override;
    std::string getName() const override;
    bool isMasterCapable() const override;

private:
    ClockConfig m_config;
    TimeOffset m_timeOffset;
};

class BoundaryClock final : public IPtpClock
{
public:
    explicit BoundaryClock(ClockConfig config);

    PtpTimestamp getCurrentTime() const override;
    void adjustTime(const TimeOffset &offset) override;
    void adjustFrequency(const FrequencyOffset &offset) override;
    std::string getName() const override;
    bool isMasterCapable() const override;

    void setPortCount(std::size_t portCount);
    std::size_t getPortCount() const;

private:
    ClockConfig m_config;
    TimeOffset m_timeOffset;
    std::size_t m_portCount;
};

class TransparentClock final : public IPtpClock
{
public:
    explicit TransparentClock(ClockConfig config);

    PtpTimestamp getCurrentTime() const override;
    void adjustTime(const TimeOffset &offset) override;
    void adjustFrequency(const FrequencyOffset &offset) override;
    std::string getName() const override;
    bool isMasterCapable() const override;

    TransparentClockMode getMode() const;

private:
    ClockConfig m_config;
    TimeOffset m_timeOffset;
};
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_IPTPCLOCK_HPP


#endif /* FD898767_D6FA_446A_BB4E_1A6DD32326D3 */
