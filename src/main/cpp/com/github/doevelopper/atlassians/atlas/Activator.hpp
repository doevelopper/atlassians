
#ifndef COM_GITHUB_DOEVELOPPER_ATLASSIANS_ATLAS_UTILS_ACTIVATOR_HPP
#define COM_GITHUB_DOEVELOPPER_ATLASSIANS_ATLAS_UTILS_ACTIVATOR_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::utils
{
    // Activator<insigned int> vUint;
    template < class T >

    class SDLC_API_EXPORT Activator
    {
    LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        Activator();
        virtual ~Activator();
        [[noreturn]] void value(const T &value);
        T value() const;
        SDLC_REQUIRED_RESULT const bool isValid() const;
        [[noreturn]] void isValid(bool value);

    private:

        T m_value;
        bool m_isValid;
    };

}  // namespace com::github::doevelopper::atlassians::atlas::utils

#endif
