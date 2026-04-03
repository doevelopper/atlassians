
#include <com/github/doevelopper/atlassians/atlas/io/AbstractSerializable.hpp>

using namespace com::github::doevelopper::atlassians::atlas::io;

log4cxx::LoggerPtr AbstractSerializable::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.io.AbstractSerializable"));

AbstractSerializable::AbstractSerializable()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

AbstractSerializable::~AbstractSerializable()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
