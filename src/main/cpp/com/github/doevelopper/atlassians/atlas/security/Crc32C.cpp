
#include <com/github/doevelopper/night/owl/security/Crc32C.hpp>

#include <random>
#include <sstream>

using namespace com::github::doevelopper::night::owl::security;

log4cxx::LoggerPtr Crc32C::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.security.Crc32C"));

Crc32C::Crc32C() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

Crc32C::~Crc32C() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}