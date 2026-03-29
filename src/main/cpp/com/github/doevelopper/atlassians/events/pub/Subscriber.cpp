#include <com/github/doevelopper/atlassians/events/pub/Subscriber.hpp>

using namespace com::github::doevelopper::atlassians::events::pub;

log4cxx::LoggerPtr Subscriber::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.events.pub.Subscriber"));

Subscriber::Subscriber() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

Subscriber::~Subscriber() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
