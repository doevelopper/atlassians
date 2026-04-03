
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_TYPETRAITS_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_TYPETRAITS_HPP

#include <functional>

#endif

namespace com::github::doevelopper::night::owl::observer
{
/*!
 * @brief Check if a callable type is compatible with an observer type.
 *
 * @details A callable is compatible with an observer type if the callable can be stored
 *      inside a `std::function<ObserverType>`.
 *
 * @tparam CallableType The type to check if it is compatible with the ObserverType.
 * @tparam ObserverType The type of the observer to check against.
 *
 * @note The static member ``value`` will be true, if the CallableType is compatible
 *  with the ObserverType.
 *
 * @ingroup observable_detail
 */

template <typename CallableType, typename ObserverType>
using is_compatible_with_observer = std::is_convertible<
                                        CallableType,
                                        std::function<ObserverType>>;

/*!
 *
 * @brief  Check if a callable type can be used to subscribe to a subject.
 *
 * @details A callable can be used to subscribe to a subject if the callable is compatible
 *  with the subject's ``observer_type`` type.
 *
 * @tparam CallableType The type to check if it can be used to subscribe to the
 *         provided subject.
 *
 * @tparam SubjectType The subject to check against.
 *
 * @note The static member ``value`` will be true if, the CallableType can be used
 *       with the SubjectType.
 * @ingroup observable_detail
 */

template <typename CallableType, typename SubjectType>
using is_compatible_with_subject = is_compatible_with_observer<
                                        CallableType,
                                        typename SubjectType::observer_type>;

/*!
 *  @brief Check if two types are equality comparable between themselves.
 */

template<typename A, typename B, typename = void>
struct are_equality_comparable : std::false_type
{

};

//! \cond
template<typename A, typename B>
struct are_equality_comparable<A, B, std::enable_if_t<
       std::is_same<
           std::decay_t<decltype(std::equal_to<> { }(std::declval<A>(),std::declval<B>()))>, bool>::value>
       > : std::true_type
{
};
//! \endcond

}