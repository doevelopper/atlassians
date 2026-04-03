
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUOTA_BOUNDEDALLOCATORTEST_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUOTA_BOUNDEDALLOCATORTEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/night/owl/quota/BoundedAllocator.hpp>

namespace  com::github::doevelopper::night::owl::quota::test
{
    class BoundedAllocatorTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER
         public:

        BoundedAllocatorTest();
        BoundedAllocatorTest(const BoundedAllocatorTest&) = delete;
        BoundedAllocatorTest(BoundedAllocatorTest&&) = delete;
        BoundedAllocatorTest& operator=(const BoundedAllocatorTest&) = delete;
        BoundedAllocatorTest& operator=(BoundedAllocatorTest&&) = delete;
        virtual ~BoundedAllocatorTest();

        void SetUp() override;
        void TearDown() override;

    protected:

        // com::github::doevelopper::night::owl::quota::DeferredReclamationAllocator * m_targetUnderTest;
    private:

    };
}
#endif