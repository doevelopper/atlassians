
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_GENERICFUTURETEST_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_GENERICFUTURETEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/premisses/atlas/concurrency/Dispatcher.hpp>
#include <com/github/doevelopper/premisses/atlas/concurrency/util/GenericFuture.hpp>

#include <condition_variable>
#include <mutex>

namespace com::github::doevelopper::premisses::atlas::concurrency::test
{
    class GenericFutureTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:
        GenericFutureTest( ) noexcept;
        GenericFutureTest(const GenericFutureTest&) = default;
        GenericFutureTest(GenericFutureTest&&) = default;
        GenericFutureTest& operator=(const GenericFutureTest&) = default;
        GenericFutureTest& operator=(GenericFutureTest&&) = default;
        virtual ~GenericFutureTest() noexcept;

        void SetUp() override;
        void TearDown() override;

    protected:

    private:

        // Q_DISABLE_COPY_MOVE ( GenericFutureTest )

    };

}
#endif
