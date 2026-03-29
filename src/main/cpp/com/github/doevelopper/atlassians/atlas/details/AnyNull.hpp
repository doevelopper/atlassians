
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_DETAILS_ANYNULL_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_DETAILS_ANYNULL_HPP

#include <type_traits>

namespace com::github::doevelopper::night::owl::details
{
    /// Used to indicate that a value passed to IsNull is not comparable to null.
    struct IsNullNothingComparable final
    {};

    static inline bool operator| (IsNullNothingComparable, bool result)
    {
        return result;
    }

    static inline bool operator| (bool result, IsNullNothingComparable)
    {
        return result;
    }

    static inline auto operator| (IsNullNothingComparable, IsNullNothingComparable)
    {
        return IsNullNothingComparable();
    }

    static inline bool operator== (IsNullNothingComparable, IsNullNothingComparable)
    {
        return true;
    }

    /// Type trait which determines whether a given type is comparable to nullptr.
    template<typename Value, bool Comparable = std::is_array<Value>::value || std::is_pointer<Value>::value || std::is_null_pointer<Value>::value || std::is_convertible<std::nullptr_t, Value>::value>
    struct IsNullComparable final : std::integral_constant<bool, Comparable>
    {};

    /// Used in the implementation of anyNull() to determine whether a given value
    /// (which may or may not be a pointer) is null.
    template<typename Value, bool Comparable = IsNullComparable<Value>::value>
    struct IsNull final
    {
    public:
        IsNull () = delete;

        static bool isNull (Value value)
        {
            return value == nullptr;
        }
    };

    template<typename Value>
    struct IsNull<Value, false> final
    {
    public:
        IsNull () = delete;

        static auto isNull (Value)
        {
            return IsNullNothingComparable();
        }
    };

    static inline auto anyNull ()
    {
        return IsNullNothingComparable();
    }

    /*!
     * @brief Checks whether any of the values passed in represent a null pointer (as
     *   determined by comparing against nullptr).
     *
     *   @details If none of the values are comparable to nullptr, the return type will be
     *      IsNullNothingComparable.
     */
    template<typename Value, typename... Remaining>
    auto anyNull (Value value, Remaining ...remaining)
    {
        return IsNull<Value>::isNull(value) | anyNull(remaining...);
    }
}
#endif //ANYNULL_HPP
