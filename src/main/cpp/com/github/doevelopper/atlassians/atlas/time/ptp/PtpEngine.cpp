#include <com/github/doevelopper/atlassians/atlas/time/ptp/PtpEngine.hpp>

#include <stdexcept>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
PtpEngine::PtpEngine(std::unique_ptr<IPtpClock> clock,
                     std::unique_ptr<INetworkAdapter> network,
                     std::unique_ptr<IBmcaStrategy> bmca)
    : m_clock(std::move(clock)),
      m_network(std::move(network)),
      m_bmca(std::move(bmca)),
      m_stateMachine(*m_clock, *m_network)
{
    if (!m_clock || !m_network || !m_bmca)
    {
        throw std::invalid_argument("PtpEngine requires clock, network, and BMCA strategy");
    }
}

void PtpEngine::start()
{
    m_stateMachine.changeState(PtpState::Listening);
}

void PtpEngine::stop()
{
    m_stateMachine.changeState(PtpState::Faulty);
}

void PtpEngine::onMessage(const PtpMessage &message)
{
    m_stateMachine.processMessage(message);
}

void PtpEngine::tick()
{
    m_stateMachine.run();
}

std::optional<ClockIdentity> PtpEngine::electGrandmaster(const std::vector<ClockIdentity> &candidates) const
{
    return m_bmca->selectBestMaster(candidates);
}

std::string PtpEngine::currentStateName() const
{
    return m_stateMachine.getStateName();
}

PtpEngineBuilder &PtpEngineBuilder::withClock(std::unique_ptr<IPtpClock> clock)
{
    m_clock = std::move(clock);
    return *this;
}

PtpEngineBuilder &PtpEngineBuilder::withNetwork(std::unique_ptr<INetworkAdapter> network)
{
    m_network = std::move(network);
    return *this;
}

PtpEngineBuilder &PtpEngineBuilder::withBmca(std::unique_ptr<IBmcaStrategy> bmca)
{
    m_bmca = std::move(bmca);
    return *this;
}

std::unique_ptr<PtpEngine> PtpEngineBuilder::build()
{
    if (!m_clock)
    {
        m_clock = IPtpClock::create(ClockConfig{});
    }
    if (!m_network)
    {
        m_network = std::make_unique<InMemoryNetworkAdapter>();
    }
    if (!m_bmca)
    {
        m_bmca = std::make_unique<DefaultBmcaStrategy>();
    }

    return std::make_unique<PtpEngine>(std::move(m_clock), std::move(m_network), std::move(m_bmca));
}
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp
