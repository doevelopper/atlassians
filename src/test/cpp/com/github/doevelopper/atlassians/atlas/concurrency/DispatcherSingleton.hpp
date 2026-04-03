
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_DISPATCHERSINGLETON_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_DISPATCHERSINGLETON_HPP

#include <com/github/doevelopper/atlassians/atlas/concurrency/TestConfiguration.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/quantum.h>

namespace com::github::doevelopper::atlassians::atlas::concurrency::test
{

    class DispatcherSingleton
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        DispatcherSingleton( ) noexcept;
        DispatcherSingleton(const DispatcherSingleton&) = default;
        DispatcherSingleton(DispatcherSingleton&&) = default;
        DispatcherSingleton& operator=(const DispatcherSingleton&) = default;
        DispatcherSingleton& operator=(DispatcherSingleton&&) = default;
        virtual ~DispatcherSingleton() noexcept;
       
        static std::shared_ptr<Bloomberg::quantum::Dispatcher> createInstance(const TestConfiguration& taskConfig);
        static Bloomberg::quantum::Dispatcher & instance(const TestConfiguration& config);
        static void deleteInstances();
        static constexpr int numCoro{4};
        static constexpr int numThreads{5};

    protected:

    private:
        using DispatcherMap = std::unordered_map<TestConfiguration, std::shared_ptr<Bloomberg::quantum::Dispatcher>>;
        static DispatcherMap m_dispatchers;
    };
}
#endif
