
#ifndef CFS_OSAL_QUANTUM_PARAMTERSTEST_HPP
#define CFS_OSAL_QUANTUM_PARAMTERSTEST_HPP
#include <com/github/doevelopper/atlassians/atlas/concurrency/quantum.h>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Fixture.hpp>

namespace com::github::doevelopper::atlassians::atlas::concurrency::test
{

    class ParamtersTest : public DispatcherFixture
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        struct NonCopyable 
        {
            NonCopyable() = delete;
            NonCopyable(const std::string& str) : _str(str){}
            NonCopyable(const NonCopyable&) = delete;
            NonCopyable(NonCopyable&&) = default;
            NonCopyable& operator=(const NonCopyable&) = delete;
            NonCopyable& operator=(NonCopyable&&) = default;
            std::string _str;
        };

        ParamtersTest( ) noexcept;
        ParamtersTest(const ParamtersTest&) = default;
        ParamtersTest(ParamtersTest&&) = default;
        ParamtersTest& operator=(const ParamtersTest&) = default;
        ParamtersTest& operator=(ParamtersTest&&) = default;
        virtual ~ParamtersTest() noexcept;

    protected:

    private:
        // Q_DISABLE_COPY_MOVE ( ParamtersTest )
    };
}
#endif
