
#include <com/github/doevelopper/atlassians/atlas/quota/OnDestruction.hpp>

using namespace  com::github::doevelopper::atlassians::atlas::quota;

log4cxx::LoggerPtr OnDestruction::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.quota.OnDestruction"));

OnDestruction::OnDestruction() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

OnDestruction::~OnDestruction() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    this->callback();
}

OnDestruction::OnDestruction(std::function<void()> f)
    : callback(f)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
