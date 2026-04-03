
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_CRC32CTEST_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_CRC32CTEST_HPP

#include <gtest/gtest.h>
#include <com/github/doevelopper/night/owl/security/Crc32C.hpp>

namespace com::github::doevelopper::night::owl::security::test
{

    class Crc32CTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        Crc32CTest( ) noexcept;
        Crc32CTest(const Crc32CTest&) = default;
        Crc32CTest(Crc32CTest&&) = default;
        Crc32CTest& operator=(const Crc32CTest&) = default;
        Crc32CTest& operator=(Crc32CTest&&) = default;
        virtual ~Crc32CTest() noexcept;

        void SetUp() override;
        void TearDown() override;
    protected:
        std::unique_ptr<com::github::doevelopper::night::owl::security::Crc32C> m_targetUnderTest;
    private:
        // Q_DISABLE_COPY_MOVE ( CleanupTest )
    };
}
#endif
