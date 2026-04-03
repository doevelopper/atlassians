
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_OBSERVABLE_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_OBSERVABLE_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::night::owl::observer
{
    class Observable
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
 public:
        Observable() noexcept;
        Observable(const Observable&) noexcept = default;
        Observable(Observable&&) noexcept =  default;
        Observable& operator=(const Observable&) noexcept = default;
        Observable& operator=(Observable&&) noexcept = default;
        virtual ~Observable() noexcept;
    protected:
    private:
    };
}


#endif
/*!
 *  @brief Top-level observable classes.
 *  @defgroup observable Observable
 */

/*!
 *  @brief Everything related to observable expressions.
 *  @defgroup observable_expressions Expressions
 *  @ingroup observable

/*!
 *  @brief These classes are internal to the library, they may change at any time. You
 *      should not use these classes in your code.
 *  @defgroup observable_detail Private
 *  @ingroup observable
*/
