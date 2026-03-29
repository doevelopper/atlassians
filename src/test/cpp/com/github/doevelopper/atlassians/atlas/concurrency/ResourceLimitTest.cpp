
#include <com/github/doevelopper/premisses/atlas/concurrency/ResourceLimitTest.hpp>

using namespace com::github::doevelopper::premisses::atlas::concurrency::test;

log4cxx::LoggerPtr ResourceLimitTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.test.ResourceLimitTest" ) );

ResourceLimitTest::ResourceLimitTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

ResourceLimitTest::~ResourceLimitTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
}

TEST_P(ResourceLimitTest, RunWhenResourceIsAvailable)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
    int batch = Bloomberg::quantum::AllocatorTraits::defaultCoroPoolAllocSize()-10;
    int ioThreads = getDispatcher().getNumIoThreads();
    std::vector<Bloomberg::quantum::ThreadContextPtr<int>> futures, futures2;
    futures.reserve(batchNum);
    futures2.reserve(batchNum);
    
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < batch; ++i)
    {
        //Post IO-bound coroutine
        futures.push_back(getDispatcher().post([ioThreads, i](Bloomberg::quantum::VoidContextPtr ctx)->int
        {
            return ctx->postAsyncIo(i%ioThreads, false, []()->int {
              std::this_thread::sleep_for(std::chrono::milliseconds(5));
              return 0;
            })->get(ctx);
        }));
    }
    auto start2 = std::chrono::steady_clock::now();
    for (auto& f : futures) {
        f->get();
        //post CPU-bound coroutine as soon as we have a resource available
        futures2.push_back(getDispatcher().post([](Bloomberg::quantum::VoidContextPtr)->int { return 0; }));
    }
    getDispatcher().drain();
    auto stop = std::chrono::steady_clock::now();
    LOG4CXX_DEBUG( logger, __LOG4CXX_FUNC__ 
        << "total:" << std::chrono::duration_cast<std::chrono::microseconds>(stop-start).count()
        << " cpu-bound:" << std::chrono::duration_cast<std::chrono::microseconds>(stop-start2).count());
}

TEST_P(ResourceLimitTest, RunWhenAllResourcesAreAvailable)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
    int batch = Bloomberg::quantum::AllocatorTraits::defaultCoroPoolAllocSize()-10;
    int ioThreads = getDispatcher().getNumIoThreads();
    std::vector<Bloomberg::quantum::ThreadContextPtr<int>> futures, futures2;
    futures.reserve(batchNum);
    futures2.reserve(batchNum);
    
    auto start = std::chrono::steady_clock::now();
     for (int i = 0; i < batch; ++i)
     {
         //Post IO-bound coroutine
         futures.push_back(getDispatcher().post([ioThreads, i](Bloomberg::quantum::VoidContextPtr ctx)->int
         {
              return ctx->postAsyncIo(i%ioThreads, false, []()->int {
                  std::this_thread::sleep_for(std::chrono::milliseconds(5));
                  return 0;
              })->get(ctx);
          }));
     }
     getDispatcher().drain();
     auto start2 = std::chrono::steady_clock::now();
     for (int i = 0; i < batch; ++i)
     {
         //Post CPU-bound coroutine
         futures2.push_back(getDispatcher().post([](Bloomberg::quantum::VoidContextPtr)->int { return 0; }));
     }
     getDispatcher().drain();
     auto stop = std::chrono::steady_clock::now();

     LOG4CXX_DEBUG( logger, __LOG4CXX_FUNC__ 
        << "total:" << std::chrono::duration_cast<std::chrono::microseconds>(stop-start).count()
        << " cpu-bound:" << std::chrono::duration_cast<std::chrono::microseconds>(stop-start2).count());
}

TEST_P(ResourceLimitTest, RunAllAtOnce)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
    int batch = Bloomberg::quantum::AllocatorTraits::defaultCoroPoolAllocSize()-10;
    int ioThreads = getDispatcher().getNumIoThreads();
    std::vector<Bloomberg::quantum::ThreadContextPtr<int>> futures, futures2;
    futures.reserve(batchNum);
    futures2.reserve(batchNum);
    
    auto start = std::chrono::steady_clock::now();
     for (int i = 0; i < batch; ++i)
     {
         //Post IO coroutine
         futures.push_back(getDispatcher().post([ioThreads, i](Bloomberg::quantum::VoidContextPtr ctx)->int
         {
              return ctx->postAsyncIo(i%ioThreads, false, []()->int {
                  std::this_thread::sleep_for(std::chrono::milliseconds(5));
                  return 0;
              })->get(ctx);
          }));
         //Post CPU-bound coroutine
         futures2.push_back(getDispatcher().post([](Bloomberg::quantum::VoidContextPtr)->int { return 0; }));
     }
     auto start2 = std::chrono::steady_clock::now();
     for (auto& f : futures2) {
        f->get();
     }
     auto stop2 = std::chrono::steady_clock::now();
     getDispatcher().drain();
     auto stop = std::chrono::steady_clock::now();
     LOG4CXX_DEBUG( logger, __LOG4CXX_FUNC__ 
        << "total:" << std::chrono::duration_cast<std::chrono::microseconds>(stop-start).count()
        << " cpu-bound:" << std::chrono::duration_cast<std::chrono::microseconds>(stop2-start2).count());
}

TEST_P(ResourceLimitTest, NoCoroutines)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
    std::string s;
    int batch = Bloomberg::quantum::AllocatorTraits::defaultCoroPoolAllocSize()-10;
    int ioThreads = getDispatcher().getNumIoThreads();
    std::vector<Bloomberg::quantum::ThreadFuturePtr<int>> futures, futures2;
    futures.reserve(batchNum);
    futures2.reserve(batchNum);
    
    auto start = std::chrono::steady_clock::now();
     for (int i = 0; i < batch; ++i)
     {
         //Post IO task
         futures.push_back(getDispatcher().postAsyncIo(i%ioThreads, false, []()->int
         {
              std::this_thread::sleep_for(std::chrono::milliseconds(5));
              return 0;
         }));
         //Post CPU-bound task
         futures2.push_back(getDispatcher().postAsyncIo([]()->int { return 0; }));
     }
     auto start2 = std::chrono::steady_clock::now();
     for (auto& f : futures2) {
        f->get();
     }
     auto stop2 = std::chrono::steady_clock::now();
     getDispatcher().drain();
     auto stop = std::chrono::steady_clock::now();
     LOG4CXX_DEBUG( logger, __LOG4CXX_FUNC__ 
            << "total:" << std::chrono::duration_cast<std::chrono::microseconds>(stop-start).count()
            << " cpu-bound:" << std::chrono::duration_cast<std::chrono::microseconds>(stop2-start2).count());
}

INSTANTIATE_TEST_CASE_P(ResourceLimit_Default,
                        ResourceLimitTest,
                        ::testing::Values(TestConfiguration(false, false),
                                          TestConfiguration(false, true)));    