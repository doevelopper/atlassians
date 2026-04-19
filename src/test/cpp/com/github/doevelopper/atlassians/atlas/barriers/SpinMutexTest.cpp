
#include <com/github/doevelopper/atlassians/atlas/barriers/SpinMutexTest.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <type_traits>

using namespace  com::github::doevelopper::night::owl::barriers;
using namespace  com::github::doevelopper::night::owl::barriers::test;

log4cxx::LoggerPtr SpinMutexTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.barriers.test.SpinMutexTest"));

SpinMutexTest::SpinMutexTest()
    : m_targetUnderTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

SpinMutexTest::~SpinMutexTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

void SpinMutexTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    this->m_targetUnderTest = new SpinMutex();
}

void SpinMutexTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    if(this->m_targetUnderTest) {
        delete this->m_targetUnderTest;
        this->m_targetUnderTest = nullptr;
    }
}

TEST_F(SpinMutexTest, test_assert)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // ASSERT_LT(sizeof(SpinMutex),sizeof(char));
}

TEST_F(SpinMutexTest, test_lock_and_unlock)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    this->m_targetUnderTest->lock();
    this->m_targetUnderTest->unlock();

    SpinMutex a {};
    SpinMutex b {};
    a.lock();
    b.lock();
    a.unlock();
    b.unlock();
}

TEST_F(SpinMutexTest, test_try_lock_and_unlock)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    EXPECT_TRUE(this->m_targetUnderTest->try_lock());
    EXPECT_FALSE(this->m_targetUnderTest->try_lock());
    EXPECT_FALSE(this->m_targetUnderTest->try_lock());
    EXPECT_FALSE(this->m_targetUnderTest->try_lock());

    this->m_targetUnderTest->unlock();
    EXPECT_TRUE(this->m_targetUnderTest->try_lock());
    this->m_targetUnderTest->unlock();
}

TEST_F(SpinMutexTest, test_try_lock_when_already_locked)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    SpinMutex b {};

    this->m_targetUnderTest->lock();
    EXPECT_FALSE(this->m_targetUnderTest->try_lock());

    b.lock();
    EXPECT_FALSE(this->m_targetUnderTest->try_lock());
    EXPECT_FALSE(b.try_lock());

    this->m_targetUnderTest->unlock();
    b.unlock();
}

template <typename Range, typename RandomNumberGenerator>
auto& pick_random(Range const& range, RandomNumberGenerator& rng)
{
    auto first = std::begin(range);
    auto last = std::end(range);
    assert(first != last && "can't pick a random element if the range is empty");
    auto const dist = std::distance(first, last);
    auto random = std::next(first, rng() % dist);
    assert(random != last && "can never happen because we picked within the range");
    return *random;
}

struct Record
{
    std::string string;
    SpinMutex mutex;
};



TEST_F(SpinMutexTest, test_multithreaded_lock)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    std::set<std::string> const valid_strings
    {
        "foo", "bar", "baz", "dinosaur", "battery", "multithreaded", "access",
        "I", "hate", "deadlocks", "and", "I'll", "be", "incredibly", "careful",
        "when", "using", "this", "class",

        "long string that takes a while to copy and hence has more chances to "
        "catch a thread in the middle of a copy xxxxxxxxxxxxxxxxxxxxxxxxxxxxx "
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    };

    std::random_device rd;
    std::mt19937 g(rd());
    Record record{pick_random(valid_strings, g)};

    std::size_t const THREADS = 4;
    std::vector<std::thread> threads;

    std::generate_n(std::back_inserter(threads), THREADS, [&record, valid_strings] {
      return std::thread{[&record, valid_strings] {
        std::random_device rd;
        std::mt19937 g(rd());

        for (int i = 0; i != 1000; ++i)
        {
          auto& s = pick_random(valid_strings, g);
          record.mutex.lock();
            EXPECT_TRUE(valid_strings.count(record.string));
            LOG4CXX_WARN(logger, valid_strings.count(record.string));
          record.string = s;
          record.mutex.unlock();
        }
      }};
    });

    for (auto& thread : threads) {
        thread.join();
    }
}

TEST_F(SpinMutexTest, test_default_constructed_to_an_unlocked_state)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    char* memory = static_cast<char*>(std::malloc(sizeof(SpinMutex)));
    std::uninitialized_fill_n(memory, sizeof(SpinMutex), 1);
    SpinMutex * mutex = new (memory) SpinMutex; // default construct, do not value-initialize

    EXPECT_TRUE(mutex->try_lock());
    mutex->unlock();

    mutex->~SpinMutex();
    std::free(memory);
}