
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_TESTCONFIGURATION_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_TESTCONFIGURATION_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/TaskStateHandler.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/LocalVariableGuard.hpp>
#include <condition_variable>
#include <mutex>

namespace com::github::doevelopper::atlassians::atlas::concurrency::test
{
    class TestConfiguration
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        TestConfiguration( ) noexcept;
        // TestConfiguration(const TestConfiguration&) = default;
        // TestConfiguration(TestConfiguration&&) = default;
        // TestConfiguration& operator=(const TestConfiguration&) = default;
        // TestConfiguration& operator=(TestConfiguration&&) = default;
        virtual ~TestConfiguration() noexcept;

        TestConfiguration(bool loadBalance, bool coroutineSharingForAny ,const Bloomberg::quantum::TaskStateConfiguration & taskStateConfiguration);

        bool loadBalance () const;
        void loadBalance (bool newValue);
        bool coroutineSharingForAny () const;
        void coroutineSharingForAny (bool newValue);

        bool operator == (const TestConfiguration& rhs) const
        {
            return m_loadBalance == rhs.m_loadBalance 
                && m_coroutineSharingForAny == rhs.m_coroutineSharingForAny;
        }
    protected:

    private:

        Q_DISABLE_COPY_MOVE ( TestConfiguration )
        bool m_loadBalance;
        bool m_coroutineSharingForAny;
        Bloomberg::quantum::TaskStateConfiguration m_taskStateConfiguration;
    };
}

namespace std 
{
    template<> 
    struct hash<com::github::doevelopper::atlassians::atlas::concurrency::test::TestConfiguration>
    {
        size_t operator()(const com::github::doevelopper::atlassians::atlas::concurrency::test::TestConfiguration & x) const
        {
            return std::hash<bool>()(x.loadBalance()) +
                std::hash<bool>()(x.coroutineSharingForAny());
        }
    };
}

#endif
