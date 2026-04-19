#include <com/github/doevelopper/atlassians/events/pub/Publisher.hpp>

using namespace com::github::doevelopper::atlassians::events::pub;

log4cxx::LoggerPtr Publisher::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.events.pub.Publisher"));

Publisher::Publisher() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

Publisher::~Publisher() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
