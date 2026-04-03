
#include <com/github/doevelopper/atlassians/atlas/concurrency/AuxiliaryTest.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/TaskStateHandler.hpp>

using namespace com::github::doevelopper::atlassians::atlas::concurrency;
using namespace com::github::doevelopper::atlassians::atlas::concurrency::test;

log4cxx::LoggerPtr AuxiliaryTest::logger =
    log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.atlassians.atlas.concurrency.test.AuxiliaryTest" ) );

AuxiliaryTest::AuxiliaryTest ( ) noexcept

{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

AuxiliaryTest::~AuxiliaryTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

void AuxiliaryTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // testee = new ();
}

void AuxiliaryTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // delete testee;
}

TEST_F(AuxiliaryTest, IsIntersection)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // Check with different integral types
    EXPECT_TRUE(Bloomberg::quantum::isIntersection(long(1), long(1)));
    EXPECT_FALSE(Bloomberg::quantum::isIntersection(short(1), short(0)));
    EXPECT_TRUE(Bloomberg::quantum::isIntersection(int(1), int(3)));

    for (auto state : {
            Bloomberg::quantum::TaskState::Started,
            Bloomberg::quantum::TaskState::Suspended,
            Bloomberg::quantum::TaskState::Resumed,
            Bloomberg::quantum::TaskState::Stopped,
            Bloomberg::quantum::TaskState::All })
    {
        EXPECT_FALSE(Bloomberg::quantum::isIntersection(Bloomberg::quantum::TaskState::None, state));
        EXPECT_TRUE(Bloomberg::quantum::isIntersection(Bloomberg::quantum::TaskState::All, state));
        EXPECT_TRUE(Bloomberg::quantum::isIntersection(state, state));
    }

    EXPECT_FALSE(Bloomberg::quantum::isIntersection(Bloomberg::quantum::TaskState::Initialized,
                                                    Bloomberg::quantum::TaskState::All));

    EXPECT_FALSE(Bloomberg::quantum::isIntersection(Bloomberg::quantum::TaskState::Initialized,
                                                    Bloomberg::quantum::TaskState::Started));

    EXPECT_FALSE(Bloomberg::quantum::isIntersection(Bloomberg::quantum::TaskState::Started,
                                                    Bloomberg::quantum::TaskState::Suspended));

    EXPECT_FALSE(Bloomberg::quantum::isIntersection(Bloomberg::quantum::TaskState::Suspended,
                                                    Bloomberg::quantum::TaskState::Resumed));

    EXPECT_FALSE(Bloomberg::quantum::isIntersection(Bloomberg::quantum::TaskState::Resumed,
                                                    Bloomberg::quantum::TaskState::Stopped));

}

TEST_F(AuxiliaryTest, MakeExceptionSafe)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::function<void()> func1 = {};
    func1 = Bloomberg::quantum::makeExceptionSafe(func1);
    EXPECT_FALSE(func1);

    func1 = []()
    {
        throw std::runtime_error("Error");
    };

    func1 = Bloomberg::quantum::makeExceptionSafe(func1);
    EXPECT_TRUE(func1);
    EXPECT_NO_THROW(func1());


    std::function<void(int, char, std::unique_ptr<int>)> func2 =
    [](int, char, std::unique_ptr<int>)
    {
        throw std::runtime_error("Error");
    };
    func2 = Bloomberg::quantum::makeExceptionSafe(func2);
    EXPECT_TRUE(func2);
    EXPECT_NO_THROW(func2(1, 'a', std::make_unique<int>(-1)));
}