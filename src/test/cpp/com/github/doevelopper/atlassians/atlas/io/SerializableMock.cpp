#include <com/github/doevelopper/night/owl/io/SerializableMock.hpp>

using namespace com::github::doevelopper::night::owl::io::test;

log4cxx::LoggerPtr SerializableMock::logger =
  log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.io.test.SerializableMock"));

SerializableMock::SerializableMock() noexcept
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

SerializableMock::~SerializableMock() noexcept
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
