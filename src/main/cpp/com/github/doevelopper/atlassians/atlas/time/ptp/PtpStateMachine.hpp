#ifndef D966ED7F_0E45_474C_B09A_BC6A6C42BEF3
#define D966ED7F_0E45_474C_B09A_BC6A6C42BEF3
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPSTATEMACHINE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPSTATEMACHINE_HPP

#include <memory>
#include <string>

#include <com/github/doevelopper/atlassians/atlas/time/ptp/INetworkAdapter.hpp>
#include <com/github/doevelopper/atlassians/atlas/time/ptp/IPtpClock.hpp>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
class PtpStateMachine;

class IPtpState
{
public:
    virtual ~IPtpState() = default;

    virtual void enter(PtpStateMachine &context) = 0;
    virtual void exit(PtpStateMachine &context) = 0;
    virtual void processMessage(PtpStateMachine &context, const PtpMessage &message) = 0;
    virtual void run(PtpStateMachine &context) = 0;
    virtual std::string name() const = 0;
};

class PtpStateMachine
{
public:
    PtpStateMachine(IPtpClock &clock, INetworkAdapter &network);

    void processMessage(const PtpMessage &message);
    void run();
    void changeState(PtpState newState);

    IPtpClock &clock();
    INetworkAdapter &network();
    PtpState getState() const;
    std::string getStateName() const;

private:
    static std::unique_ptr<IPtpState> createState(PtpState state);

    IPtpClock &m_clock;
    INetworkAdapter &m_network;
    PtpState m_state;
    std::unique_ptr<IPtpState> m_current;
};
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPSTATEMACHINE_HPP


#endif /* D966ED7F_0E45_474C_B09A_BC6A6C42BEF3 */
