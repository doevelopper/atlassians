#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_MUTUALSUBSCRIPTION_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_MUTUALSUBSCRIPTION_HPP

#include <com/github/doevelopper/night/owl/observer/Subscription.hpp>

namespace com::github::doevelopper::night::owl::observer
{
    class MutualSubscription final
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
 public:
        MutualSubscription() noexcept;
        MutualSubscription(const MutualSubscription&) noexcept = default;
        MutualSubscription(MutualSubscription&&) noexcept =  default;
        MutualSubscription& operator=(const MutualSubscription&) noexcept = default;
        MutualSubscription& operator=(MutualSubscription&&) noexcept = default;
        virtual ~MutualSubscription() noexcept;

        explicit MutualSubscription(AbstractSubscription && subscription);
        void unsubscribe();
        explicit operator bool() const noexcept;
    protected:
    private:
        std::shared_ptr<Subscription> m_unsubscribe;
        std::shared_ptr<std::mutex> mtx;
    };
}
#endif
