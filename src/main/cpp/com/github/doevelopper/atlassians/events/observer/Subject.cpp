
#include <com/github/doevelopper/night/owl/observer/Subject.hpp>
using namespace com::github::doevelopper::night::owl::observer;

template <typename... Args>
log4cxx::LoggerPtr Subject<void(Args...)>::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.observer.Subject"));

template <typename... Args>
Subject<void(Args...)>::Subject() noexcept
    : m_observers{std::make_shared<Collections>() }
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

template <typename... Args>
Subject<void(Args...)>::~Subject() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

template <typename... Args>
template <typename Callable>
auto com::github::doevelopper::night::owl::observer::Subject<void(Args...)>::subscribe(Callable && observer)
    -> AbstractSubscription
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // static_assert((is_compatible_with_observer<Callable, ObserverType>::value),
    //               "The provided observer object is not callable or not compatible with the subject");
    LOG4CXX_ASSERT(
        logger, (is_compatible_with_observer<Callable, ObserverType>::value),
        "The provided observer object is not callable or not compatible with the subject");

    auto const id = this->m_observers->insert(observer);

    // return AbstractSubscription{ [id, weak_observers = std::weak_ptr<Collection> { this->m_observers }]() {
    //     auto const observers = weak_observers.lock();
    //     if(!observers)
    //         return;
    //
    //     observers->remove(id);
    // }};
}

template <typename... Args>
template <typename Callable, typename... ActualArgs>
auto Subject<void(Args...)>::subscribeAndCall(Callable && observer, ActualArgs... arguments) -> AbstractSubscription
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    observer(std::forward<Args ...>(arguments)...);
    return this->subscribe(std::forward<Callable>(observer));
}

template <typename... Args>
void Subject<void(Args...)>::notify(Args... arguments) const
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    LOG4CXX_ASSERT(
        logger, this->m_observers, " Observers not availabe ");
    this->m_observers->apply([&](auto && observer) { observer(arguments ...); });
}

template <typename... Args>
auto Subject<void(Args...)>::empty() const noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    LOG4CXX_ASSERT(
        logger, this->m_observers, " Observers not availabe ");
    return this->m_observers->empty();
}