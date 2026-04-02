#include <com/github/doevelopper/atlassians/demo/CalculatorTest.hpp>

#include <cmath>

using namespace com::github::doevelopper::atlassians::demo;
using namespace com::github::doevelopper::atlassians::demo::test;

// log4cxx::LoggerPtr CalculatorTest::logger =
//     log4cxx::Logger::getLogger(std::string("com.github.doevelopper.atlassians.demo.test.CalculatorTest"));

CalculatorTest::CalculatorTest()
    : m_targetUnderTest(nullptr)
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

CalculatorTest::~CalculatorTest()
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void CalculatorTest::SetUp()
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  // Create a fresh Calculator instance before each test
  m_targetUnderTest = new Calculator("TestCalculator");
  ASSERT_NE(m_targetUnderTest, nullptr) << "Failed to create Calculator instance";
  ASSERT_TRUE(m_targetUnderTest->isValid()) << "Calculator should be valid after construction";
}

void CalculatorTest::TearDown()
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  // Clean up after each test
  if (this->m_targetUnderTest)
    delete m_targetUnderTest;
  this->m_targetUnderTest = nullptr;
}

// Helper function implementations
auto CalculatorTest::getDoubleResult(const Calculator::ResultType &result) -> std::optional<double>
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  if (std::holds_alternative<double>(result))
  {
    return std::get<double>(result);
  }
  return std::nullopt;
}

auto CalculatorTest::getStringResult(const Calculator::ResultType &result) -> std::optional<std::string>
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

  if (std::holds_alternative<std::string>(result))
  {
    return std::get<std::string>(result);
  }
  return std::nullopt;
}

auto CalculatorTest::isErrorResult(const Calculator::ResultType &result) -> bool
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  return std::holds_alternative<std::string>(result);
}

// ============================================
// Constructor and Basic State Tests
// ============================================

TEST_F(CalculatorTest, ConstructorWithName_SetsNameCorrectly)
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  EXPECT_EQ(m_targetUnderTest->getName(), "TestCalculator");
}

TEST_F(CalculatorTest, ConstructorDefault_CreatesValidCalculator)
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  Calculator defaultCalc;
  EXPECT_TRUE(defaultCalc.isValid());
  EXPECT_EQ(defaultCalc.getName(), "DefaultCalculator");
}

TEST_F(CalculatorTest, IsValid_ReturnsTrue_ForNewlyConstructedCalculator)
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  EXPECT_TRUE(m_targetUnderTest->isValid());
}

TEST_F(CalculatorTest, SetName_ChangesCalculatorName)
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  m_targetUnderTest->setName("NewName");
  EXPECT_EQ(m_targetUnderTest->getName(), "NewName");
}

// ============================================
// Basic Arithmetic Operations Tests
// ============================================

TEST_F(CalculatorTest, Add_PositiveNumbers_ReturnsCorrectSum)
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  double result = m_targetUnderTest->add(5.5, 3.2);
  EXPECT_DOUBLE_EQ(result, 8.7);
}

TEST_F(CalculatorTest, Add_NegativeNumbers_ReturnsCorrectSum)
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  double result = m_targetUnderTest->add(-5.0, -3.0);
  EXPECT_DOUBLE_EQ(result, -8.0);
}

TEST_F(CalculatorTest, Add_MixedSignNumbers_ReturnsCorrectSum)
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  double result = m_targetUnderTest->add(10.0, -3.0);
  EXPECT_DOUBLE_EQ(result, 7.0);
}

TEST_F(CalculatorTest, Subtract_PositiveNumbers_ReturnsCorrectDifference)
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  double result = m_targetUnderTest->subtract(10.0, 4.0);
  EXPECT_DOUBLE_EQ(result, 6.0);
}

TEST_F(CalculatorTest, Subtract_NegativeResult_ReturnsCorrectDifference)
{
  // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
  double result = m_targetUnderTest->subtract(3.0, 8.0);
  EXPECT_DOUBLE_EQ(result, -5.0);
}

TEST_F(CalculatorTest, Multiply_PositiveNumbers_ReturnsCorrectProduct)
{
  double result = m_targetUnderTest->multiply(4.0, 5.0);
  EXPECT_DOUBLE_EQ(result, 20.0);
}

TEST_F(CalculatorTest, Multiply_ByZero_ReturnsZero)
{
  double result = m_targetUnderTest->multiply(100.0, 0.0);
  EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST_F(CalculatorTest, Multiply_NegativeNumbers_ReturnsPositiveProduct)
{
  double result = m_targetUnderTest->multiply(-3.0, -4.0);
  EXPECT_DOUBLE_EQ(result, 12.0);
}

TEST_F(CalculatorTest, Divide_ValidDivision_ReturnsCorrectQuotient)
{
  auto result = m_targetUnderTest->divide(10.0, 2.0);
  ASSERT_TRUE(result.has_value()) << "Division should return a value";
  EXPECT_DOUBLE_EQ(result.value(), 5.0);
}

TEST_F(CalculatorTest, Divide_ByZero_ReturnsNullopt)
{
  auto result = m_targetUnderTest->divide(10.0, 0.0);
  EXPECT_FALSE(result.has_value()) << "Division by zero should return nullopt";
}

TEST_F(CalculatorTest, Divide_ByVerySmallNumber_ReturnsNullopt)
{
  auto result = m_targetUnderTest->divide(10.0, 1e-11);
  EXPECT_FALSE(result.has_value()) << "Division by very small number should return nullopt";
}

TEST_F(CalculatorTest, Divide_NegativeNumbers_ReturnsCorrectQuotient)
{
  auto result = m_targetUnderTest->divide(-12.0, 3.0);
  ASSERT_TRUE(result.has_value());
  EXPECT_DOUBLE_EQ(result.value(), -4.0);
}

TEST_F(CalculatorTest, Power_PositiveExponent_ReturnsCorrectResult)
{
  double result = m_targetUnderTest->power(2.0, 3.0);
  EXPECT_DOUBLE_EQ(result, 8.0);
}

TEST_F(CalculatorTest, Power_ZeroExponent_ReturnsOne)
{
  double result = m_targetUnderTest->power(5.0, 0.0);
  EXPECT_DOUBLE_EQ(result, 1.0);
}

TEST_F(CalculatorTest, Power_NegativeExponent_ReturnsCorrectResult)
{
  double result = m_targetUnderTest->power(2.0, -2.0);
  EXPECT_DOUBLE_EQ(result, 0.25);
}

TEST_F(CalculatorTest, Modulo_ValidOperation_ReturnsCorrectRemainder)
{
  auto result = m_targetUnderTest->modulo(17, 5);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 2);
}

TEST_F(CalculatorTest, Modulo_ByZero_ReturnsNullopt)
{
  auto result = m_targetUnderTest->modulo(10, 0);
  EXPECT_FALSE(result.has_value()) << "Modulo by zero should return nullopt";
}

TEST_F(CalculatorTest, Modulo_NegativeNumbers_ReturnsCorrectRemainder)
{
  auto result = m_targetUnderTest->modulo(-17, 5);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), -2);
}

// ============================================
// Execute Operation Tests (with [[fallthrough]])
// ============================================

TEST_F(CalculatorTest, ExecuteOperation_Add_ReturnsCorrectResult)
{
  auto result = m_targetUnderTest->executeOperation(Calculator::Operation::Add, 7.0, 3.0);
  auto doubleResult = getDoubleResult(result);
  ASSERT_TRUE(doubleResult.has_value());
  EXPECT_DOUBLE_EQ(doubleResult.value(), 10.0);
}

TEST_F(CalculatorTest, ExecuteOperation_Subtract_ReturnsCorrectResult)
{
  auto result = m_targetUnderTest->executeOperation(Calculator::Operation::Subtract, 7.0, 3.0);
  auto doubleResult = getDoubleResult(result);
  ASSERT_TRUE(doubleResult.has_value());
  EXPECT_DOUBLE_EQ(doubleResult.value(), 4.0);
}

TEST_F(CalculatorTest, ExecuteOperation_Multiply_ReturnsCorrectResult)
{
  auto result = m_targetUnderTest->executeOperation(Calculator::Operation::Multiply, 7.0, 3.0);
  auto doubleResult = getDoubleResult(result);
  ASSERT_TRUE(doubleResult.has_value());
  EXPECT_DOUBLE_EQ(doubleResult.value(), 21.0);
}

TEST_F(CalculatorTest, ExecuteOperation_Divide_ReturnsCorrectResult)
{
  auto result = m_targetUnderTest->executeOperation(Calculator::Operation::Divide, 15.0, 3.0);
  auto doubleResult = getDoubleResult(result);
  ASSERT_TRUE(doubleResult.has_value());
  EXPECT_DOUBLE_EQ(doubleResult.value(), 5.0);
}

TEST_F(CalculatorTest, ExecuteOperation_DivideByZero_ReturnsErrorString)
{
  auto result = m_targetUnderTest->executeOperation(Calculator::Operation::Divide, 10.0, 0.0);
  EXPECT_TRUE(isErrorResult(result));
  auto errorMsg = getStringResult(result);
  ASSERT_TRUE(errorMsg.has_value());
  EXPECT_EQ(errorMsg.value(), "Error: Division by zero");
}

TEST_F(CalculatorTest, ExecuteOperation_Power_ReturnsCorrectResult)
{
  auto result = m_targetUnderTest->executeOperation(Calculator::Operation::Power, 2.0, 10.0);
  auto doubleResult = getDoubleResult(result);
  ASSERT_TRUE(doubleResult.has_value());
  EXPECT_DOUBLE_EQ(doubleResult.value(), 1024.0);
}

TEST_F(CalculatorTest, ExecuteOperation_Modulo_ReturnsCorrectResult)
{
  auto result = m_targetUnderTest->executeOperation(Calculator::Operation::Modulo, 10.0, 3.0);
  auto doubleResult = getDoubleResult(result);
  ASSERT_TRUE(doubleResult.has_value());
  EXPECT_DOUBLE_EQ(doubleResult.value(), 1.0);
}

TEST_F(CalculatorTest, ExecuteOperation_ModuloByZero_ReturnsErrorString)
{
  auto result = m_targetUnderTest->executeOperation(Calculator::Operation::Modulo, 10.0, 0.0);
  EXPECT_TRUE(isErrorResult(result));
  auto errorMsg = getStringResult(result);
  ASSERT_TRUE(errorMsg.has_value());
  EXPECT_EQ(errorMsg.value(), "Error: Modulo by zero");
}

TEST_F(CalculatorTest, ExecuteOperation_Unknown_ReturnsErrorString)
{
  auto result = m_targetUnderTest->executeOperation(Calculator::Operation::Unknown, 10.0, 5.0);
  EXPECT_TRUE(isErrorResult(result));
  auto errorMsg = getStringResult(result);
  ASSERT_TRUE(errorMsg.has_value());
  EXPECT_EQ(errorMsg.value(), "Error: Unknown operation");
}

// ============================================
// Parse Operation Tests
// ============================================

TEST_F(CalculatorTest, ParseOperation_PlusSymbol_ReturnsAdd)
{
  auto op = Calculator::parseOperation("+");
  EXPECT_EQ(op, Calculator::Operation::Add);
}

TEST_F(CalculatorTest, ParseOperation_AddKeyword_ReturnsAdd)
{
  auto op = Calculator::parseOperation("add");
  EXPECT_EQ(op, Calculator::Operation::Add);
}

TEST_F(CalculatorTest, ParseOperation_MinusSymbol_ReturnsSubtract)
{
  auto op = Calculator::parseOperation("-");
  EXPECT_EQ(op, Calculator::Operation::Subtract);
}

TEST_F(CalculatorTest, ParseOperation_AsteriskSymbol_ReturnsMultiply)
{
  auto op = Calculator::parseOperation("*");
  EXPECT_EQ(op, Calculator::Operation::Multiply);
}

TEST_F(CalculatorTest, ParseOperation_SlashSymbol_ReturnsDivide)
{
  auto op = Calculator::parseOperation("/");
  EXPECT_EQ(op, Calculator::Operation::Divide);
}

TEST_F(CalculatorTest, ParseOperation_CaretSymbol_ReturnsPower)
{
  auto op = Calculator::parseOperation("^");
  EXPECT_EQ(op, Calculator::Operation::Power);
}

TEST_F(CalculatorTest, ParseOperation_PercentSymbol_ReturnsModulo)
{
  auto op = Calculator::parseOperation("%");
  EXPECT_EQ(op, Calculator::Operation::Modulo);
}

TEST_F(CalculatorTest, ParseOperation_InvalidSymbol_ReturnsUnknown)
{
  auto op = Calculator::parseOperation("@");
  EXPECT_EQ(op, Calculator::Operation::Unknown);
}

TEST_F(CalculatorTest, ParseOperation_EmptyString_ReturnsUnknown)
{
  auto op = Calculator::parseOperation("");
  EXPECT_EQ(op, Calculator::Operation::Unknown);
}

// ============================================
// Calculate Method Tests (with History)
// ============================================

TEST_F(CalculatorTest, Calculate_Add_StoresInHistory)
{
  EXPECT_EQ(m_targetUnderTest->getHistorySize(), 0);

  auto result = m_targetUnderTest->calculate(Calculator::Operation::Add, 5.0, 3.0);

  EXPECT_EQ(m_targetUnderTest->getHistorySize(), 1);
  ASSERT_TRUE(getDoubleResult(result).has_value());
  EXPECT_DOUBLE_EQ(getDoubleResult(result).value(), 8.0);
}

TEST_F(CalculatorTest, Calculate_MultipleOperations_AllStoredInHistory)
{
  m_targetUnderTest->calculate(Calculator::Operation::Add, 5.0, 3.0);
  m_targetUnderTest->calculate(Calculator::Operation::Multiply, 4.0, 2.0);
  m_targetUnderTest->calculate(Calculator::Operation::Subtract, 10.0, 7.0);

  EXPECT_EQ(m_targetUnderTest->getHistorySize(), 3);
}

TEST_F(CalculatorTest, Calculate_DivisionByZero_StoresErrorInHistory)
{
  auto result = m_targetUnderTest->calculate(Calculator::Operation::Divide, 10.0, 0.0);

  EXPECT_EQ(m_targetUnderTest->getHistorySize(), 1);
  EXPECT_TRUE(isErrorResult(result));
}

// ============================================
// History Management Tests
// ============================================

TEST_F(CalculatorTest, GetHistory_InitiallyEmpty)
{
  EXPECT_EQ(m_targetUnderTest->getHistorySize(), 0);
  EXPECT_TRUE(m_targetUnderTest->getHistory().empty());
}

TEST_F(CalculatorTest, HasHistory_InitiallyFalse)
{
  EXPECT_FALSE(m_targetUnderTest->hasHistory());
}

TEST_F(CalculatorTest, HasHistory_AfterCalculation_ReturnsTrue)
{
  m_targetUnderTest->calculate(Calculator::Operation::Add, 1.0, 2.0);
  EXPECT_TRUE(m_targetUnderTest->hasHistory());
}

TEST_F(CalculatorTest, ClearHistory_RemovesAllEntries)
{
  m_targetUnderTest->calculate(Calculator::Operation::Add, 1.0, 2.0);
  m_targetUnderTest->calculate(Calculator::Operation::Multiply, 3.0, 4.0);

  EXPECT_EQ(m_targetUnderTest->getHistorySize(), 2);

  m_targetUnderTest->clearHistory();

  EXPECT_EQ(m_targetUnderTest->getHistorySize(), 0);
  EXPECT_FALSE(m_targetUnderTest->hasHistory());
}

TEST_F(CalculatorTest, GetLastResult_NoHistory_ReturnsNullopt)
{
  auto lastResult = m_targetUnderTest->getLastResult();
  EXPECT_FALSE(lastResult.has_value());
}

TEST_F(CalculatorTest, GetLastResult_WithHistory_ReturnsLastEntry)
{
  m_targetUnderTest->calculate(Calculator::Operation::Add, 5.0, 3.0);
  m_targetUnderTest->calculate(Calculator::Operation::Multiply, 7.0, 2.0);

  auto lastResult = m_targetUnderTest->getLastResult();
  ASSERT_TRUE(lastResult.has_value());
  EXPECT_NE(lastResult.value().find("7 * 2"), std::string::npos);
}

TEST_F(CalculatorTest, GetHistory_ContainsFormattedOperations)
{
  m_targetUnderTest->calculate(Calculator::Operation::Add, 10.0, 5.0);

  const auto &history = m_targetUnderTest->getHistory();
  ASSERT_EQ(history.size(), 1);
  EXPECT_NE(history[0].find("10"), std::string::npos);
  EXPECT_NE(history[0].find("+"), std::string::npos);
  EXPECT_NE(history[0].find("5"), std::string::npos);
  EXPECT_NE(history[0].find("15"), std::string::npos);
}

// ============================================
// Constexpr Functions Tests
// ============================================

TEST_F(CalculatorTest, Factorial_Zero_ReturnsOne)
{
  constexpr auto result = Calculator::factorial(0);
  EXPECT_EQ(result, 1);
}

TEST_F(CalculatorTest, Factorial_One_ReturnsOne)
{
  constexpr auto result = Calculator::factorial(1);
  EXPECT_EQ(result, 1);
}

TEST_F(CalculatorTest, Factorial_Five_ReturnsCorrectValue)
{
  constexpr auto result = Calculator::factorial(5);
  EXPECT_EQ(result, 120);
}

TEST_F(CalculatorTest, Factorial_Ten_ReturnsCorrectValue)
{
  constexpr auto result = Calculator::factorial(10);
  EXPECT_EQ(result, 3628800);
}

TEST_F(CalculatorTest, ConstPower_BaseTwo_ExponentZero_ReturnsOne)
{
  constexpr auto result = Calculator::constPower(2.0, 0);
  EXPECT_DOUBLE_EQ(result, 1.0);
}

TEST_F(CalculatorTest, ConstPower_BaseTwo_ExponentThree_ReturnsEight)
{
  constexpr auto result = Calculator::constPower(2.0, 3);
  EXPECT_DOUBLE_EQ(result, 8.0);
}

TEST_F(CalculatorTest, ConstPower_BaseThree_ExponentFour_ReturnsCorrectValue)
{
  constexpr auto result = Calculator::constPower(3.0, 4);
  EXPECT_DOUBLE_EQ(result, 81.0);
}

// ============================================
// Rule of Five Tests
// ============================================

TEST_F(CalculatorTest, CopyConstructor_CreatesIndependentCopy)
{
  m_targetUnderTest->calculate(Calculator::Operation::Add, 5.0, 3.0);
  m_targetUnderTest->setName("Original");

  Calculator copy(*m_targetUnderTest);

  EXPECT_EQ(copy.getName(), "Original");
  EXPECT_EQ(copy.getHistorySize(), 1);
  EXPECT_TRUE(copy.isValid());

  // Verify independence
  copy.setName("Copy");
  EXPECT_EQ(m_targetUnderTest->getName(), "Original");
  EXPECT_EQ(copy.getName(), "Copy");
}

TEST_F(CalculatorTest, CopyConstructor_CopiesHistory)
{
  m_targetUnderTest->calculate(Calculator::Operation::Add, 10.0, 20.0);
  m_targetUnderTest->calculate(Calculator::Operation::Multiply, 5.0, 6.0);

  Calculator copy(*m_targetUnderTest);

  EXPECT_EQ(copy.getHistorySize(), m_targetUnderTest->getHistorySize());

  // Verify history independence
  copy.clearHistory();
  EXPECT_EQ(copy.getHistorySize(), 0);
  EXPECT_EQ(m_targetUnderTest->getHistorySize(), 2);
}

TEST_F(CalculatorTest, CopyAssignment_CreatesIndependentCopy)
{
  m_targetUnderTest->calculate(Calculator::Operation::Add, 5.0, 3.0);
  m_targetUnderTest->setName("Source");

  Calculator dest("Destination");
  dest = *m_targetUnderTest;

  EXPECT_EQ(dest.getName(), "Source");
  EXPECT_EQ(dest.getHistorySize(), 1);
  EXPECT_TRUE(dest.isValid());
}

TEST_F(CalculatorTest, CopyAssignment_SelfAssignment_WorksCorrectly)
{
  m_targetUnderTest->calculate(Calculator::Operation::Add, 5.0, 3.0);

  *m_targetUnderTest = *m_targetUnderTest;

  EXPECT_TRUE(m_targetUnderTest->isValid());
  EXPECT_EQ(m_targetUnderTest->getHistorySize(), 1);
}

TEST_F(CalculatorTest, MoveConstructor_TransfersOwnership)
{
  m_targetUnderTest->calculate(Calculator::Operation::Add, 10.0, 5.0);
  m_targetUnderTest->setName("MoveSource");

  Calculator moved(std::move(*m_targetUnderTest));

  EXPECT_EQ(moved.getName(), "MoveSource");
  EXPECT_EQ(moved.getHistorySize(), 1);
  EXPECT_TRUE(moved.isValid());
}

TEST_F(CalculatorTest, MoveAssignment_TransfersOwnership)
{
  m_targetUnderTest->calculate(Calculator::Operation::Multiply, 7.0, 8.0);
  m_targetUnderTest->setName("MoveSource");

  Calculator dest("Destination");
  dest = std::move(*m_targetUnderTest);

  EXPECT_EQ(dest.getName(), "MoveSource");
  EXPECT_EQ(dest.getHistorySize(), 1);
  EXPECT_TRUE(dest.isValid());
}

TEST_F(CalculatorTest, MoveAssignment_SelfMove_WorksCorrectly)
{
  m_targetUnderTest->calculate(Calculator::Operation::Add, 5.0, 3.0);

  *m_targetUnderTest = std::move(*m_targetUnderTest);

  // After self-move, object should still be in a valid state
  EXPECT_TRUE(m_targetUnderTest->isValid());
}

// ============================================
// Edge Cases and Error Handling Tests
// ============================================

TEST_F(CalculatorTest, VeryLargeNumbers_HandledCorrectly)
{
  double result = m_targetUnderTest->add(1e308, 1e308);
  EXPECT_TRUE(std::isinf(result) || result > 1e308);
}

TEST_F(CalculatorTest, VerySmallNumbers_HandledCorrectly)
{
  double result = m_targetUnderTest->multiply(1e-150, 1e-150);
  EXPECT_GE(result, 0.0);
}

TEST_F(CalculatorTest, NegativeZero_HandledCorrectly)
{
  double result = m_targetUnderTest->add(-0.0, 0.0);
  EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST_F(CalculatorTest, MultipleOperations_MaintainsPrecision)
{
  double result = m_targetUnderTest->add(0.1, 0.2);
  EXPECT_NEAR(result, 0.3, 1e-10);
}

// ============================================
// Stress Tests
// ============================================

TEST_F(CalculatorTest, LargeHistory_HandlesCorrectly)
{
  for (int i = 0; i < 1000; ++i)
  {
    m_targetUnderTest->calculate(Calculator::Operation::Add, i, i + 1);
  }

  EXPECT_EQ(m_targetUnderTest->getHistorySize(), 1000);
  EXPECT_TRUE(m_targetUnderTest->hasHistory());
}

TEST_F(CalculatorTest, RepeatedClearHistory_WorksCorrectly)
{
  for (int i = 0; i < 10; ++i)
  {
    m_targetUnderTest->calculate(Calculator::Operation::Add, 1.0, 2.0);
    m_targetUnderTest->clearHistory();
  }

  EXPECT_EQ(m_targetUnderTest->getHistorySize(), 0);
}

// ============================================
// Inline Constants Tests
// ============================================

TEST_F(CalculatorTest, PIConstant_HasCorrectValue)
{
  EXPECT_NEAR(PI, 3.14159265358979323846, 1e-15);
}

TEST_F(CalculatorTest, EConstant_HasCorrectValue)
{
  EXPECT_NEAR(E, 2.71828182845904523536, 1e-15);
}
