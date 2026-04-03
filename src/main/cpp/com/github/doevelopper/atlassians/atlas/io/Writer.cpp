#include <com/github/doevelopper/atlassians/atlas/io/Writer.hpp>

using namespace com::github::doevelopper::atlassians::atlas::io;

log4cxx::LoggerPtr Writer::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.io.Writer"));

Writer::Writer()
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

Writer::~Writer()
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
