#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_DEMO_CALCULATORTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_DEMO_CALCULATORTEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/atlassians/demo/Calculator.hpp>

namespace com::github::doevelopper::atlassians::demo::test
{
    class CalculatorTest : public ::testing::Test
    {
        // LOG4CXX_DECLARE_STATIC_TEST_LOGGER
    public:
        CalculatorTest();
        CalculatorTest(const CalculatorTest &) = delete;
        CalculatorTest(CalculatorTest &&) = delete;
        CalculatorTest &operator=(const CalculatorTest &) = delete;
        CalculatorTest &operator=(CalculatorTest &&) = delete;
        virtual ~CalculatorTest();

        void SetUp() override;
        void TearDown() override;

    protected:
        // Helper function to extract double from ResultType
        auto getDoubleResult(const Calculator::ResultType &result) -> std::optional<double>;

        // Helper function to extract string from ResultType
        auto getStringResult(const Calculator::ResultType &result) -> std::optional<std::string>;

        // Helper function to check if result is error
        auto isErrorResult(const Calculator::ResultType &result) -> bool;

        com::github::doevelopper::atlassians::demo::Calculator *m_targetUnderTest;

    private:
    };
}
#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_DEMO_CALCULATORTEST_HPP
