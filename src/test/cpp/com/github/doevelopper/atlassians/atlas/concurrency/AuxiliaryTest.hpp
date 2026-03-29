#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_AUXILIARYTEST_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_AUXILIARYTEST_HPP

#include <gtest/gtest.h>
#include <com/github/doevelopper/premisses/atlas/concurrency/Auxiliary.hpp>
#include <com/github/doevelopper/premisses/logging/LogMacros.hpp>

#include <condition_variable>
#include <functional>
#include <mutex>

namespace com::github::doevelopper::premisses::atlas::concurrency::test
{

    class AuxiliaryTest  : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:
        AuxiliaryTest( ) noexcept;
        AuxiliaryTest(const AuxiliaryTest&) = default;
        AuxiliaryTest(AuxiliaryTest&&) = default;
        AuxiliaryTest& operator=(const AuxiliaryTest&) = default;
        AuxiliaryTest& operator=(AuxiliaryTest&&) = default;
        virtual ~AuxiliaryTest() noexcept;

        void SetUp() override;
        void TearDown() override;
    protected:

    private:

    };
};


#endif
