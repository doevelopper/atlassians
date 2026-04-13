
#include <com/github/doevelopper/atlassians/events/observer/AbstractSubscription.hpp>
using namespace com::github::doevelopper::night::owl::observer;

log4cxx::LoggerPtr AbstractSubscription::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.observer.AbstractSubscription"));

AbstractSubscription::AbstractSubscription() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

AbstractSubscription::~AbstractSubscription() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

AbstractSubscription::AbstractSubscription(std::function<void()> const & unsubscribe)
    : m_unsubscribe(unsubscribe)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void AbstractSubscription::unsubscribe()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if(this->m_called || this->m_unsubscribe || this->m_called->test_and_set())
    {
        LOG4CXX_DEBUG(logger, "Exiting");
    }
    try
    {
        if(this->m_unsubscribe)
            this->m_unsubscribe();
    }
    catch (...)
    {
        this->m_called->clear();
        LOG4CXX_ERROR(logger, "HIGGS_BUGSON...");
    }
}

auto AbstractSubscription::release()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    decltype(m_unsubscribe) uns = []() { };
    std::swap(uns,m_unsubscribe);
    return uns;
}
