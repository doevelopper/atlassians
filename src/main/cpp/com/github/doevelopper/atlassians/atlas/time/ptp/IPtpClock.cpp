#include <com/github/doevelopper/atlassians/atlas/time/ptp/IPtpClock.hpp>

#include <chrono>
#include <stdexcept>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
namespace
{
PtpTimestamp withOffset(const TimeOffset &offset)
{
    const auto nowTp = std::chrono::system_clock::now() + std::chrono::nanoseconds(offset.nanoseconds);
    const auto sinceEpoch = nowTp.time_since_epoch();
    const auto secs = std::chrono::duration_cast<std::chrono::seconds>(sinceEpoch);
    const auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(sinceEpoch - secs);
    return PtpTimestamp(static_cast<std::uint64_t>(secs.count()), static_cast<std::uint32_t>(nanos.count()));
}
} // namespace

std::unique_ptr<IPtpClock> IPtpClock::create(const ClockConfig &config)
{
    switch (config.type)
    {
    case ClockType::Ordinary:
        return std::make_unique<OrdinaryClock>(config);
    case ClockType::Boundary:
        return std::make_unique<BoundaryClock>(config);
    case ClockType::Transparent:
        return std::make_unique<TransparentClock>(config);
    default:
        throw std::invalid_argument("Unsupported clock type");
    }
}

OrdinaryClock::OrdinaryClock(ClockConfig config)
    : m_config(std::move(config)),
      m_timeOffset{}
{
}

PtpTimestamp OrdinaryClock::getCurrentTime() const
{
    return withOffset(m_timeOffset);
}

void OrdinaryClock::adjustTime(const TimeOffset &offset)
{
    m_timeOffset.nanoseconds += offset.nanoseconds;
}

void OrdinaryClock::adjustFrequency(const FrequencyOffset &offset)
{
    const auto correction = static_cast<std::int64_t>(offset.partsPerMillion * 1000.0);
    m_timeOffset.nanoseconds += correction;
}

std::string OrdinaryClock::getName() const
{
    return m_config.name;
}

bool OrdinaryClock::isMasterCapable() const
{
    return m_config.masterCapable;
}

BoundaryClock::BoundaryClock(ClockConfig config)
    : m_config(std::move(config)),
      m_timeOffset{},
      m_portCount(1U)
{
}

PtpTimestamp BoundaryClock::getCurrentTime() const
{
    return withOffset(m_timeOffset);
}

void BoundaryClock::adjustTime(const TimeOffset &offset)
{
    m_timeOffset.nanoseconds += offset.nanoseconds;
}

void BoundaryClock::adjustFrequency(const FrequencyOffset &offset)
{
    const auto correction = static_cast<std::int64_t>(offset.partsPerMillion * 1000.0);
    m_timeOffset.nanoseconds += correction;
}

std::string BoundaryClock::getName() const
{
    return m_config.name;
}

bool BoundaryClock::isMasterCapable() const
{
    return m_config.masterCapable;
}

void BoundaryClock::setPortCount(const std::size_t portCount)
{
    m_portCount = portCount;
}

std::size_t BoundaryClock::getPortCount() const
{
    return m_portCount;
}

TransparentClock::TransparentClock(ClockConfig config)
    : m_config(std::move(config)),
      m_timeOffset{}
{
}

PtpTimestamp TransparentClock::getCurrentTime() const
{
    return withOffset(m_timeOffset);
}

void TransparentClock::adjustTime(const TimeOffset &offset)
{
    m_timeOffset.nanoseconds += offset.nanoseconds;
}

void TransparentClock::adjustFrequency(const FrequencyOffset &offset)
{
    const auto correction = static_cast<std::int64_t>(offset.partsPerMillion * 1000.0);
    m_timeOffset.nanoseconds += correction;
}

std::string TransparentClock::getName() const
{
    return m_config.name;
}

bool TransparentClock::isMasterCapable() const
{
    return false;
}

TransparentClockMode TransparentClock::getMode() const
{
    return m_config.transparentMode;
}
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp
