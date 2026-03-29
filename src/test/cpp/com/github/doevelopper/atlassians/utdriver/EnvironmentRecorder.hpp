#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_CUSTOMTESTENVIRONMENT_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_CUSTOMTESTENVIRONMENT_HPP

#include <gtest/gtest.h>
#include <unistd.h>
#include <limits.h>
#include <cstdlib>

#include <log4cxx/logger.h>
#include <sys/utsname.h>

namespace com::github::doevelopper::premisses::utdriver
{
    class EnvironmentRecorder : public ::testing::Environment
    {
    public:
        void SetUp() override;
        void TearDown() override;
    protected:
    private:
        static log4cxx::LoggerPtr logger;
    };
}  // namespace com::github::doevelopper::premisses::utdriver

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_CUSTOMTESTENVIRONMENT_HPP
