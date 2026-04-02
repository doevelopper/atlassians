
#include <stack>
#include <stdexcept>
#include <string>
#include <memory>
#include <variant>
#include <optional>
#include <cmath>

#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <com/github/doevelopper/premisses/demo/Calculator.hpp>

using com::github::doevelopper::premisses::demo::Calculator;
using cucumber::ScenarioScope;

// ============================================
// Context Structure for Sharing State
// ============================================

struct CalculatorContext
{
    std::unique_ptr<Calculator> calculator;
    std::unique_ptr<Calculator> copiedCalculator;
    double lastNumericResult{0.0};
    Calculator::ResultType lastVariantResult;
    Calculator::Operation lastOperation{Calculator::Operation::Unknown};
    long long lastFactorialResult{0};
    double lastConstPowerResult{0.0};
    bool operationFailed{false};
    std::string lastErrorMessage;

    CalculatorContext()
        : lastVariantResult(0.0)
    {
    }

    // Helper to extract double from ResultType
    auto getDoubleFromResult(const Calculator::ResultType &result) -> std::optional<double>
    {
        if (std::holds_alternative<double>(result))
        {
            return std::get<double>(result);
        }
        return std::nullopt;
    }

    // Helper to extract string from ResultType
    auto getStringFromResult(const Calculator::ResultType &result) -> std::optional<std::string>
    {
        if (std::holds_alternative<std::string>(result))
        {
            return std::get<std::string>(result);
        }
        return std::nullopt;
    }

    // Helper to check if result is error
    auto isErrorResult(const Calculator::ResultType &result) -> bool
    {
        return std::holds_alternative<std::string>(result);
    }
};

// ============================================
// Background Steps
// ============================================

GIVEN("^a calculator$")
{
    ScenarioScope<CalculatorContext> context;
    context->calculator = std::make_unique<Calculator>();
    EXPECT_NE(context->calculator, nullptr);
    EXPECT_TRUE(context->calculator->isValid());
}

GIVEN("^I have a calculator named \"([^\"]*)\"$")
{
    REGEX_PARAM(std::string, name);
    ScenarioScope<CalculatorContext> context;

    context->calculator = std::make_unique<Calculator>(name);
    EXPECT_NE(context->calculator, nullptr);
    EXPECT_TRUE(context->calculator->isValid());
}

// ============================================
// Basic Arithmetic Operation Steps
// ============================================

WHEN("^I add (-?\\d+\\.?\\d*) and (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(double, a);
    REGEX_PARAM(double, b);
    ScenarioScope<CalculatorContext> context;

    context->lastNumericResult = context->calculator->add(a, b);
    context->lastVariantResult = context->calculator->calculate(Calculator::Operation::Add, a, b);
}

WHEN("^I subtract (-?\\d+\\.?\\d*) from (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(double, b);
    REGEX_PARAM(double, a);
    ScenarioScope<CalculatorContext> context;

    context->lastNumericResult = context->calculator->subtract(a, b);
    context->lastVariantResult = context->calculator->calculate(Calculator::Operation::Subtract, a, b);
}

WHEN("^I multiply (-?\\d+\\.?\\d*) by (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(double, a);
    REGEX_PARAM(double, b);
    ScenarioScope<CalculatorContext> context;

    context->lastNumericResult = context->calculator->multiply(a, b);
    context->lastVariantResult = context->calculator->calculate(Calculator::Operation::Multiply, a, b);
}

WHEN("^I divide (-?\\d+\\.?\\d*) by (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(double, a);
    REGEX_PARAM(double, b);
    ScenarioScope<CalculatorContext> context;

    auto result = context->calculator->divide(a, b);
    if (result.has_value())
    {
        context->lastNumericResult = result.value();
        context->operationFailed = false;
    }
    else
    {
        context->operationFailed = true;
    }

    context->lastVariantResult = context->calculator->calculate(Calculator::Operation::Divide, a, b);

    if (context->isErrorResult(context->lastVariantResult))
    {
        auto errorMsg = context->getStringFromResult(context->lastVariantResult);
        if (errorMsg.has_value())
        {
            context->lastErrorMessage = errorMsg.value();
        }
    }
}

WHEN("^I calculate (-?\\d+\\.?\\d*) to the power of (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(double, base);
    REGEX_PARAM(double, exponent);
    ScenarioScope<CalculatorContext> context;

    context->lastNumericResult = context->calculator->power(base, exponent);
    context->lastVariantResult = context->calculator->calculate(Calculator::Operation::Power, base, exponent);
}

WHEN("^I calculate (-?\\d+) modulo (-?\\d+)$")
{
    REGEX_PARAM(int, a);
    REGEX_PARAM(int, b);
    ScenarioScope<CalculatorContext> context;

    auto result = context->calculator->modulo(a, b);
    if (result.has_value())
    {
        context->lastNumericResult = static_cast<double>(result.value());
        context->operationFailed = false;
    }
    else
    {
        context->operationFailed = true;
    }

    context->lastVariantResult = context->calculator->calculate(
        Calculator::Operation::Modulo,
        static_cast<double>(a),
        static_cast<double>(b));

    if (context->isErrorResult(context->lastVariantResult))
    {
        auto errorMsg = context->getStringFromResult(context->lastVariantResult);
        if (errorMsg.has_value())
        {
            context->lastErrorMessage = errorMsg.value();
        }
    }
}

// ============================================
// Result Verification Steps
// ============================================

THEN("^the result is (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(double, expected);
    ScenarioScope<CalculatorContext> context;

    EXPECT_DOUBLE_EQ(context->lastNumericResult, expected);
}

THEN("^the result should be (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(double, expected);
    ScenarioScope<CalculatorContext> context;

    EXPECT_DOUBLE_EQ(context->lastNumericResult, expected);
}

THEN("^the operation should fail$")
{
    ScenarioScope<CalculatorContext> context;

    EXPECT_TRUE(context->operationFailed || context->isErrorResult(context->lastVariantResult));
}

THEN("^the error message should contain \"([^\"]*)\"$")
{
    REGEX_PARAM(std::string, expectedMessage);
    ScenarioScope<CalculatorContext> context;

    EXPECT_NE(context->lastErrorMessage.find(expectedMessage), std::string::npos);
}

THEN("^the result should be greater than (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(double, threshold);
    ScenarioScope<CalculatorContext> context;

    EXPECT_GT(context->lastNumericResult, threshold);
}

THEN("^the result should be greater than or equal to (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(double, threshold);
    ScenarioScope<CalculatorContext> context;

    EXPECT_GE(context->lastNumericResult, threshold);
}

// ============================================
// Operation Parsing Steps
// ============================================

WHEN("^I parse the operation \"([^\"]*)\"$")
{
    REGEX_PARAM(std::string, symbol);
    ScenarioScope<CalculatorContext> context;

    context->lastOperation = Calculator::parseOperation(symbol);
}

THEN("^the operation type should be \"([^\"]*)\"$")
{
    REGEX_PARAM(std::string, expectedOp);
    ScenarioScope<CalculatorContext> context;

    Calculator::Operation expected = Calculator::Operation::Unknown;

    if (expectedOp == "Add")
        expected = Calculator::Operation::Add;
    else if (expectedOp == "Subtract")
        expected = Calculator::Operation::Subtract;
    else if (expectedOp == "Multiply")
        expected = Calculator::Operation::Multiply;
    else if (expectedOp == "Divide")
        expected = Calculator::Operation::Divide;
    else if (expectedOp == "Power")
        expected = Calculator::Operation::Power;
    else if (expectedOp == "Modulo")
        expected = Calculator::Operation::Modulo;
    else if (expectedOp == "Unknown")
        expected = Calculator::Operation::Unknown;

    EXPECT_EQ(context->lastOperation, expected);
}

// ============================================
// Calculator State Management Steps
// ============================================

THEN("^the calculator name should be \"([^\"]*)\"$")
{
    REGEX_PARAM(std::string, expectedName);
    ScenarioScope<CalculatorContext> context;

    EXPECT_EQ(context->calculator->getName(), expectedName);
}

WHEN("^I change the calculator name to \"([^\"]*)\"$")
{
    REGEX_PARAM(std::string, newName);
    ScenarioScope<CalculatorContext> context;

    context->calculator->setName(newName);
}

THEN("^the calculator should be valid$")
{
    ScenarioScope<CalculatorContext> context;

    EXPECT_TRUE(context->calculator->isValid());
}

THEN("^the calculator should have (\\d+) history entries$")
{
    REGEX_PARAM(size_t, expectedCount);
    ScenarioScope<CalculatorContext> context;

    EXPECT_EQ(context->calculator->getHistorySize(), expectedCount);
}

THEN("^the calculator should have history$")
{
    ScenarioScope<CalculatorContext> context;

    EXPECT_TRUE(context->calculator->hasHistory());
}

THEN("^the calculator should not have history$")
{
    ScenarioScope<CalculatorContext> context;

    EXPECT_FALSE(context->calculator->hasHistory());
}

// ============================================
// History Management Steps
// ============================================

WHEN("^I perform the following calculations:$")
{
    TABLE_PARAM(table);
    ScenarioScope<CalculatorContext> context;

    auto rows = table.hashes();
    for (const auto &row : rows)
    {
        std::string operation = row.at("operation");
        double operand1 = std::stod(row.at("operand1"));
        double operand2 = std::stod(row.at("operand2"));

        Calculator::Operation op = Calculator::Operation::Unknown;
        if (operation == "Add")
            op = Calculator::Operation::Add;
        else if (operation == "Subtract")
            op = Calculator::Operation::Subtract;
        else if (operation == "Multiply")
            op = Calculator::Operation::Multiply;
        else if (operation == "Divide")
            op = Calculator::Operation::Divide;
        else if (operation == "Power")
            op = Calculator::Operation::Power;
        else if (operation == "Modulo")
            op = Calculator::Operation::Modulo;

        context->lastVariantResult = context->calculator->calculate(op, operand1, operand2);

        auto numericResult = context->getDoubleFromResult(context->lastVariantResult);
        if (numericResult.has_value())
        {
            context->lastNumericResult = numericResult.value();
        }
    }
}

THEN("^the last result should contain \"([^\"]*)\"$")
{
    REGEX_PARAM(std::string, expectedSubstring);
    ScenarioScope<CalculatorContext> context;

    auto lastResult = context->calculator->getLastResult();
    ASSERT_TRUE(lastResult.has_value());
    EXPECT_NE(lastResult.value().find(expectedSubstring), std::string::npos);
}

WHEN("^I clear the calculator history$")
{
    ScenarioScope<CalculatorContext> context;

    context->calculator->clearHistory();
}

THEN("^the history should contain an entry with \"([^\"]*)\"$")
{
    REGEX_PARAM(std::string, expectedEntry);
    ScenarioScope<CalculatorContext> context;

    const auto &history = context->calculator->getHistory();
    bool found = false;

    for (const auto &entry : history)
    {
        if (entry.find(expectedEntry) != std::string::npos)
        {
            found = true;
            break;
        }
    }

    EXPECT_TRUE(found) << "Expected to find '" << expectedEntry << "' in history";
}

// ============================================
// Complex Calculation Steps
// ============================================

WHEN("^I multiply the result by (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(double, multiplier);
    ScenarioScope<CalculatorContext> context;

    context->lastNumericResult = context->calculator->multiply(context->lastNumericResult, multiplier);
    context->lastVariantResult = context->calculator->calculate(
        Calculator::Operation::Multiply,
        context->lastNumericResult,
        multiplier);
}

WHEN("^I add the result and (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(double, addend);
    ScenarioScope<CalculatorContext> context;

    context->lastNumericResult = context->calculator->add(context->lastNumericResult, addend);
    context->lastVariantResult = context->calculator->calculate(
        Calculator::Operation::Add,
        context->lastNumericResult,
        addend);
}

WHEN("^I subtract (-?\\d+\\.?\\d*) from the result$")
{
    REGEX_PARAM(double, subtrahend);
    ScenarioScope<CalculatorContext> context;

    context->lastNumericResult = context->calculator->subtract(context->lastNumericResult, subtrahend);
    context->lastVariantResult = context->calculator->calculate(
        Calculator::Operation::Subtract,
        context->lastNumericResult,
        subtrahend);
}

THEN("^the final result should be (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(double, expected);
    ScenarioScope<CalculatorContext> context;

    EXPECT_DOUBLE_EQ(context->lastNumericResult, expected);
}

// ============================================
// Constexpr Function Steps
// ============================================

WHEN("^I calculate factorial of (\\d+)$")
{
    REGEX_PARAM(int, number);
    ScenarioScope<CalculatorContext> context;

    context->lastFactorialResult = Calculator::factorial(number);
}

THEN("^the factorial result should be (\\d+)$")
{
    REGEX_PARAM(long long, expected);
    ScenarioScope<CalculatorContext> context;

    EXPECT_EQ(context->lastFactorialResult, expected);
}

WHEN("^I calculate (-?\\d+\\.?\\d*) to the constant power of (\\d+)$")
{
    REGEX_PARAM(double, base);
    REGEX_PARAM(int, exponent);
    ScenarioScope<CalculatorContext> context;

    context->lastConstPowerResult = Calculator::constPower(base, exponent);
}

THEN("^the constant power result should be (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(double, expected);
    ScenarioScope<CalculatorContext> context;

    EXPECT_DOUBLE_EQ(context->lastConstPowerResult, expected);
}

// ============================================
// Copy Semantics Steps
// ============================================

WHEN("^I create a copy of the calculator$")
{
    ScenarioScope<CalculatorContext> context;

    context->copiedCalculator = std::make_unique<Calculator>(*context->calculator);
}

THEN("^the copied calculator should have the same name$")
{
    ScenarioScope<CalculatorContext> context;

    EXPECT_EQ(context->copiedCalculator->getName(), context->calculator->getName());
}

WHEN("^I change the copied calculator name to \"([^\"]*)\"$")
{
    REGEX_PARAM(std::string, newName);
    ScenarioScope<CalculatorContext> context;

    context->copiedCalculator->setName(newName);
}

WHEN("^I clear the copied calculator history$")
{
    ScenarioScope<CalculatorContext> context;

    context->copiedCalculator->clearHistory();
}

THEN("^the original calculator name should be \"([^\"]*)\"$")
{
    REGEX_PARAM(std::string, expectedName);
    ScenarioScope<CalculatorContext> context;

    EXPECT_EQ(context->calculator->getName(), expectedName);
}

THEN("^the original calculator should have (\\d+) history entries$")
{
    REGEX_PARAM(size_t, expectedCount);
    ScenarioScope<CalculatorContext> context;

    EXPECT_EQ(context->calculator->getHistorySize(), expectedCount);
}

THEN("^the copied calculator name should be \"([^\"]*)\"$")
{
    REGEX_PARAM(std::string, expectedName);
    ScenarioScope<CalculatorContext> context;

    EXPECT_EQ(context->copiedCalculator->getName(), expectedName);
}

THEN("^the copied calculator should have (\\d+) history entries$")
{
    REGEX_PARAM(size_t, expectedCount);
    ScenarioScope<CalculatorContext> context;

    EXPECT_EQ(context->copiedCalculator->getHistorySize(), expectedCount);
}

THEN("^the copied calculator should be valid$")
{
    ScenarioScope<CalculatorContext> context;

    EXPECT_TRUE(context->copiedCalculator->isValid());
}

// ============================================
// Execute Operation with Variant Steps
// ============================================

WHEN("^I execute operation \"([^\"]*)\" with operands (-?\\d+\\.?\\d*) and (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(std::string, opName);
    REGEX_PARAM(double, a);
    REGEX_PARAM(double, b);
    ScenarioScope<CalculatorContext> context;

    Calculator::Operation op = Calculator::Operation::Unknown;
    if (opName == "Add")
        op = Calculator::Operation::Add;
    else if (opName == "Subtract")
        op = Calculator::Operation::Subtract;
    else if (opName == "Multiply")
        op = Calculator::Operation::Multiply;
    else if (opName == "Divide")
        op = Calculator::Operation::Divide;
    else if (opName == "Power")
        op = Calculator::Operation::Power;
    else if (opName == "Modulo")
        op = Calculator::Operation::Modulo;

    context->lastVariantResult = context->calculator->executeOperation(op, a, b);
}

THEN("^the result should be a numeric value$")
{
    ScenarioScope<CalculatorContext> context;

    EXPECT_TRUE(std::holds_alternative<double>(context->lastVariantResult));
}

THEN("^the numeric result should be (-?\\d+\\.?\\d*)$")
{
    REGEX_PARAM(double, expected);
    ScenarioScope<CalculatorContext> context;

    auto result = context->getDoubleFromResult(context->lastVariantResult);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result.value(), expected);
}

THEN("^the result should be an error message$")
{
    ScenarioScope<CalculatorContext> context;

    EXPECT_TRUE(std::holds_alternative<std::string>(context->lastVariantResult));
}

THEN("^the error message should be \"([^\"]*)\"$")
{
    REGEX_PARAM(std::string, expectedMessage);
    ScenarioScope<CalculatorContext> context;

    auto errorMsg = context->getStringFromResult(context->lastVariantResult);
    ASSERT_TRUE(errorMsg.has_value());
    EXPECT_EQ(errorMsg.value(), expectedMessage);
}

// ============================================
// Stress Testing Steps
// ============================================

WHEN("^I perform (\\d+) addition operations$")
{
    REGEX_PARAM(int, count);
    ScenarioScope<CalculatorContext> context;

    for (int i = 0; i < count; ++i)
    {
        context->calculator->calculate(Calculator::Operation::Add, i, i + 1);
    }
}

// ============================================
// Additional Helper Steps
// ============================================

WHEN("^I add (-?\\d+\\.?\\d*e[+-]?\\d+) and (-?\\d+\\.?\\d*e[+-]?\\d+)$")
{
    REGEX_PARAM(double, a);
    REGEX_PARAM(double, b);
    ScenarioScope<CalculatorContext> context;

    context->lastNumericResult = context->calculator->add(a, b);
}

WHEN("^I multiply (-?\\d+\\.?\\d*e[+-]?\\d+) by (-?\\d+\\.?\\d*e[+-]?\\d+)$")
{
    REGEX_PARAM(double, a);
    REGEX_PARAM(double, b);
    ScenarioScope<CalculatorContext> context;

    context->lastNumericResult = context->calculator->multiply(a, b);
}

WHEN("^I divide (-?\\d+\\.?\\d*) by (-?\\d+\\.?\\d*e[+-]?\\d+)$")
{
    REGEX_PARAM(double, a);
    REGEX_PARAM(double, b);
    ScenarioScope<CalculatorContext> context;

    auto result = context->calculator->divide(a, b);
    if (result.has_value())
    {
        context->lastNumericResult = result.value();
        context->operationFailed = false;
    }
    else
    {
        context->operationFailed = true;
    }
}

// // THEN("^the result should be approximately (-?\\d+\\.?\\d*) within a tolerance of (\\d+\\.?\\d*)$") {
// //     REGEX_PARAM(double, expected);
// //     REGEX_PARAM(double, tolerance);
// //     ScenarioScope<CalculatorContext> context;

// //     EXPECT_NEAR(context->lastNumericResult, expected, tolerance);
// // }

