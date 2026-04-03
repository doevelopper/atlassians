#include <com/github/doevelopper/atlassians/atlas/time/PosixTimestamp.hpp>

using namespace com::github::doevelopper::atlassians::atlas::time;

log4cxx::LoggerPtr PosixTimestamp::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.time.PosixTimestamp"));

PosixTimestamp::PosixTimestamp() noexcept
    : m_clock_id{CLOCK_MONOTONIC}
    , m_nanoseconds{0}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

PosixTimestamp::PosixTimestamp(clockid_t clk, std::chrono::nanoseconds ns)
    : m_clock_id{clk}
    , m_nanoseconds{ns}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

PosixTimestamp::PosixTimestamp(clockid_t clk, struct timespec const& ts)
    : m_clock_id{clk}
    , m_nanoseconds{ts.tv_sec*1000000000LL + ts.tv_nsec}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

clockid_t PosixTimestamp::get_m_clock_id() const
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return m_clock_id;
}

void PosixTimestamp::set_m_clock_id(const clockid_t m_clock_id)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    this->m_clock_id = m_clock_id;
}

const std::chrono::nanoseconds & PosixTimestamp::get_m_nanoseconds() const
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return m_nanoseconds;
}

void PosixTimestamp::set_m_nanoseconds(const std::chrono::nanoseconds & m_nanoseconds)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    this->m_nanoseconds = m_nanoseconds;
}

PosixTimestamp::~PosixTimestamp() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}


