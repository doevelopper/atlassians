
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_TIME_TIMEFIXTURETEST_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_TIME_TIMEFIXTURETEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/atlassians/events/observer/Value.hpp>

namespace  com::github::doevelopper::night::owl::time::test
{
    class TimeFixtureTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER
         public:

        TimeFixtureTest();
        TimeFixtureTest(const TimeFixtureTest&) = delete;
        TimeFixtureTest(TimeFixtureTest&&) = delete;
        TimeFixtureTest& operator=(const TimeFixtureTest&) = delete;
        TimeFixtureTest& operator=(TimeFixtureTest&&) = delete;
        virtual ~TimeFixtureTest();

        void SetUp() override;
        void TearDown() override;

    protected:
        // com::github::doevelopper::night::owl::observer::Value * m_targetUnderTest;
    private:

    };
}
#endif