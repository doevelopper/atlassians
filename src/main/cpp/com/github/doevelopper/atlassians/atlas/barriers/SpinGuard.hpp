
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_BARRIERS_SPINGUARD_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_BARRIERS_SPINGUARD_HPP

#include <com/github/doevelopper/night/owl/barriers/SpinLock.hpp>

namespace  com::github::doevelopper::night::owl::barriers
{
    class SpinGuard
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
     public:
        SpinGuard() noexcept;
        SpinGuard(const SpinGuard&) noexcept = default;
        SpinGuard(SpinGuard&&) noexcept =  default;
        SpinGuard& operator=(const SpinGuard&) noexcept = default;
        SpinGuard& operator=(SpinGuard&&) noexcept = default;
        virtual ~SpinGuard() noexcept;
    protected:
    private:
    };
}
#endif
