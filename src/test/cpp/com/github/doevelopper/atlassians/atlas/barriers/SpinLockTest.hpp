
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_BARRIERS_SPINLOCKTEST_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_BARRIERS_SPINLOCKTEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/night/owl/barriers/SpinLock.hpp>

namespace  com::github::doevelopper::night::owl::barriers::test
{
    class SpinLockTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER
         public:

        SpinLockTest();
        SpinLockTest(const SpinLockTest&) = delete;
        SpinLockTest(SpinLockTest&&) = delete;
        SpinLockTest& operator=(const SpinLockTest&) = delete;
        SpinLockTest& operator=(SpinLockTest&&) = delete;
        virtual ~SpinLockTest();

        void SetUp() override;
        void TearDown() override;

    protected:
        com::github::doevelopper::night::owl::barriers::SpinLock * m_targetUnderTest;
    private:

    };
}
#endif