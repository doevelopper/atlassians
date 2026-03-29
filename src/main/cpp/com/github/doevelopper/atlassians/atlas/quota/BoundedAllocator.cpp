
#include <com/github/doevelopper/atlassians/atlas/quota/BoundedAllocator.hpp>

using namespace com::github::doevelopper::atlassians::atlas::quota;

template <typename Allocator>
log4cxx::LoggerPtr BoundedAllocator<Allocator>::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.quota.BoundedAllocator"));

template <typename Allocator>
BoundedAllocator<Allocator>::BoundedAllocator() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename Allocator>
BoundedAllocator<Allocator>::~BoundedAllocator() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename Allocator>
BoundedAllocator<Allocator>::BoundedAllocator(
    Allocator allocator, std::size_t max_live_allocations, std::size_t & live_allocations)
    : m_allocator {std::move(allocator)}
    , m_max_live_allocations {max_live_allocations}
    , m_live_allocations {live_allocations}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename Allocator>
BoundedAllocator<Allocator>::BoundedAllocator(std::size_t max_live_allocations, std::size_t & live_allocations)
    : BoundedAllocator {Allocator {}, max_live_allocations, live_allocations}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename Allocator>
template <typename OtherAllocator, typename>
BoundedAllocator<Allocator>::BoundedAllocator(BoundedAllocator<OtherAllocator> const & other)
    : m_allocator {other.m_allocator}
    , m_max_live_allocations {other.m_max_live_allocations}
    , m_live_allocations {other.m_live_allocations}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename Allocator>
template <typename... Args>
void BoundedAllocator<Allocator>::construct(pointer p, Args &&... args)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_allocator.construct(p, std::forward<Args>(args)...);
}

template <typename Allocator>
void BoundedAllocator<Allocator>::destroy(pointer p)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_allocator.destroy(p);
}

template <typename Allocator>
typename BoundedAllocator<Allocator>::pointer BoundedAllocator<Allocator>::allocate(size_type n)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (m_live_allocations + 1 > m_max_live_allocations)
    {
        throw std::bad_alloc {};
    }
    else
    {
        auto p = m_allocator.allocate(n);
        ++m_live_allocations;
        return p;
    }
}

template <typename Allocator>
void BoundedAllocator<Allocator>::deallocate(pointer p, size_type n)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_allocator.deallocate(p, n);
    --m_live_allocations;
}
