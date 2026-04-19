
#include <com/github/doevelopper/atlassians/atlas/quota/NullAllocator.hpp>

using namespace com::github::doevelopper::atlassians::atlas::quota;

template <typename T>
log4cxx::LoggerPtr NullAllocator<T>::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.quota.DeferredReclamationAllocator"));

template <typename T>
NullAllocator<T>::NullAllocator() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename T>
NullAllocator<T>::~NullAllocator() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename T>
template <typename U>
NullAllocator<T>::NullAllocator([[maybe_unused]] const NullAllocator<U> & allocator)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename T>
T * NullAllocator<T>::allocate([[maybe_unused]]  std::size_t numObjects)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return static_cast<T*>( nullptr );
}

template <typename T>
void NullAllocator<T>::deallocate([[maybe_unused]] T * ptr, [[maybe_unused]] std::size_t numObjects) noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
