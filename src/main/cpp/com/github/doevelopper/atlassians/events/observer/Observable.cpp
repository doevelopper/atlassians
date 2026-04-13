
#include <com/github/doevelopper/atlassians/events/observer/Observable.hpp>
using namespace com::github::doevelopper::night::owl::observer;

log4cxx::LoggerPtr Observable::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.observer.Observable"));

Observable::Observable() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

Observable::~Observable() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

