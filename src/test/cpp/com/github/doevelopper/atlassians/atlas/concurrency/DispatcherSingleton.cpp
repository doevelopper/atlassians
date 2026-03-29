
#include <com/github/doevelopper/premisses/atlas/concurrency/DispatcherSingleton.hpp>

using namespace com::github::doevelopper::premisses::atlas::concurrency;
using namespace com::github::doevelopper::premisses::atlas::concurrency::test;

log4cxx::LoggerPtr DispatcherSingleton::logger = 
    log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.test.DispatcherSingleton" ) );

using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

constexpr int DispatcherSingleton::numCoro;
constexpr int DispatcherSingleton::numThreads;
DispatcherSingleton::DispatcherMap DispatcherSingleton::m_dispatchers;

DispatcherSingleton::DispatcherSingleton ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

DispatcherSingleton::~DispatcherSingleton ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
}

std::shared_ptr<Bloomberg::quantum::Dispatcher> 
DispatcherSingleton::createInstance(const TestConfiguration& taskConfig)
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
    Bloomberg::quantum::Configuration config;
    config.setNumCoroutineThreads(numCoro);
    config.setNumIoThreads(numThreads);
    config.setLoadBalanceSharedIoQueues(taskConfig.loadBalance());
    config.setLoadBalancePollIntervalMs(std::chrono::milliseconds(10));
    config.setCoroQueueIdRangeForAny(std::make_pair(1,numCoro-1));
    config.setCoroutineSharingForAny(taskConfig.coroutineSharingForAny());
    return std::make_shared<Bloomberg::quantum::Dispatcher>(config);    
}

Bloomberg::quantum::Dispatcher & 
DispatcherSingleton::instance(const TestConfiguration& config)
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    auto it = m_dispatchers.find(config);
    if (it == m_dispatchers.end())
    {                
        // it = m_dispatchers.emplace(config, createInstance(config)).first; //???
    }
    return *it->second;
}

void DispatcherSingleton::deleteInstances()
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    m_dispatchers.clear();
}
