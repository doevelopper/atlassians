
#include <chrono>
#include <cstddef>
#include <memory>

#include <com/github/doevelopper/atlassians/atlas/quota/OnDestruction.hpp>
#include <com/github/doevelopper/atlassians/quota/DeferredReclamationAllocatorTest.hpp>

using namespace  com::github::doevelopper::atlassians::atlas::quota;
using namespace  com::github::doevelopper::night::owl::quota::test;

template <typename T>
log4cxx::LoggerPtr DeferredReclamationAllocatorTest<T>::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.quota.test.DeferredReclamationAllocatorTest"));

template <typename T>
DeferredReclamationAllocatorTest<T>::DeferredReclamationAllocatorTest()
    // : m_targetUnderTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

template <typename T>
DeferredReclamationAllocatorTest<T>::~DeferredReclamationAllocatorTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

template <typename T>
void DeferredReclamationAllocatorTest<T>::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    // this->m_targetUnderTest = new SpinMutex();
}

template <typename T>
void DeferredReclamationAllocatorTest<T>::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    // if(this->m_targetUnderTest) {
    //     delete this->m_targetUnderTest;
    //     this->m_targetUnderTest = nullptr;
    // }
}

TYPED_TEST_P(DeferredReclamationAllocatorTest, test_an_allocator_should_be_equal_to_itself)
{
    TypeParam parameter;
    using Allocator = DeferredReclamationAllocator<SelfCompatibleAllocator<int>>;
    // Allocator alloc{std::chrono::microseconds{10}};
}

TYPED_TEST_P(DeferredReclamationAllocatorTest, test_making_a_copy_of_an_allocator_should_yield_an_allocator_that_compares_equal)
{
    // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

REGISTER_TYPED_TEST_SUITE_P(DeferredReclamationAllocatorTest,
        test_an_allocator_should_be_equal_to_itself,
        test_making_a_copy_of_an_allocator_should_yield_an_allocator_that_compares_equal
);

INSTANTIATE_TYPED_TEST_SUITE_P(DifferentAllocatorTypes, DeferredReclamationAllocatorTest, ContainerTypes);

/*

TEST_CASE("an allocator should be equal to itself") {
  using Allocator = amz::deferred_reclamation_allocator<SelfCompatibleAllocator<int>>;
  Allocator alloc{std::chrono::microseconds{10}};
  REQUIRE(alloc == alloc);
}

TEST_CASE("making a copy of an allocator should yield an allocator that compares equal") {
  using Allocator = amz::deferred_reclamation_allocator<AlwaysEqualAllocator<int>>;
  Allocator alloc{std::chrono::microseconds{10}};
  Allocator copy{alloc};
  REQUIRE(alloc == copy);
}

TEST_CASE("two allocators with equal allocators and timeouts should compare equal") {
  using Allocator = amz::deferred_reclamation_allocator<AlwaysEqualAllocator<int>>;
  Allocator alloc1{std::chrono::microseconds{10}};
  Allocator alloc2{std::chrono::microseconds{10}};
  REQUIRE(alloc1 == alloc2);
}

TEST_CASE("allocators with different timeouts should not compare equal") {
  using Allocator = amz::deferred_reclamation_allocator<AlwaysEqualAllocator<int>>;
  Allocator alloc1{std::chrono::microseconds{10}};
  Allocator alloc2{std::chrono::microseconds{11}};

  REQUIRE(alloc1 != alloc2);
}

TEST_CASE("allocators with different allocators should not compare equal") {
  using Allocator = amz::deferred_reclamation_allocator<SelfCompatibleAllocator<int>>;
  Allocator alloc1{std::chrono::microseconds{10}};
  Allocator alloc2{std::chrono::microseconds{10}};

  REQUIRE(alloc1 != alloc2);
}

TEST_CASE("allocators with both different timeouts and allocators should not compare equal") {
  using Allocator = amz::deferred_reclamation_allocator<SelfCompatibleAllocator<int>>;
  Allocator alloc1{std::chrono::microseconds{10}};
  Allocator alloc2{std::chrono::microseconds{11}};

  REQUIRE(alloc1 != alloc2);
}
 */

// TYPED_TEST_P(DeferredReclamationAllocatorTest, test_ctor_copy)
// {
//     // using ValueType = int;
//     // using UnderlyingAllocator = std::allocator<ValueType>;
//     // using Allocator = DeferredReclamationAllocator<UnderlyingAllocator>;
//     // auto const with_buffer_size = [](std::size_t buffer_size, std::size_t overflow) {
//     //     auto const timeout = std::chrono::microseconds{10};
//     //     Allocator alloc1{timeout};
//     //     Allocator alloc2{alloc1};
//     //
//     //     REQUIRE(alloc1 == alloc2);
//     //
//     //     std::size_t const allocations = buffer_size * 10 + overflow;
//     //     for (std::size_t i = 0; i != allocations; ++i) {
//     //         ValueType* p = alloc1.allocate(1);
//     //         alloc1.construct(p);
//     //
//     //         alloc2.destroy(p);
//     //         alloc2.deallocate(p, 1);
//     //     }
//     // };
//     //
//     // for (std::size_t overflow : {0, 1, 2}) {
//     //     with_buffer_size(1, overflow);
//     //     with_buffer_size(2, overflow);
//     //     with_buffer_size(20, overflow);
//     //     with_buffer_size(40, overflow);
//     //     with_buffer_size(100, overflow);
//     //     with_buffer_size(1000, overflow);
//     // }
// }

/*
using ValueType = int;
using UnderlyingAllocator = std::allocator<ValueType>;
using Allocator = amz::deferred_reclamation_allocator<UnderlyingAllocator>;

TEST_CASE("move constructing passes the resources to the newly-constructed allocator") {
auto const with_buffer_size = [](std::size_t buffer_size, std::size_t overflow) {
std::vector<ValueType*> pointers;

auto const timeout = std::chrono::microseconds{10};
Allocator alloc1{timeout};

std::size_t const allocations = buffer_size * 10 + overflow;
for (std::size_t i = 0; i != allocations; ++i) {
ValueType* p = alloc1.allocate(1);
alloc1.construct(p);
pointers.push_back(p);
}

Allocator alloc2{std::move(alloc1)};
for (std::size_t i = 0; i != allocations; ++i) {
alloc2.destroy(pointers[i]);
alloc2.deallocate(pointers[i], 1);
}
};

for (std::size_t overflow : {0, 1, 2}) {
with_buffer_size(1, overflow);
with_buffer_size(2, overflow);
with_buffer_size(20, overflow);
with_buffer_size(40, overflow);
with_buffer_size(100, overflow);
with_buffer_size(1000, overflow);
}
}
*/