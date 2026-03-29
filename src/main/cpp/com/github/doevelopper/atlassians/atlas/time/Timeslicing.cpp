
#include <com/github/doevelopper/atlas/time/Timeslicing.hpp>

using namespace com::github::doevelopper::atlas::time;

template <typename Clock>
log4cxx::LoggerPtr Timeslicing<Clock>::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.atlas.time.Timeslicing"));

template <typename Clock>
Timeslicing<Clock>::Timeslicing() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename Clock>
Timeslicing<Clock>::~Timeslicing() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // If this fails, it means that some events have outlived the event loop, which is not permitted.
    // assert(isEmpty());
}
