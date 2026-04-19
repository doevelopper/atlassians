#include <com/github/doevelopper/atlassians/atlas/time/ptp/PtpStateMachine.hpp>

#include <stdexcept>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
namespace
{
class InitializingState final : public IPtpState
{
public:
    void enter(PtpStateMachine &context) override
    {
        (void)context;
    }

    void exit(PtpStateMachine &context) override
    {
        (void)context;
    }

    void processMessage(PtpStateMachine &context, const PtpMessage &message) override
    {
        if (message.getType() == MessageType::Announce)
        {
            context.changeState(PtpState::Slave);
        }
    }

    void run(PtpStateMachine &context) override
    {
        context.changeState(PtpState::Listening);
    }

    std::string name() const override
    {
        return "Initializing";
    }
};

class ListeningState final : public IPtpState
{
public:
    void enter(PtpStateMachine &context) override
    {
        (void)context;
    }

    void exit(PtpStateMachine &context) override
    {
        (void)context;
    }

    void processMessage(PtpStateMachine &context, const PtpMessage &message) override
    {
        if (message.getType() == MessageType::Sync)
        {
            context.changeState(PtpState::Slave);
            return;
        }
        if (message.getType() == MessageType::Announce)
        {
            context.changeState(PtpState::Master);
        }
    }

    void run(PtpStateMachine &context) override
    {
        (void)context;
    }

    std::string name() const override
    {
        return "Listening";
    }
};

class MasterState final : public IPtpState
{
public:
    void enter(PtpStateMachine &context) override
    {
        SyncMessage sync(context.clock().getCurrentTime());
        context.network().sendMessage(sync);
    }

    void exit(PtpStateMachine &context) override
    {
        (void)context;
    }

    void processMessage(PtpStateMachine &context, const PtpMessage &message) override
    {
        if (message.getType() == MessageType::Announce)
        {
            context.changeState(PtpState::Slave);
        }
    }

    void run(PtpStateMachine &context) override
    {
        SyncMessage sync(context.clock().getCurrentTime());
        context.network().sendMessage(sync);
    }

    std::string name() const override
    {
        return "Master";
    }
};

class SlaveState final : public IPtpState
{
public:
    void enter(PtpStateMachine &context) override
    {
        (void)context;
    }

    void exit(PtpStateMachine &context) override
    {
        (void)context;
    }

    void processMessage(PtpStateMachine &context, const PtpMessage &message) override
    {
        if (message.getType() == MessageType::Sync)
        {
            context.clock().adjustTime(TimeOffset{1});
        }
    }

    void run(PtpStateMachine &context) override
    {
        (void)context;
    }

    std::string name() const override
    {
        return "Slave";
    }
};

class FaultyState final : public IPtpState
{
public:
    void enter(PtpStateMachine &context) override
    {
        (void)context;
    }

    void exit(PtpStateMachine &context) override
    {
        (void)context;
    }

    void processMessage(PtpStateMachine &context, const PtpMessage &message) override
    {
        (void)context;
        (void)message;
    }

    void run(PtpStateMachine &context) override
    {
        (void)context;
    }

    std::string name() const override
    {
        return "Faulty";
    }
};
} // namespace

PtpStateMachine::PtpStateMachine(IPtpClock &clock, INetworkAdapter &network)
    : m_clock(clock),
      m_network(network),
      m_state(PtpState::Initializing),
      m_current(createState(PtpState::Initializing))
{
    m_current->enter(*this);
}

void PtpStateMachine::processMessage(const PtpMessage &message)
{
    m_current->processMessage(*this, message);
}

void PtpStateMachine::run()
{
    m_current->run(*this);
}

void PtpStateMachine::changeState(const PtpState newState)
{
    m_current->exit(*this);
    m_state = newState;
    m_current = createState(newState);
    m_current->enter(*this);
}

IPtpClock &PtpStateMachine::clock()
{
    return m_clock;
}

INetworkAdapter &PtpStateMachine::network()
{
    return m_network;
}

PtpState PtpStateMachine::getState() const
{
    return m_state;
}

std::string PtpStateMachine::getStateName() const
{
    return m_current->name();
}

std::unique_ptr<IPtpState> PtpStateMachine::createState(const PtpState state)
{
    switch (state)
    {
    case PtpState::Initializing:
        return std::make_unique<InitializingState>();
    case PtpState::Listening:
        return std::make_unique<ListeningState>();
    case PtpState::Master:
        return std::make_unique<MasterState>();
    case PtpState::Slave:
        return std::make_unique<SlaveState>();
    case PtpState::Faulty:
        return std::make_unique<FaultyState>();
    default:
        throw std::logic_error("Unknown PTP state");
    }
}
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp
