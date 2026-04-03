

#include <com/github/doevelopper/night/owl/barriers/SpinLock.hpp>

using namespace  com::github::doevelopper::night::owl::barriers;

log4cxx::LoggerPtr SpinLock::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.barriers.SpinLock"));

SpinLock::SpinLock() noexcept
    : m_lock{ATOMIC_FLAG_INIT}
    , m_owner { }
    , m_count { 0 }
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

SpinLock::~SpinLock() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

void SpinLock::lock() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    if (const std::thread::id   threadID = std::this_thread::get_id(); threadID != m_owner) [[likely]]
    {
        while (true)
        {
            // test_and_set sets the flag to true and returns the previous value;
            // if it's True, someone else is owning the lock.
            if (! m_lock.test_and_set(std::memory_order_acquire)) break;
            while (m_lock.test(std::memory_order_relaxed)) ;
        }
        m_owner = threadID;
    }
    m_count += 1;
}

bool SpinLock::try_lock() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    const std::thread::id   threadID = std::this_thread::get_id();
    if (threadID == m_owner || ! m_lock.test_and_set(std::memory_order_acquire))
    {
        m_owner = threadID;
        m_count += 1;
    }

    return (threadID == m_owner);
}

void SpinLock::unlock() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    if (const std::thread::id   threadID = std::this_thread::get_id(); threadID == m_owner) [[likely]]  {
        m_count -= 1;

        LOG4CXX_ASSERT(logger,m_count >= 0,"Dazed and Confused (m_count >= 0), but trying to continue.");//assert(m_count >= 0);
        if (m_count == 0)
        {
            m_owner = std::thread::id { };
            m_lock.clear(std::memory_order_release);
        }
        else
        {
            LOG4CXX_ASSERT(logger,false,"Dazed and Confused, but trying to continue.");
            //else  assert(0);// Should fire/fails
        }

    }
}
