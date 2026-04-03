#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_FIXTURE_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_FIXTURE_HPP

#include <gtest/gtest.h>
#include <com/github/doevelopper/atlassians/atlas/concurrency/TestConfiguration.hpp>
// #include <com/github/doevelopper/atlassians/atlas/concurrency/DispatcherSingleton.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Dispatcher.hpp>

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

namespace com::github::doevelopper::atlassians::atlas::concurrency::test
{
    class DispatcherFixture : public ::testing::TestWithParam<TestConfiguration>
    {

        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        DispatcherFixture() noexcept;
        DispatcherFixture(const DispatcherFixture &) = default;
        DispatcherFixture(DispatcherFixture &&) = default;
        DispatcherFixture &operator=(const DispatcherFixture &) = default;
        DispatcherFixture &operator=(DispatcherFixture &&) = default;
        virtual ~DispatcherFixture() noexcept;

        void SetUp() override;
        void TearDown() override;
        Bloomberg::quantum::Dispatcher & getDispatcher();

    protected:
        Bloomberg::quantum::Dispatcher * m_dispatcher;
    };
}

#endif