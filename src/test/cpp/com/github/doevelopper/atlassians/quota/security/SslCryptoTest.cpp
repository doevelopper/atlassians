#include <com/github/doevelopper/night/owl/security/SslCryptoTest.hpp>

using namespace com::github::doevelopper::night::owl::security;
using namespace com::github::doevelopper::night::owl::security::test;

log4cxx::LoggerPtr SslCryptoTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.security.test.SslCryptoTest"));

static const std::string key = "01234567890123456789012345678901";
static const std::string cipher = "aes-256-cbc";

SslCryptoTest::SslCryptoTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

SslCryptoTest::~SslCryptoTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

void SslCryptoTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    // this->m_targetUnderTest = new SslCrypto();
    this->m_targetUnderTest = std::make_unique<SslCrypto>();
}

void SslCryptoTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    // if(this->m_targetUnderTest) {
    //     delete this->m_targetUnderTest;
    //     this->m_targetUnderTest = nullptr;
    // }
}

TEST_F(SslCryptoTest, test_Construction)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    EXPECT_TRUE(this->m_targetUnderTest);

    // EXPECT_TRUE(!this->m_targetUnderTest->validateCryptoParams(cipher, key));
    //
    // std::string plaintext = "test string to encrypt";
    // std::string ciphertext;
    // EXPECT_TRUE(this->m_targetUnderTest->encrypt(cipher, key, plaintext, ciphertext));
    //
    // std::string decrypted_plaintext;
    // EXPECT_TRUE(this->m_targetUnderTest->decrypt(cipher, key, ciphertext, decrypted_plaintext));
    //
    //
    // EXPECT_EQ(decrypted_plaintext,plaintext);
}