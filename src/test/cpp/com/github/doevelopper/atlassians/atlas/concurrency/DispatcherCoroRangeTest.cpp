
#include <com/github/doevelopper/premisses/atlas/concurrency/DispatcherCoroRangeTest.hpp>
#include <com/github/doevelopper/premisses/atlas/concurrency/Dispatcher.hpp>

using namespace com::github::doevelopper::premisses::atlas::concurrency;
using namespace com::github::doevelopper::premisses::atlas::concurrency::test;

log4cxx::LoggerPtr DispatcherCoroRangeTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.test.DispatcherCoroRangeTest" ) );

DispatcherCoroRangeTest::DispatcherCoroRangeTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

DispatcherCoroRangeTest::~DispatcherCoroRangeTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
}

void DispatcherCoroRangeTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    configuration.setNumCoroutineThreads(10);
}

void DispatcherCoroRangeTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

TEST_F(DispatcherCoroRangeTest, DefaultAnyCoroQueueIdRange1) 
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    Bloomberg::quantum::Dispatcher dispatcher(configuration);

    EXPECT_EQ(0, dispatcher.getCoroQueueIdRangeForAny().first);
    EXPECT_EQ(9, dispatcher.getCoroQueueIdRangeForAny().second);
}
//*
TEST_F(DispatcherCoroRangeTest, ValidAnyCoroQueueIdRange1)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    configuration.setCoroQueueIdRangeForAny(std::make_pair(2,3));
    Bloomberg::quantum::Dispatcher dispatcher(configuration);
    
    EXPECT_EQ(2, dispatcher.getCoroQueueIdRangeForAny().first);
    EXPECT_EQ(3, dispatcher.getCoroQueueIdRangeForAny().second);
}

TEST_F(DispatcherCoroRangeTest, ValidAnyCoroQueueIdRange2)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    configuration.setCoroQueueIdRangeForAny(std::make_pair(1,8));
    Bloomberg::quantum::Dispatcher dispatcher(configuration);
    
    EXPECT_EQ(1, dispatcher.getCoroQueueIdRangeForAny().first);
    EXPECT_EQ(8, dispatcher.getCoroQueueIdRangeForAny().second);
}

TEST_F(DispatcherCoroRangeTest, ValidAnyCoroQueueIdRange3)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    configuration.setCoroQueueIdRangeForAny(std::make_pair(1,1));
    Bloomberg::quantum::Dispatcher dispatcher(configuration);
    
    EXPECT_EQ(1, dispatcher.getCoroQueueIdRangeForAny().first);
    EXPECT_EQ(1, dispatcher.getCoroQueueIdRangeForAny().second);
}

TEST_F(DispatcherCoroRangeTest, EmptyAnyCoroQueueIdRange1)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    configuration.setCoroQueueIdRangeForAny(std::make_pair(1,0));
    Bloomberg::quantum::Dispatcher dispatcher(configuration);
    
    EXPECT_EQ(0, dispatcher.getCoroQueueIdRangeForAny().first);
    EXPECT_EQ(9, dispatcher.getCoroQueueIdRangeForAny().second);
}

TEST_F(DispatcherCoroRangeTest, EmptyAnyCoroQueueIdRange2)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    configuration.setCoroQueueIdRangeForAny(std::make_pair(10,9));
    Bloomberg::quantum::Dispatcher dispatcher(configuration);
    
    EXPECT_EQ(0, dispatcher.getCoroQueueIdRangeForAny().first);
    EXPECT_EQ(9, dispatcher.getCoroQueueIdRangeForAny().second);
}

TEST_F(DispatcherCoroRangeTest, EmptyAnyCoroQueueIdRange3)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    configuration.setNumCoroutineThreads(0);
    configuration.setCoroQueueIdRangeForAny(std::make_pair(10,9));
    Bloomberg::quantum::Dispatcher dispatcher(configuration);
    
    EXPECT_EQ(0, dispatcher.getCoroQueueIdRangeForAny().first);
    EXPECT_EQ(0, dispatcher.getCoroQueueIdRangeForAny().second);
}

TEST_F(DispatcherCoroRangeTest, InvalidAnyCoroQueueIdRange1)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    configuration.setCoroQueueIdRangeForAny(std::make_pair(20,30));
    Bloomberg::quantum::Dispatcher dispatcher(configuration);
    
    EXPECT_EQ(0, dispatcher.getCoroQueueIdRangeForAny().first);
    EXPECT_EQ(9, dispatcher.getCoroQueueIdRangeForAny().second);
}

TEST_F(DispatcherCoroRangeTest, InvalidAnyCoroQueueIdRange2)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    configuration.setNumCoroutineThreads(0);
    configuration.setCoroQueueIdRangeForAny(std::make_pair(20,30));
    Bloomberg::quantum::Dispatcher dispatcher(configuration);
    
    EXPECT_EQ(0, dispatcher.getCoroQueueIdRangeForAny().first);
    EXPECT_EQ(0, dispatcher.getCoroQueueIdRangeForAny().second);
}
//*/