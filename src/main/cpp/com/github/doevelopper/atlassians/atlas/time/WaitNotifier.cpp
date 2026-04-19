

#include <com/github/doevelopper/atlassians/atlas/time/WaitNotifier.hpp>

using namespace com::github::doevelopper::atlassians::atlas::time;

log4cxx::LoggerPtr WaitNotifier::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.time.WaitNotifier"));

WaitNotifier::WaitNotifier() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

WaitNotifier::~WaitNotifier() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void WaitNotifier::wait()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::unique_lock<std::mutex> lck(this->m_mutex);
    if (!this->m_notified)
    {
        this->m_cv.wait(lck);
    }
    this->m_notified = false;
}

void WaitNotifier::wait_until( const std::chrono::system_clock::time_point &tm_point)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::unique_lock<std::mutex> lck(this->m_mutex);
    if (!this->m_notified)
    {
        this->m_cv.wait_until(lck, tm_point);
    }
    this->m_notified = false;
}

void WaitNotifier::wait_for( const std::chrono::system_clock::duration &tm_duration)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::unique_lock<std::mutex> lck(this->m_mutex);
    if (!this->m_notified)
    {
        this->m_cv.wait_for(lck, tm_duration);
    }
    this->m_notified = false;
}

void WaitNotifier::notify()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::unique_lock<std::mutex> lck(this->m_mutex);
    this->m_notified = true;
    lck.unlock();
    this->m_cv.notify_one();
}
