#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_CRC32C_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_CRC32C_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::night::owl::security
{
    class Crc32C
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
 public:
        Crc32C() noexcept;
        Crc32C(const Crc32C&) noexcept = default;
        Crc32C(Crc32C&&) noexcept =  default;
        Crc32C& operator=(const Crc32C&) noexcept = default;
        Crc32C& operator=(Crc32C&&) noexcept = default;
        virtual ~Crc32C() noexcept;

    protected:
    private:
    };
}

#endif
