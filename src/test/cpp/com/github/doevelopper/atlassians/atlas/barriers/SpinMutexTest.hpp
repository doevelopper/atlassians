
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_BARRIERS_SPINMUTEXTEST_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_BARRIERS_SPINMUTEXTEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/night/owl/barriers/SpinMutex.hpp>

namespace  com::github::doevelopper::night::owl::barriers::test
{
    class SpinMutexTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER
         public:

        SpinMutexTest();
        SpinMutexTest(const SpinMutexTest&) = delete;
        SpinMutexTest(SpinMutexTest&&) = delete;
        SpinMutexTest& operator=(const SpinMutexTest&) = delete;
        SpinMutexTest& operator=(SpinMutexTest&&) = delete;
        virtual ~SpinMutexTest();

        void SetUp() override;
        void TearDown() override;

    protected:

        com::github::doevelopper::night::owl::barriers::SpinMutex * m_targetUnderTest;
    private:

    };
}
#endif