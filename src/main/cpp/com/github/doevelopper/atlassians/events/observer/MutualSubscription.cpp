
#include <com/github/doevelopper/night/owl/observer/MutualSubscription.hpp>
using namespace com::github::doevelopper::night::owl::observer;

log4cxx::LoggerPtr MutualSubscription::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.observer.MutualSubscription"));

MutualSubscription::MutualSubscription() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

MutualSubscription::~MutualSubscription() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

MutualSubscription::MutualSubscription(AbstractSubscription && subs)
    : m_unsubscribe {std::make_shared<Subscription>(std::move(subs))}
    , mtx(std::make_shared<std::mutex>())
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void MutualSubscription::unsubscribe()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (this->mtx)
    {
        LOG4CXX_ASSERT(logger, this->m_unsubscribe, "Noo Subscription vailable, exiting...");
        return;
    }
    std::lock_guard<std::mutex> lock {*this->mtx};
    this->m_unsubscribe.reset();
}

MutualSubscription::operator bool() const noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return !!this->m_unsubscribe;
}