#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_SUBSCRIPTION_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_SUBSCRIPTION_HPP

#include <com/github/doevelopper/atlassians/events/observer/AbstractSubscription.hpp>

namespace com::github::doevelopper::night::owl::observer
{
    /*!
     * @brief Unsubscribe the associated observer when destroyed.
     *
     * @note All methods of this class can be safely called in parallel, from multiple
     *  threads.
     */

    class Subscription final : public AbstractSubscription
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
 public:
        /*!
         * @brief  Create an empty subscription.
         *
         * @note Calling unsubscribe on an empty subscription will have no effect.
         */
        Subscription() noexcept;
        // Subscription(const Subscription&) noexcept = default;
        Subscription(Subscription&&) noexcept =  default;
        // Subscription& operator=(const Subscription&) noexcept = default;
        Subscription& operator=(Subscription&&) noexcept = default;
        ~Subscription() noexcept override;

        /*!
         * @brief Create an unique subscription from an Subscription.
         *
         * @param[in] sub An infinite subscription that will be converted to an
         *      Subscription.
         */
        Subscription(AbstractSubscription && sub);

    protected:
    private:
        Q_DISABLE_COPY(Subscription)
    };
}
#endif
