#include <com/github/doevelopper/atlassians/atlas/io/Reader.hpp>

using namespace com::github::doevelopper::atlassians::atlas::io;

log4cxx::LoggerPtr Reader::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.io.Reader"));

Reader::Reader()
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

Reader::~Reader()
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
