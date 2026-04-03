#include <com/github/doevelopper/atlassians/atlas/utils/Task.hpp>

using namespace com::github::doevelopper::atlassians::atlas::utils;

#include <stdexcept>

log4cxx::LoggerPtr Task::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.utils.Task"));

Task::Task(Action action, std::string_view name)
    : m_name(name), m_action(std::move(action)), m_state(State::Idle)
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

Task::~Task() noexcept
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

void Task::run()
{
    Action actionCopy{};

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_state != State::Scheduled)
        {
            return;
        }
        m_state = State::Running;
        m_lastStarted = std::chrono::steady_clock::now();
        actionCopy = m_action;
    }

    try
    {
        if (!actionCopy)
        {
            throw std::runtime_error("Task action not set");
        }
        actionCopy();
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_state != State::Cancelled)
        {
            m_state = State::Completed;
        }
    }
    catch (const std::exception &ex)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_lastError = ex.what();
        if (m_state != State::Cancelled)
        {
            m_state = State::Failed;
        }
    }
    catch (...)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_lastError = "Unknown task error";
        if (m_state != State::Cancelled)
        {
            m_state = State::Failed;
        }
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_lastFinished = std::chrono::steady_clock::now();
        ++m_runCount;
    }
}

void Task::schedule()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_state == State::Running)
    {
        return;
    }
    m_lastError.reset();
    m_state = State::Scheduled;
}

void Task::cancel()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_state = State::Cancelled;
}

auto Task::getName() const noexcept -> std::string_view
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::string_view(m_name);
}

auto Task::setName(std::string_view name) -> void
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_name = name;
}

auto Task::getState() const noexcept -> State
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_state;
}

auto Task::isScheduled() const noexcept -> bool
{
    return getState() == State::Scheduled;
}

auto Task::isCancelled() const noexcept -> bool
{
    return getState() == State::Cancelled;
}

auto Task::isRunning() const noexcept -> bool
{
    return getState() == State::Running;
}

auto Task::hasCompleted() const noexcept -> bool
{
    return getState() == State::Completed;
}

auto Task::getRunCount() const noexcept -> std::size_t
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_runCount;
}

auto Task::getLastError() const noexcept -> std::optional<std::string>
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_lastError;
}

auto Task::getStatus() const noexcept -> Status
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return Status{m_state, m_runCount, m_lastError, m_lastStarted, m_lastFinished};
}

