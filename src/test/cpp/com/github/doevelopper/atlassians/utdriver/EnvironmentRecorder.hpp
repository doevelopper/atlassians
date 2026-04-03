#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_ENVIRONMENTRECORDER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_ENVIRONMENTRECORDER_HPP

#include <gtest/gtest.h>
#include <unistd.h>
#include <limits.h>
#include <cstdlib>

#include <log4cxx/logger.h>
#include <sys/utsname.h>

namespace com::github::doevelopper::atlassians::utdriver
{
    class EnvironmentRecorder : public ::testing::Environment
    {
    public:
        EnvironmentRecorder() noexcept;
        ~EnvironmentRecorder() noexcept override;
        void SetUp() override;
        void TearDown() override;
    protected:
    private:
        static log4cxx::LoggerPtr logger;
    };
}  // namespace com::github::doevelopper::atlassians::utdriver

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_ENVIRONMENTRECORDER_HPP
