
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUOTA_DEFERREDRECLAMATIONALLOCATORTEST_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUOTA_DEFERREDRECLAMATIONALLOCATORTEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/atlassians/atlas/quota/DeferredReclamationAllocator.hpp>

namespace  com::github::doevelopper::night::owl::quota::test
{

    /*!
     * \brief An allocator that refuses to free what's been allocated by anyone but itself.
     * \tparam T
     */
    template <typename T>
    struct SelfCompatibleAllocator : std::allocator<T>
    {
        bool operator==(SelfCompatibleAllocator const & other) const
        {
            return &other == this;
        }

        bool operator!=(SelfCompatibleAllocator const & other) const
        {
            return !(*this == other);
        }
    };

    // An allocator that is always equal to another allocator of the same type.
    template <typename T>
    using AlwaysEqualAllocator = std::allocator<T>;
    /*!
     * \brief
     */
    using ContainerTypes = ::testing::Types<int, std::uint8_t>;

    template <typename T>
    class DeferredReclamationAllocatorTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER
         public:

        DeferredReclamationAllocatorTest();
        DeferredReclamationAllocatorTest(const DeferredReclamationAllocatorTest&) = delete;
        DeferredReclamationAllocatorTest(DeferredReclamationAllocatorTest&&) = delete;
        DeferredReclamationAllocatorTest& operator=(const DeferredReclamationAllocatorTest&) = delete;
        DeferredReclamationAllocatorTest& operator=(DeferredReclamationAllocatorTest&&) = delete;
        virtual ~DeferredReclamationAllocatorTest();

        void SetUp() override;
        void TearDown() override;

    protected:

        // com::github::doevelopper::night::owl::quota::DeferredReclamationAllocator * m_targetUnderTest;
    private:

    };
    TYPED_TEST_SUITE_P(DeferredReclamationAllocatorTest);
}
#endif