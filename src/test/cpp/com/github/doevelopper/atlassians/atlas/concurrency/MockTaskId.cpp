
#include <com/github/doevelopper/atlassians/atlas/concurrency/MockTaskId.hpp>

using namespace com::github::doevelopper::atlassians::atlas::concurrency;
using namespace com::github::doevelopper::atlassians::atlas::concurrency::test;

log4cxx::LoggerPtr MockTaskId::logger =
    log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.atlassians.atlas.concurrency.test.MockTaskId" ) );

MockTaskId::MockTaskId ( ) noexcept
    : Bloomberg::quantum::TaskId()
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

MockTaskId::~MockTaskId ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
}

MockTaskId::MockTaskId(ThisThreadTag) 
    : Bloomberg::quantum::TaskId(Bloomberg::quantum::TaskId::ThisThreadTag{})
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
}

MockTaskId::MockTaskId(Bloomberg::quantum::CoroContextTag t) 
    : Bloomberg::quantum::TaskId(t)
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
}

MockTaskId::MockTaskId(Bloomberg::quantum::ThreadContextTag t) 
    : Bloomberg::quantum::TaskId(t)
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
}

void MockTaskId::assign() 
{ 
     LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
    assignCurrentThread(); 
}