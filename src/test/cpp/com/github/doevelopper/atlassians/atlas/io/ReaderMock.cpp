#include <com/github/doevelopper/night/owl/io/ReaderMock.hpp>

using namespace com::github::doevelopper::night::owl::io::test;

log4cxx::LoggerPtr ReaderMock::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.io.test.ReaderMock"));

ReaderMock::ReaderMock() noexcept
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ReaderMock::~ReaderMock() noexcept
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
