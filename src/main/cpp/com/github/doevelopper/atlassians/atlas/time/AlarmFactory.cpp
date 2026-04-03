
#include <com/github/doevelopper/atlassians/atlas/time/AlarmFactory.hpp>

using namespace com::github::doevelopper::atlassians::atlas::time;

log4cxx::LoggerPtr AlarmFactory::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.time.AlarmFactory"));

AlarmFactory::AlarmFactory() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

AlarmFactory::~AlarmFactory() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

