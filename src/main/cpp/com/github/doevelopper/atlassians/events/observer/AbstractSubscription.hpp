#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_ABSTRACTSUBSCRIPTION_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_ABSTRACTSUBSCRIPTION_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::night::owl::observer
{
    class AbstractSubscription
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
 public:
        AbstractSubscription() noexcept;
        AbstractSubscription(const AbstractSubscription&) noexcept = default;
        AbstractSubscription(AbstractSubscription&&) noexcept =  default;
        AbstractSubscription& operator=(const AbstractSubscription&) noexcept = default;
        AbstractSubscription& operator=(AbstractSubscription&&) noexcept = default;
        virtual ~AbstractSubscription() noexcept;

        /*!
         * @brief Create a subscription with the specified unsubscribe functor.
         *
         * @param[in] unsubscribe Calling this functor will unsubscribe the
         *       associated observer.
         *
         * @note This is for internal use by subject instances.
         */
        explicit AbstractSubscription(std::function<void()> const & unsubscribe);

        /*!
         * @brief Disassociate the subscription from the subscribed observer.
         *
         * @details  After calling this method, calling unsubscribe() or destroying the
         *       subscription instance will have no effect.
         *
         *  @return Functor taking no parameters that will perform the unsubscribe
         *         when called.
         *
         *    For example:
         *  @code
         *  subscription.release()()
         *  @endcode
         *  is equivalent to
         *  @code
         *  subscription.unsubscribe()
         *  @endcode
         */
        auto release();

        /*!
         * @brief Unsubscribe the associated observer from receiving notifications.
         *      Only the first call of this method will have an effect.
         * @note If release() has been called, this method will have no effect.
         */

        void unsubscribe();
    protected:
    private:
        std::function<void()> m_unsubscribe { []() { } };

        // std::call_once with a std::once_flag would have worked, but it requires
        // pthreads on Linux. We're using this in order not to bring in that
        // dependency.
        std::unique_ptr<std::atomic_flag> m_called { std::make_unique<std::atomic_flag>() };
    };
}
#endif

