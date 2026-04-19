#ifndef B6FD342D_BCC4_4845_9437_39F2397981CB
#define B6FD342D_BCC4_4845_9437_39F2397981CB
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPMESSAGE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPMESSAGE_HPP

#include <cstddef>
#include <memory>
#include <vector>

#include <com/github/doevelopper/atlassians/atlas/time/ptp/PtpTimestamp.hpp>
#include <com/github/doevelopper/atlassians/atlas/time/ptp/PtpTypes.hpp>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
class PtpMessage
{
public:
    virtual ~PtpMessage() = default;

    virtual MessageType getType() const = 0;
    virtual std::vector<std::byte> serialize() const = 0;
    virtual void deserialize(const std::vector<std::byte> &data) = 0;

    static std::unique_ptr<PtpMessage> createFromData(const std::vector<std::byte> &data);
};

class SyncMessage final : public PtpMessage
{
public:
    SyncMessage();
    explicit SyncMessage(PtpTimestamp originTimestamp);

    MessageType getType() const override;
    std::vector<std::byte> serialize() const override;
    void deserialize(const std::vector<std::byte> &data) override;

    PtpTimestamp getOriginTimestamp() const;

private:
    PtpTimestamp m_originTimestamp;
};

class AnnounceMessage final : public PtpMessage
{
public:
    AnnounceMessage();
    explicit AnnounceMessage(ClockIdentity identity);

    MessageType getType() const override;
    std::vector<std::byte> serialize() const override;
    void deserialize(const std::vector<std::byte> &data) override;

    ClockIdentity getClockIdentity() const;

private:
    ClockIdentity m_identity;
};
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPMESSAGE_HPP


#endif /* B6FD342D_BCC4_4845_9437_39F2397981CB */
