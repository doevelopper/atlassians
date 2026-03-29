#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_DEMANGLER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_DEMANGLER_HPP

#include <cstdio>
#include <chrono>
#include <fstream>
#include <cerrno>
#include <cxxabi.h>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::logging
{
    class Demangler
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:

        Demangler() noexcept;
        Demangler(const Demangler &)             = default;
        Demangler(Demangler &&)                  = default;
        Demangler & operator=(const Demangler &) = default;
        Demangler & operator=(Demangler &&)      = default;
        virtual ~Demangler() noexcept;

        template <typename T>
        std::string typeName();
        template <class T>
        std::string typeName(T &val);

    protected:
    private:
    };
}
#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_DEMANGLER_HPP
