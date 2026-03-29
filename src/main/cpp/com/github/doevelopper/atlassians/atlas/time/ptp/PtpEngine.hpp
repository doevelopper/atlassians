#ifndef A0472C8C_05EC_47C1_8D67_017FEE0FD93A
#define A0472C8C_05EC_47C1_8D67_017FEE0FD93A
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPENGINE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPENGINE_HPP

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include <com/github/doevelopper/atlassians/atlas/time/ptp/Bmca.hpp>
#include <com/github/doevelopper/atlassians/atlas/time/ptp/PtpStateMachine.hpp>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
class PtpEngine
{
public:
    PtpEngine(std::unique_ptr<IPtpClock> clock,
              std::unique_ptr<INetworkAdapter> network,
              std::unique_ptr<IBmcaStrategy> bmca);

    void start();
    void stop();
    void onMessage(const PtpMessage &message);
    void tick();

    std::optional<ClockIdentity> electGrandmaster(const std::vector<ClockIdentity> &candidates) const;
    std::string currentStateName() const;

private:
    std::unique_ptr<IPtpClock> m_clock;
    std::unique_ptr<INetworkAdapter> m_network;
    std::unique_ptr<IBmcaStrategy> m_bmca;
    PtpStateMachine m_stateMachine;
};

class PtpEngineBuilder
{
public:
    PtpEngineBuilder &withClock(std::unique_ptr<IPtpClock> clock);
    PtpEngineBuilder &withNetwork(std::unique_ptr<INetworkAdapter> network);
    PtpEngineBuilder &withBmca(std::unique_ptr<IBmcaStrategy> bmca);

    std::unique_ptr<PtpEngine> build();

private:
    std::unique_ptr<IPtpClock> m_clock;
    std::unique_ptr<INetworkAdapter> m_network;
    std::unique_ptr<IBmcaStrategy> m_bmca;
};
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPENGINE_HPP


#endif /* A0472C8C_05EC_47C1_8D67_017FEE0FD93A */
