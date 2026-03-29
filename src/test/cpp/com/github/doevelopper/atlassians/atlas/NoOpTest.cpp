
#include <com/github/doevelopper/premisses/atlas/NoOpTest.hpp>

using namespace com::github::doevelopper::premisses::atlas::utils;
using namespace com::github::doevelopper::premisses::atlas::utils::test;

// log4cxx::LoggerPtr EventSystemTest::logger =
//     log4cxx::Logger::getLogger(std::string("cfs.edar.test.EventSystemTest"));

NoOpTest::NoOpTest() : testee() {
  //   LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

NoOpTest::~NoOpTest() {
  //   LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void NoOpTest::SetUp() {
  //   LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  testee = std::make_unique<NoOp>();
  ASSERT_NE(testee, nullptr);
}

void NoOpTest::TearDown() {
  //   LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  testee.reset();
}

TEST_F(NoOpTest, BoolConversion_IsFalse) {
  // ASSERT_NE(testee, nullptr);
  // EXPECT_FALSE(static_cast<bool>(*testee));
}

TEST_F(NoOpTest, NotOperator_IsTrue) {
  // ASSERT_NE(testee, nullptr);
  // EXPECT_TRUE(!(*testee));
}

TEST_F(NoOpTest, CallOperator_AcceptsArguments_DoesNotThrow) {
  // ASSERT_NE(testee, nullptr);
  // EXPECT_NO_THROW((*testee)(1, 2.0, "abc"));
}

TEST_F(NoOpTest, LockUnlock_DoNotThrow) {
  // ASSERT_NE(testee, nullptr);
  // EXPECT_NO_THROW(testee->lock());
  // EXPECT_NO_THROW(testee->unlock());
}
