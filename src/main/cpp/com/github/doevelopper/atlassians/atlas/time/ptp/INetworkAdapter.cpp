#include <com/github/doevelopper/atlassians/atlas/time/ptp/INetworkAdapter.hpp>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
InMemoryNetworkAdapter::InMemoryNetworkAdapter()
    : m_frames{}
{
}

void InMemoryNetworkAdapter::sendMessage(const PtpMessage &message)
{
    m_frames.push(message.serialize());
}

std::optional<std::unique_ptr<PtpMessage>> InMemoryNetworkAdapter::receiveMessage(const std::chrono::milliseconds timeout)
{
    (void)timeout;

    if (m_frames.empty())
    {
        return std::nullopt;
    }

    auto frame = std::move(m_frames.front());
    m_frames.pop();
    return PtpMessage::createFromData(frame);
}

PtpTimestamp InMemoryNetworkAdapter::getHardwareTimestamp(const PtpMessage &message) const
{
    (void)message;
    return PtpTimestamp::now();
}
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp
