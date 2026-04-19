#include <com/github/doevelopper/atlassians/atlas/time/ptp/PtpMessage.hpp>

#include <stdexcept>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
namespace
{
std::vector<std::byte> serializeTypeAndPayload(const MessageType type, const std::vector<std::byte> &payload)
{
    std::vector<std::byte> data;
    data.reserve(payload.size() + 1U);
    data.push_back(static_cast<std::byte>(type));
    data.insert(data.end(), payload.begin(), payload.end());
    return data;
}

std::vector<std::byte> payloadView(const std::vector<std::byte> &data)
{
    if (data.size() < 1U)
    {
        throw std::invalid_argument("Invalid PTP frame");
    }
    return std::vector<std::byte>(data.begin() + 1, data.end());
}
} // namespace

std::unique_ptr<PtpMessage> PtpMessage::createFromData(const std::vector<std::byte> &data)
{
    if (data.empty())
    {
        throw std::invalid_argument("Cannot decode empty PTP frame");
    }

    const auto type = static_cast<MessageType>(data.front());
    std::unique_ptr<PtpMessage> message;

    switch (type)
    {
    case MessageType::Sync:
        message = std::make_unique<SyncMessage>();
        break;
    case MessageType::Announce:
        message = std::make_unique<AnnounceMessage>();
        break;
    default:
        throw std::invalid_argument("Unsupported PTP message type");
    }

    message->deserialize(data);
    return message;
}

SyncMessage::SyncMessage()
    : m_originTimestamp(PtpTimestamp::now())
{
}

SyncMessage::SyncMessage(PtpTimestamp originTimestamp)
    : m_originTimestamp(std::move(originTimestamp))
{
}

MessageType SyncMessage::getType() const
{
    return MessageType::Sync;
}

std::vector<std::byte> SyncMessage::serialize() const
{
    return serializeTypeAndPayload(getType(), m_originTimestamp.serialize());
}

void SyncMessage::deserialize(const std::vector<std::byte> &data)
{
    if (data.empty() || static_cast<MessageType>(data.front()) != MessageType::Sync)
    {
        throw std::invalid_argument("Not a Sync message");
    }
    m_originTimestamp = PtpTimestamp::deserialize(payloadView(data));
}

PtpTimestamp SyncMessage::getOriginTimestamp() const
{
    return m_originTimestamp;
}

AnnounceMessage::AnnounceMessage()
    : m_identity{}
{
}

AnnounceMessage::AnnounceMessage(ClockIdentity identity)
    : m_identity(std::move(identity))
{
}

MessageType AnnounceMessage::getType() const
{
    return MessageType::Announce;
}

std::vector<std::byte> AnnounceMessage::serialize() const
{
    std::vector<std::byte> payload;
    payload.reserve(6U + m_identity.clockId.size());
    payload.push_back(static_cast<std::byte>(m_identity.priority1));
    payload.push_back(static_cast<std::byte>(m_identity.clockClass));
    payload.push_back(static_cast<std::byte>(m_identity.clockAccuracy));
    payload.push_back(static_cast<std::byte>((m_identity.offsetScaledLogVariance >> 8U) & 0xFFU));
    payload.push_back(static_cast<std::byte>(m_identity.offsetScaledLogVariance & 0xFFU));
    payload.push_back(static_cast<std::byte>(m_identity.priority2));
    for (const char c : m_identity.clockId)
    {
        payload.push_back(static_cast<std::byte>(static_cast<unsigned char>(c)));
    }

    return serializeTypeAndPayload(getType(), payload);
}

void AnnounceMessage::deserialize(const std::vector<std::byte> &data)
{
    if (data.size() < 7U || static_cast<MessageType>(data.front()) != MessageType::Announce)
    {
        throw std::invalid_argument("Not an Announce message");
    }

    const auto payload = payloadView(data);
    m_identity.priority1 = static_cast<std::uint8_t>(payload[0]);
    m_identity.clockClass = static_cast<std::uint8_t>(payload[1]);
    m_identity.clockAccuracy = static_cast<std::uint8_t>(payload[2]);
    m_identity.offsetScaledLogVariance = static_cast<std::uint16_t>(
        (static_cast<std::uint16_t>(payload[3]) << 8U) |
        static_cast<std::uint16_t>(payload[4]));
    m_identity.priority2 = static_cast<std::uint8_t>(payload[5]);

    m_identity.clockId.clear();
    for (std::size_t index = 6U; index < payload.size(); ++index)
    {
        m_identity.clockId.push_back(static_cast<char>(payload[index]));
    }
}

ClockIdentity AnnounceMessage::getClockIdentity() const
{
    return m_identity;
}
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp
