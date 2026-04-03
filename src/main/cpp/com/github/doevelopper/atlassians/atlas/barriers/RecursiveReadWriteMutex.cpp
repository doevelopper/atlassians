
#include <algorithm>

#include <com/github/doevelopper/atlassians/atlas/barriers/RecursiveReadWriteMutex.hpp>

using namespace com::github::doevelopper::atlassians::atlas::barriers;

log4cxx::LoggerPtr RecursiveReadWriteMutex::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.barriers.RecursiveReadWriteMutex"));

log4cxx::LoggerPtr RecursiveReadWriteMutex::ThreadLockCount::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.barriers.RecursiveReadWriteMutex.ThreadLockCount"));

RecursiveReadWriteMutex::ThreadLockCount::ThreadLockCount() noexcept
    : id()
    , count(0)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

RecursiveReadWriteMutex::ThreadLockCount::ThreadLockCount(std::thread::id id, unsigned int count)
    : id(id)
    , count(count)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

RecursiveReadWriteMutex::ThreadLockCount::~ThreadLockCount() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

RecursiveReadWriteMutex::RecursiveReadWriteMutex() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

RecursiveReadWriteMutex::~RecursiveReadWriteMutex() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void RecursiveReadWriteMutex::read_lock()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto const id = std::this_thread::get_id();

    std::unique_lock lock{this->m_mutex};

    LOG4CXX_INFO(logger, __LOG4CXX_FUNC__
        << "Mutex " << &this->m_mutex
        << " locked by thread " << std::this_thread::get_id());

    this->m_cv.wait(lock, [&]{
        return !this->m_writeLockingThread.count ||
            this->m_writeLockingThread.id == id; });

    auto const my_count = std::find_if(
        this->m_readLockingThreads.begin(),
        this->m_readLockingThreads.end(),
        [id](ThreadLockCount const& candidate) { return id == candidate.id; });

    if (my_count == this->m_readLockingThreads.end())
    {
        this->m_readLockingThreads.push_back(ThreadLockCount(id, 1U));
    }
    else
    {
        ++(my_count->count);
    }
}

void RecursiveReadWriteMutex::read_unlock()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto const id = std::this_thread::get_id();
    {
        std::lock_guard lock{this->m_mutex};

        LOG4CXX_INFO(logger, __LOG4CXX_FUNC__
            << "Mutex " << &this->m_mutex
            << " locked by thread " << std::this_thread::get_id());

        auto const cnt =
            std::find_if(
                this->m_readLockingThreads.begin(),
                this->m_readLockingThreads.end(),
                [id](ThreadLockCount const& candidate) { return id == candidate.id; });

        --(cnt->count);
    }
    this->m_cv.notify_one();
}

void RecursiveReadWriteMutex::write_lock()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto const id = std::this_thread::get_id();
    std::unique_lock lock{this->m_mutex};

    LOG4CXX_INFO(logger, __LOG4CXX_FUNC__
        << "Mutex " << &this->m_mutex
        << " locked by thread " << std::this_thread::get_id());

    this->m_cv.wait(lock, [&]
        {
            if (this->m_writeLockingThread.count &&  this->m_writeLockingThread.id != id) return false;
            for (auto const& candidate : this->m_readLockingThreads)
            {
                if (candidate.id != id && candidate.count != 0) return false;
            }
            return true;
        });

    ++this->m_writeLockingThread.count;
    this->m_writeLockingThread.id = id;
}

void RecursiveReadWriteMutex::write_unlock()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    {
        std::lock_guard lock {this->m_mutex};
        --this->m_writeLockingThread.count;
    }

    this->m_cv.notify_all();
}
