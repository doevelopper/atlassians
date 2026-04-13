
#ifndef CFS_OSAL_QUANTUM_CLEANUPTEST_HPP
#define CFS_OSAL_QUANTUM_CLEANUPTEST_HPP

#include <gtest/gtest.h>
#include <com/github/doevelopper/atlassians/atlas/security/SslCrypto.hpp>

namespace com::github::doevelopper::night::owl::security::test
{

    class SslCryptoTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        SslCryptoTest( ) noexcept;
        SslCryptoTest(const SslCryptoTest&) = default;
        SslCryptoTest(SslCryptoTest&&) = default;
        SslCryptoTest& operator=(const SslCryptoTest&) = default;
        SslCryptoTest& operator=(SslCryptoTest&&) = default;
        virtual ~SslCryptoTest() noexcept;

        void SetUp() override;
        void TearDown() override;
    protected:
        std::unique_ptr<com::github::doevelopper::night::owl::security::SslCrypto> m_targetUnderTest;
    private:
        // Q_DISABLE_COPY_MOVE ( CleanupTest )
    };
}
#endif
