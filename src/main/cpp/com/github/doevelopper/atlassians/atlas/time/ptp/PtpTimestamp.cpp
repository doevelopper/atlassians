#include <com/github/doevelopper/atlassians/atlas/time/ptp/PtpTimestamp.hpp>

#include <chrono>
#include <sstream>
#include <stdexcept>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
namespace
{
constexpr std::uint64_t kSecondsMask48 = (std::uint64_t{1} << 48U) - std::uint64_t{1};
constexpr std::uint32_t kOneSecondInNanos = 1000000000U;
} // namespace

PtpTimestamp::PtpTimestamp() noexcept
    : m_seconds(0U),
      m_nanoseconds(0U)
{
}

PtpTimestamp::PtpTimestamp(const std::uint64_t seconds, const std::uint32_t nanoseconds)
    : m_seconds(seconds),
      m_nanoseconds(nanoseconds)
{
    normalize();
}

std::uint64_t PtpTimestamp::getSeconds() const noexcept
{
    return m_seconds;
}

std::uint32_t PtpTimestamp::getNanoseconds() const noexcept
{
    return m_nanoseconds;
}

std::chrono::system_clock::time_point PtpTimestamp::toSystemTime() const
{
    const auto secs = std::chrono::seconds(static_cast<std::int64_t>(m_seconds));
    const auto nanos = std::chrono::nanoseconds(static_cast<std::int64_t>(m_nanoseconds));
    return std::chrono::system_clock::time_point(secs + nanos);
}

std::vector<std::byte> PtpTimestamp::serialize() const
{
    std::vector<std::byte> bytes(10U);

    for (std::size_t i = 0; i < 6U; ++i)
    {
        const auto shift = static_cast<unsigned>((5U - i) * 8U);
        bytes[i] = static_cast<std::byte>((m_seconds >> shift) & 0xFFU);
    }

    for (std::size_t i = 0; i < 4U; ++i)
    {
        const auto shift = static_cast<unsigned>((3U - i) * 8U);
        bytes[i + 6U] = static_cast<std::byte>((m_nanoseconds >> shift) & 0xFFU);
    }

    return bytes;
}

PtpTimestamp PtpTimestamp::deserialize(const std::vector<std::byte> &data)
{
    if (data.size() != 10U)
    {
        throw std::invalid_argument("PTP timestamp must contain exactly 10 bytes");
    }

    std::uint64_t seconds = 0U;
    std::uint32_t nanos = 0U;

    for (std::size_t i = 0; i < 6U; ++i)
    {
        seconds <<= 8U;
        seconds |= static_cast<std::uint64_t>(data[i]);
    }

    for (std::size_t i = 0; i < 4U; ++i)
    {
        nanos <<= 8U;
        nanos |= static_cast<std::uint32_t>(data[i + 6U]);
    }

    return PtpTimestamp(seconds, nanos);
}

PtpTimestamp PtpTimestamp::now()
{
    const auto nowTime = std::chrono::system_clock::now();
    const auto sinceEpoch = nowTime.time_since_epoch();
    const auto sec = std::chrono::duration_cast<std::chrono::seconds>(sinceEpoch);
    const auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(sinceEpoch - sec);

    return PtpTimestamp(static_cast<std::uint64_t>(sec.count()), static_cast<std::uint32_t>(nanos.count()));
}

bool PtpTimestamp::operator==(const PtpTimestamp &other) const noexcept
{
    return m_seconds == other.m_seconds && m_nanoseconds == other.m_nanoseconds;
}

bool PtpTimestamp::operator!=(const PtpTimestamp &other) const noexcept
{
    return !(*this == other);
}

bool PtpTimestamp::operator<(const PtpTimestamp &other) const noexcept
{
    return (m_seconds < other.m_seconds) || ((m_seconds == other.m_seconds) && (m_nanoseconds < other.m_nanoseconds));
}

bool PtpTimestamp::operator<=(const PtpTimestamp &other) const noexcept
{
    return (*this < other) || (*this == other);
}

bool PtpTimestamp::operator>(const PtpTimestamp &other) const noexcept
{
    return other < *this;
}

bool PtpTimestamp::operator>=(const PtpTimestamp &other) const noexcept
{
    return !(*this < other);
}

std::string PtpTimestamp::toString() const
{
    std::ostringstream stream;
    stream << m_seconds << "." << m_nanoseconds;
    return stream.str();
}

void PtpTimestamp::normalize() noexcept
{
    const auto carry = m_nanoseconds / kOneSecondInNanos;
    m_seconds += carry;
    m_nanoseconds %= kOneSecondInNanos;
    m_seconds &= kSecondsMask48;
}
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp
