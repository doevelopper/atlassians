
#include <com/github/doevelopper/night/owl/barriers/SpinMutex.hpp>

using namespace  com::github::doevelopper::night::owl::barriers;

log4cxx::LoggerPtr SpinMutex::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.barriers.SpinMutex"));

SpinMutex::SpinMutex() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

SpinMutex::~SpinMutex() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

bool SpinMutex::try_lock() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return !this->m_flag.test_and_set(std::memory_order_acquire);
}

void SpinMutex::lock() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    while (!this->try_lock());
}

void SpinMutex::unlock() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    this->m_flag.clear(std::memory_order_release);
}
