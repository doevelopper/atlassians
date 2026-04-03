#include <com/github/doevelopper/atlassians/atlas/barriers/RecursiveWriteLock.hpp>

using namespace com::github::doevelopper::atlassians::atlas::barriers;

log4cxx::LoggerPtr RecursiveWriteLock::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.barriers.RecursiveWriteLock"));

// RecursiveWriteLock::RecursiveWriteLock() noexcept
// : m_mutex()
// {
//     LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
// }

RecursiveWriteLock::RecursiveWriteLock(RecursiveReadWriteMutex& mutex)
    : m_mutex(mutex)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_mutex.write_lock();
}

RecursiveWriteLock::~RecursiveWriteLock() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_mutex.write_unlock();
}

