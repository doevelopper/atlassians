#include <com/github/doevelopper/atlassians/atlas/time/NaturallyAligned.hpp>

using namespace com::github::doevelopper::atlassians::atlas::time;

template <typename T>
log4cxx::LoggerPtr NaturallyAligned<T>::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.time.NaturallyAligned"));

template <typename T>
NaturallyAligned<T>::NaturallyAligned() noexcept
    : T()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename T>
NaturallyAligned<T>::~NaturallyAligned() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename T>
NaturallyAligned<T>::NaturallyAligned(const NaturallyAligned& t) noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename T>
template <class U>
NaturallyAligned<T>::NaturallyAligned(const U& u) noexcept
    : T(u)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
