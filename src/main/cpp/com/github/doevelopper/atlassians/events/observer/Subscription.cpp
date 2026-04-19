
#include <com/github/doevelopper/atlassians/events/observer/Subscription.hpp>
using namespace com::github::doevelopper::night::owl::observer;

log4cxx::LoggerPtr Subscription::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.observer.Subscription"));

Subscription::Subscription() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

Subscription::~Subscription() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

Subscription::Subscription(AbstractSubscription && sub)
    : AbstractSubscription(std::move(sub))
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    this->unsubscribe();
}
