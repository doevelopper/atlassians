#include <com/github/doevelopper/atlassians/quota/security/Crc32CTest.hpp>

using namespace com::github::doevelopper::night::owl::security;
using namespace com::github::doevelopper::night::owl::security::test;

log4cxx::LoggerPtr Crc32CTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.security.test.Crc32CTest"));

Crc32CTest::Crc32CTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

Crc32CTest::~Crc32CTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

void Crc32CTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    // this->m_targetUnderTest = new SslCrypto();
    this->m_targetUnderTest = std::make_unique<Crc32C>();
}

void Crc32CTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    // if(this->m_targetUnderTest) {
    //     delete this->m_targetUnderTest;
    //     this->m_targetUnderTest = nullptr;
    // }
}

TEST_F(Crc32CTest, test_Construction)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    EXPECT_TRUE(this->m_targetUnderTest);
}
