
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUOTA_OUTOFMEMORYALLOCATORTEST_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUOTA_OUTOFMEMORYALLOCATORTEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/night/owl/quota/OutOfMemoryAllocator.hpp>

namespace  com::github::doevelopper::night::owl::quota::test
{
    class OutOfMemoryAllocatorTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER
         public:

        OutOfMemoryAllocatorTest();
        OutOfMemoryAllocatorTest(const OutOfMemoryAllocatorTest&) = delete;
        OutOfMemoryAllocatorTest(OutOfMemoryAllocatorTest&&) = delete;
        OutOfMemoryAllocatorTest& operator=(const OutOfMemoryAllocatorTest&) = delete;
        OutOfMemoryAllocatorTest& operator=(OutOfMemoryAllocatorTest&&) = delete;
        virtual ~OutOfMemoryAllocatorTest();

        void SetUp() override;
        void TearDown() override;

    protected:

        // com::github::doevelopper::night::owl::quota::DeferredReclamationAllocator * m_targetUnderTest;
    private:

    };
}
#endif