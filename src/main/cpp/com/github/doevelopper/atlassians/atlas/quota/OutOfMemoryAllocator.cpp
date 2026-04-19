
#include <com/github/doevelopper/atlassians/atlas/quota/OutOfMemoryAllocator.hpp>

using namespace com::github::doevelopper::atlassians::atlas::quota;

template <typename Allocator>
log4cxx::LoggerPtr OutOfMemoryAllocator<Allocator>::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.quota.OutOfMemoryAllocator"));

template <typename Allocator>
OutOfMemoryAllocator<Allocator>::OutOfMemoryAllocator() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename Allocator>
OutOfMemoryAllocator<Allocator>::~OutOfMemoryAllocator() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename Allocator>
OutOfMemoryAllocator<Allocator>::OutOfMemoryAllocator(Allocator allocator, bool & oom_flag)
    : m_allocator {std::move(allocator)}
    , m_oom_flag {oom_flag}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename Allocator>
OutOfMemoryAllocator<Allocator>::OutOfMemoryAllocator(bool & oom_flag)
    : OutOfMemoryAllocator {Allocator {}, oom_flag}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename Allocator>
template <typename OtherAllocator, typename>
OutOfMemoryAllocator<Allocator>::OutOfMemoryAllocator(OutOfMemoryAllocator<OtherAllocator> const & rsh)
    : m_allocator {rsh.m_allocator}
    , m_oom_flag {rsh.m_oom_flag}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename Allocator>
template <typename... Args>
void OutOfMemoryAllocator<Allocator>::construct(pointer p, Args &&... args)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    this->m_allocator.construct(p, std::forward<Args>(args)...);
}

template <typename Allocator>
void OutOfMemoryAllocator<Allocator>::destroy(pointer p)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    this->m_allocator.destroy(p);
}

template <typename Allocator>
typename OutOfMemoryAllocator<Allocator>::pointer OutOfMemoryAllocator<Allocator>::allocate(size_type n)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (this->m_oom_flag)
    {
        this->m_oom_flag = false;
        throw std::bad_alloc {};
    }
    else
    {
        return this->m_allocator.allocate(n);
    }
}

template <typename Allocator>
void OutOfMemoryAllocator<Allocator>::deallocate(pointer p, size_type n)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    this->m_allocator.deallocate(p, n);
}
