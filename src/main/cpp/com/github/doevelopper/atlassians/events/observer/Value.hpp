#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_VALUE_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_VALUE_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::night::owl::observer
{
    template <typename ValueType>
    class Value
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
 public:
        Value() noexcept;
        Value(const Value&) noexcept = default;
        Value(Value&&) noexcept =  default;
        Value& operator=(const Value&) noexcept = default;
        Value& operator=(Value&&) noexcept = default;
        virtual ~Value() noexcept;
    protected:
    private:
    };
}
#endif
