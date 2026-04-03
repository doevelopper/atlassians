#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_SUBJECT_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_SUBJECT_HPP

#include <com/github/doevelopper/night/owl/observer/Collection.hpp>
#include <com/github/doevelopper/night/owl/observer/TypeTraits.hpp>
#include <com/github/doevelopper/night/owl/observer/AbstractSubscription.hpp>


namespace com::github::doevelopper::night::owl::observer
{
    template <typename ...>
    class Subject;


    /*!
     * @brief Store observers and provide a way to notify them when events occur.
     *
     * @details  Observers are objects that satisfy the Callable concept and can be stored
     * @code
     *    std::function<void(Args ...)>
     * @endcode
     *
     *  Once you call subscribe(), the observer is said to be subscribed to
     *  notifications from the subject.
     *
     *  Calling notify(), will call all the currently subscribed observers with the
     *   arguments provided to notify().
     *
     *   All methods can be safely called in parallel, from multiple threads.
     *
     *   @tparam Args Observer arguments. All observer types must be storable
     *   inside a
     *   @code
     *      std::function<void(Args ...)>
     *   @endcode
     *
     *   @warning Even though subjects themselves are safe to use in parallel,
     *      observers need to handle being called from multiple threads too.
    */

    template <typename... Args>
    class Subject<void(Args...)>
    {
        Q_DISABLE_COPY(Subject)
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        using ObserverType = void(Args...);

        Subject() noexcept;
        // Subject(const Subject &) noexcept             = default;
        Subject(Subject &&) noexcept                  = default;
        // Subject & operator=(const Subject &) noexcept = default;
        Subject & operator=(Subject &&) noexcept      = default;
        virtual ~Subject() noexcept;


        /*!
         * @brief Subscribe an observer to notifications.
         *
         * @note Can safely call this method in parallel, from multiple threads.
         *
         * @details This method is reentrant, you can add and remove observers from inside
         *      other, running, observers.
         *
         * @param[in] observer An observer callable that will be subscribed to
         *                     notifications from this subject.
         *
         * @tparam Callable Type of the observer object. This type must satisfy the
         *                  Callable concept and must be storable inside a
         *                  ``std::function<void(Args ...)>``.
         *
         * @return An unique subscription that can be used to unsubscribe the
         *         provided observer from receiving notifications from this subject.
         *
         * @warning Observers must be valid to be called for as long as they are
         *          subscribed and there is a possibility to be called.
         *
         * @warning Observers must be safe to be called in parallel, if the notify()
         *          method will be called from multiple threads.
         */

        template <typename Callable>
        auto subscribe(Callable && observer) -> AbstractSubscription;
        /*!
         * @brief Subscribe an observer to notifications and immediately call it with
         *      the provided arguments.
         *
         * @details This method works exactly like the regular subscribe except it also
         *      invokes the observer.
         *
         *      If the observer call throws an exception during the initial call, it
         *      will not be subscribed.
         *
         * @note The observer will not be subscribed during the initial call.
         *
         * @param[in] observer An observer callable that will be subscribed to
         *                     notifications from this subject and immediately
         *                     invoked with the provided arguments.
         * @param[in] arguments Arguments to pass to the observer when called.
         *
         * @tparam Callable Type of the observer callable. This type must satisfy
         *         the Callable concept and must be storable inside a
         *         ``std::function<void(Args...)>``.
         *
         * @return An unique subscription that can be used to unsubscribe the
         *         provided observer from receiving notifications from this
         *         subject.
         *
         * @warning Observers must be valid to be called for as long as they are
         *          subscribed and there is a possibility to be called.
         *
         * @warning Observers must be safe to be called in parallel, if the notify()
         *          method will be called from multiple threads.
         *
         * @see subscribe()
        */
        template <typename Callable, typename ... ActualArgs>
        auto subscribeAndCall(Callable && observer, ActualArgs ... arguments)
        -> AbstractSubscription;
        /*!
         * @brief Notify all currently subscribed observers.
         *
         * @details This method will block until all subscribed observers are called. The
         *      method will call observers one-by-one in an unspecified order.
         *
         *      You can safely call this method in parallel, from multiple threads.
         *
         * @note Observers subscribed during a notify call, will not be called as
         *       part of the notify call during which they were added.
         *
         * @note Observers removed during the notify call, before they themselves
         *       have been called, will not be called.
         *
         * The method is reentrant; you can call notify() from inside a running
         * observer.
         *
         * @param[in] arguments Arguments that will be forwarded to the subscribed
         *                      observers.
         *
         * @warning All observers that will be called by notify() must remain valid
         *          to be called for the duration of the notify() call.
         *
         * @warning If notify() is called from multiple threads, all observers must
         *          be safe to call from multiple threads.
         */
        void notify(Args ... arguments) const;
        //! @broef Return true if there are no subscribers.
        auto empty() const noexcept;

    protected:
    private:

        using Collections = Collection<std::function<ObserverType>>;
        std::shared_ptr<Collections> m_observers;
    };


    /*!
     * @brief Subject specialization that can be used inside a class, as a member, to
     *      prevent external code from calling notify(), but still allow anyone to
     *      subscribe.
     *
     * @note Except for the notify() method being private, this specialization is
     *      exactly the same as a regular subject.
     *
     * @tparam ObserverType The function type of the observers that will subscribe
     *      to notifications.
     *
     * @tparam EnclosingType This type will be declared a friend of the subject and
     *      will have access to the notify() method.
     * @see subject<void(Args ...)>
     * @ingroup Observable
     */
    template <typename ObserverType, typename EnclosingType>
    class Subject<ObserverType, EnclosingType> : public Subject<ObserverType>
    {
    public:
        using Subject<ObserverType>::subject;

    private:
         //!* @see subject<void(Args...)>::notify
        using Subject<ObserverType>::notify;
        friend EnclosingType;
    };

}
#endif
