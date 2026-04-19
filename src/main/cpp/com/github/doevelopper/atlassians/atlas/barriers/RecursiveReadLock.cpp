
#include <com/github/doevelopper/atlassians/atlas/barriers/RecursiveReadLock.hpp>

using namespace com::github::doevelopper::atlassians::atlas::barriers;

log4cxx::LoggerPtr RecursiveReadLock::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.barriers.RecursiveReadLock"));

// RecursiveReadLock::RecursiveReadLock() noexcept
// : m_mutex()
// {
//     LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
// }

RecursiveReadLock::~RecursiveReadLock() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_mutex.read_unlock();
}

RecursiveReadLock::RecursiveReadLock(RecursiveReadWriteMutex& mutex)
    : m_mutex(mutex)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_mutex.read_lock();
}
