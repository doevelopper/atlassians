#include <com/github/doevelopper/night/owl/io/WriterMock.hpp>

using namespace com::github::doevelopper::night::owl::io::test;

log4cxx::LoggerPtr WriterMock::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.io.test.WriterMock"));

WriterMock::WriterMock() noexcept
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

WriterMock::~WriterMock() noexcept
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
