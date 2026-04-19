
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_TIME_FAKECLOCKTEST_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_TIME_FAKECLOCKTEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/atlassians/atlas/time/FakeClock.hpp>

namespace  com::github::doevelopper::night::owl::time::test
{
    class FakeClockTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER
         public:

        FakeClockTest();
        FakeClockTest(const FakeClockTest&) = delete;
        FakeClockTest(FakeClockTest&&) = delete;
        FakeClockTest& operator=(const FakeClockTest&) = delete;
        FakeClockTest& operator=(FakeClockTest&&) = delete;
        virtual ~FakeClockTest();

        void SetUp() override;
        void TearDown() override;

    protected:
        com::github::doevelopper::night::owl::extentions::test::FakeClock * m_targetUnderTest;
    private:

    };
}
#endif