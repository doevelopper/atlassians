
#include <com/github/doevelopper/night/owl/quota/OutOfMemoryAllocatorTest.hpp>

using namespace  com::github::doevelopper::night::owl::quota;
using namespace  com::github::doevelopper::night::owl::quota::test;

log4cxx::LoggerPtr OutOfMemoryAllocatorTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.quota.test.OutOfMemoryAllocatorTest"));

OutOfMemoryAllocatorTest::OutOfMemoryAllocatorTest()
    // : m_targetUnderTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

OutOfMemoryAllocatorTest::~OutOfMemoryAllocatorTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

void OutOfMemoryAllocatorTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    // this->m_targetUnderTest = new SpinMutex();
}

void OutOfMemoryAllocatorTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    // if(this->m_targetUnderTest) {
    //     delete this->m_targetUnderTest;
    //     this->m_targetUnderTest = nullptr;
    // }
}
/*

struct OnDestruction {
  OnDestruction(std::function<void()> f) : callback(f) { }
  ~OnDestruction() { callback(); }
  std::function<void()> callback;
};

using ValueType = OnDestruction;
using OutOfMemoryAllocator = utils::oom_allocator<std::allocator<ValueType>>;
using Allocator = amz::deferred_reclamation_allocator<OutOfMemoryAllocator>;

TEST_CASE("objects are still freed when we're in low memory conditions") {
  auto const test = [](auto timeout, std::size_t delay_buffer_size, std::size_t overflow) {
    bool oom_flag = false;
    std::size_t const allocations = delay_buffer_size * 10 + overflow;
    std::vector<ValueType*> pointers; pointers.resize(allocations, nullptr);
    std::vector<bool> was_destroyed; was_destroyed.resize(allocations, false);

    {
      Allocator allocator{OutOfMemoryAllocator{oom_flag}, timeout, delay_buffer_size};

      // Allocate a bunch of stuff.
      for (std::size_t i = 0; i != allocations; ++i) {
        ValueType* p = allocator.allocate(1);
        allocator.construct(p, [&was_destroyed, i]{ was_destroyed[i] = true; });
        pointers[i] = p;
      }

      // Deallocate half of it. This will add some entries to the delay list.
      std::size_t const first_half = allocations / 2;
      for (std::size_t i = 0; i != first_half; ++i) {
        allocator.destroy(pointers[i]);
        allocator.deallocate(pointers[i], 1);
      }

      // Put the underlying allocator in out-of-memory situation, and deallocate
      // the rest.
      std::size_t const second_half = allocations - first_half;
      oom_flag = true;
      for (std::size_t i = first_half; i != allocations; ++i) {
        allocator.destroy(pointers[i]);
        allocator.deallocate(pointers[i], 1);
      }

      // make sure a `bad_alloc` was thrown at least once, otherwise the unit test is moot
      REQUIRE(oom_flag == false);
    }

    for (bool destroyed : was_destroyed) {
      REQUIRE(destroyed);
    }
  };

  for (std::size_t delay_buffer_size : {1, 2, 10, 100}) {
    for (std::size_t overflow : {0, 1, 2, 10}) {
      test(std::chrono::milliseconds{10}, delay_buffer_size, overflow);
    }
  }
}
 */