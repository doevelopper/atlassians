
#include <com/github/doevelopper/night/owl/barriers/SpinGuard.hpp>

using namespace  com::github::doevelopper::night::owl::barriers;

log4cxx::LoggerPtr SpinGuard::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.barriers.SpinGuard"));

SpinGuard::SpinGuard() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

SpinGuard::~SpinGuard() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}
