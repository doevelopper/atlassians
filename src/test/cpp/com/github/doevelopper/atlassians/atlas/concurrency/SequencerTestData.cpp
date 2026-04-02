
#include <com/github/doevelopper/atlassians/atlas/concurrency/SequencerTestData.hpp>

using namespace com::github::doevelopper::atlassians::atlas::concurrency;
using namespace com::github::doevelopper::atlassians::atlas::concurrency::test;

log4cxx::LoggerPtr SequencerTestData::logger = 
    log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.atlassians.atlas.concurrency.test.SequencerTestData" ) );

SequencerTestData::SequencerTestData ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

SequencerTestData::~SequencerTestData ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
}

void 
SequencerTestData::ensureOrder(TaskId beforeTaskId, TaskId afterTaskId)
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    SequencerTestData::TaskResultMap::const_iterator beforeTaskIt = m_results.find(beforeTaskId);
    ASSERT_NE(beforeTaskIt, m_results.end());

    SequencerTestData::TaskResultMap::const_iterator afterTaskIt = m_results.find(afterTaskId);
    ASSERT_NE(afterTaskIt, m_results.end());

    if (beforeTaskIt == m_results.end() || afterTaskIt == m_results.end())
    {
        LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__  << " Before or after task end");
        return;
    }
    EXPECT_LE(beforeTaskIt->second.endTime, afterTaskIt->second.startTime);    
}

std::function<int(Bloomberg::quantum::VoidContextPtr)> 
SequencerTestData::makeTask(TaskId taskId)
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 

    return [this, taskId](Bloomberg::quantum::VoidContextPtr ctx)->int
    {
        taskFunc(ctx, taskId, nullptr, "");
        return (EXIT_SUCCESS);
    };
}

std::function<int(Bloomberg::quantum::VoidContextPtr)> 
SequencerTestData::makeTaskWithBlock(TaskId taskId, std::atomic<bool>* blockFlag)
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    return [this, taskId, blockFlag](Bloomberg::quantum::VoidContextPtr ctx)->int
    {
        taskFunc(ctx, taskId, blockFlag, "");
        return (EXIT_SUCCESS);
    };
}

std::function<int(Bloomberg::quantum::VoidContextPtr)> 
SequencerTestData::makeTaskWithException(TaskId taskId, std::string error)
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    return [this, taskId, error](Bloomberg::quantum::VoidContextPtr ctx)->int
    {
        taskFunc(ctx, taskId, nullptr, error);
        return (EXIT_SUCCESS);
    };
}

SequencerTestData::TaskResultMap & 
SequencerTestData::results()
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    return m_results;
}

void 
SequencerTestData::sleep(int periodCount)
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    std::this_thread::sleep_for(std::chrono::milliseconds(periodCount*1));
}

void 
SequencerTestData::taskFunc(Bloomberg::quantum::VoidContextPtr ctx, TaskId id, std::atomic<bool>* blockFlag, std::string error)
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    do 
    {
        ctx->sleep(std::chrono::milliseconds(1));

        if (not error.empty()) 
        {
            throw std::runtime_error(error);
        }
    }
    while(blockFlag and *blockFlag);

    std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();

    // update the task map with the time stats
    Bloomberg::quantum::Mutex::Guard lock(ctx, m_resultMutex);
        
    m_results[id].startTime = startTime;
    m_results[id].endTime = endTime;
}