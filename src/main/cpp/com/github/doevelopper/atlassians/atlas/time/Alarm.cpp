
#include <com/github/doevelopper/atlassians/atlas/time/Alarm.hpp>

using namespace com::github::doevelopper::atlassians::atlas::time;

log4cxx::LoggerPtr Alarm::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.time.Alarm"));

Alarm::Alarm() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

Alarm::~Alarm() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
