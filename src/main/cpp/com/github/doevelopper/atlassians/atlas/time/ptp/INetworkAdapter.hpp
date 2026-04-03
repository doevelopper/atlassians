#ifndef E59EAFE3_7A3B_49F3_8B82_3937022AAADE
#define E59EAFE3_7A3B_49F3_8B82_3937022AAADE
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_INETWORKADAPTER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_INETWORKADAPTER_HPP

#include <chrono>
#include <memory>
#include <optional>
#include <queue>

#include <com/github/doevelopper/atlassians/atlas/time/ptp/PtpMessage.hpp>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
class INetworkAdapter
{
public:
    virtual ~INetworkAdapter() = default;

    virtual void sendMessage(const PtpMessage &message) = 0;
    virtual std::optional<std::unique_ptr<PtpMessage>> receiveMessage(std::chrono::milliseconds timeout) = 0;
    virtual PtpTimestamp getHardwareTimestamp(const PtpMessage &message) const = 0;
};

class InMemoryNetworkAdapter final : public INetworkAdapter
{
public:
    InMemoryNetworkAdapter();

    void sendMessage(const PtpMessage &message) override;
    std::optional<std::unique_ptr<PtpMessage>> receiveMessage(std::chrono::milliseconds timeout) override;
    PtpTimestamp getHardwareTimestamp(const PtpMessage &message) const override;

private:
    std::queue<std::vector<std::byte>> m_frames;
};
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_INETWORKADAPTER_HPP


#endif /* E59EAFE3_7A3B_49F3_8B82_3937022AAADE */
