#include <gtest/gtest.h>

#include <cucumber-cpp/autodetect.hpp>

#include <optional>
#include <string>
#include <variant>

#include <com/github/doevelopper/premisses/demo/Calculator.hpp>

using com::github::doevelopper::premisses::demo::Calculator;
using cucumber::ScenarioScope;

namespace
{
  struct AcceptanceContext
  {
    Calculator calculator;
    double lastNumericResult{0.0};
    Calculator::ResultType lastVariantResult{0.0};
    Calculator::Operation lastOperation{Calculator::Operation::Unknown};
    bool operationFailed{false};
    std::string lastErrorMessage;

    static auto getDouble(const Calculator::ResultType &result) -> std::optional<double>
    {
      if (std::holds_alternative<double>(result))
      {
        return std::get<double>(result);
      }
      return std::nullopt;
    }

    static auto getString(const Calculator::ResultType &result) -> std::optional<std::string>
    {
      if (std::holds_alternative<std::string>(result))
      {
        return std::get<std::string>(result);
      }
      return std::nullopt;
    }
  };
} // namespace

// -------------------------------------------------------------------------------------------------
// Acceptance TP steps
// -------------------------------------------------------------------------------------------------

GIVEN("^a calculator$")
{
  ScenarioScope<AcceptanceContext> context;
  EXPECT_TRUE(context->calculator.isValid());
}

GIVEN("^I have a calculator named \"([^\"]*)\"$")
{
  REGEX_PARAM(std::string, name);
  ScenarioScope<AcceptanceContext> context;

  context->calculator.setName(name);
  EXPECT_TRUE(context->calculator.isValid());
}

WHEN("^I add (-?\\d+(?:\\.\\d+)?) and (-?\\d+(?:\\.\\d+)?)$")
{
  REGEX_PARAM(double, a);
  REGEX_PARAM(double, b);

  ScenarioScope<AcceptanceContext> context;
  context->lastNumericResult = context->calculator.add(a, b);
  context->lastVariantResult = context->calculator.calculate(Calculator::Operation::Add, a, b);
  context->operationFailed = false;
  context->lastErrorMessage.clear();
}

WHEN("^I divide (-?\\d+(?:\\.\\d+)?) by (-?\\d+(?:\\.\\d+)?)$")
{
  REGEX_PARAM(double, a);
  REGEX_PARAM(double, b);

  ScenarioScope<AcceptanceContext> context;

  const auto optionalResult = context->calculator.divide(a, b);
  context->operationFailed = !optionalResult.has_value();
  if (optionalResult.has_value())
  {
    context->lastNumericResult = optionalResult.value();
  }

  context->lastVariantResult = context->calculator.calculate(Calculator::Operation::Divide, a, b);
  const auto error = AcceptanceContext::getString(context->lastVariantResult);
  context->lastErrorMessage = error.value_or("");
}

WHEN("^I clear the calculator history$")
{
  ScenarioScope<AcceptanceContext> context;
  context->calculator.clearHistory();
}

WHEN("^I parse the operation \"([^\"]*)\"$")
{
  REGEX_PARAM(std::string, symbol);
  ScenarioScope<AcceptanceContext> context;

  context->lastOperation = Calculator::parseOperation(symbol);
}

THEN("^the result should be (-?\\d+(?:\\.\\d+)?)$")
{
  REGEX_PARAM(double, expected);
  ScenarioScope<AcceptanceContext> context;

  EXPECT_DOUBLE_EQ(expected, context->lastNumericResult);
}

THEN("^the operation should fail$")
{
  ScenarioScope<AcceptanceContext> context;

  const bool variantIsError = std::holds_alternative<std::string>(context->lastVariantResult);
  EXPECT_TRUE(context->operationFailed || variantIsError);
}

THEN("^the error message should contain \"([^\"]*)\"$")
{
  REGEX_PARAM(std::string, expected);
  ScenarioScope<AcceptanceContext> context;

  EXPECT_NE(context->lastErrorMessage.find(expected), std::string::npos);
}

THEN("^the calculator should have (\\d+) history entries$")
{
  REGEX_PARAM(std::size_t, expected);
  ScenarioScope<AcceptanceContext> context;

  EXPECT_EQ(expected, context->calculator.getHistorySize());
}

THEN("^the history should contain an entry with \"([^\"]*)\"$")
{
  REGEX_PARAM(std::string, expectedSubstring);
  ScenarioScope<AcceptanceContext> context;

  bool found = false;
  for (const auto &entry : context->calculator.getHistory())
  {
    if (entry.find(expectedSubstring) != std::string::npos)
    {
      found = true;
      break;
    }
  }
  EXPECT_TRUE(found) << "Expected to find '" << expectedSubstring << "' in history";
}

THEN("^the operation type should be \"([^\"]*)\"$")
{
  REGEX_PARAM(std::string, expectedOp);
  ScenarioScope<AcceptanceContext> context;

  Calculator::Operation expected = Calculator::Operation::Unknown;
  if (expectedOp == "Add")
  {
    expected = Calculator::Operation::Add;
  }
  else if (expectedOp == "Subtract")
  {
    expected = Calculator::Operation::Subtract;
  }
  else if (expectedOp == "Multiply")
  {
    expected = Calculator::Operation::Multiply;
  }
  else if (expectedOp == "Divide")
  {
    expected = Calculator::Operation::Divide;
  }
  else if (expectedOp == "Power")
  {
    expected = Calculator::Operation::Power;
  }
  else if (expectedOp == "Modulo")
  {
    expected = Calculator::Operation::Modulo;
  }

  EXPECT_EQ(context->lastOperation, expected);
}
