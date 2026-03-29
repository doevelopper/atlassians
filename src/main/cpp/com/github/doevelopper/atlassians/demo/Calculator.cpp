#include "com/github/doevelopper/atlassians/demo/Calculator.hpp"

using namespace com::github::doevelopper::atlassians::demo;

#include <cmath>
#include <sstream>
#include <algorithm>
#include <stdexcept>

// ============================================
// PIMPL Implementation Struct
// ============================================
log4cxx::LoggerPtr Calculator::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.demo.Calculator"));

struct Calculator::Impl
{
  std::string name;
  HistoryContainer history;
  bool valid;

  explicit Impl(std::string_view calcName) noexcept
      : name(calcName), valid(true)
  {
    history.reserve(100); // Pre-allocate for performance
  }

  // Copy constructor for Impl
  Impl(const Impl &other)
      : name(other.name), history(other.history), valid(other.valid)
  {
  }

  // Move constructor for Impl
  Impl(Impl &&other) noexcept
      : name(std::move(other.name)), history(std::move(other.history)), valid(other.valid)
  {
    other.valid = false;
  }
};

// ============================================
// Rule of Five Implementation
// ============================================

Calculator::Calculator(std::string_view name) noexcept
    : pImpl(std::make_unique<Impl>(name))
{
  LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

Calculator::~Calculator() noexcept
{
  LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

Calculator::Calculator(const Calculator &other)
    : pImpl(std::make_unique<Impl>(*other.pImpl))
{
  LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

auto Calculator::operator=(const Calculator &other) -> Calculator &
{
  LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
  if (this != &other)
  {
    pImpl = std::make_unique<Impl>(*other.pImpl);
  }
  return *this;
}

Calculator::Calculator(Calculator &&other) noexcept = default;

auto Calculator::operator=(Calculator &&other) noexcept -> Calculator & = default;

// ============================================
// Basic Operations Implementation
// ============================================

auto Calculator::add(double a, double b) const noexcept -> double
{
  return a + b;
}

auto Calculator::subtract(double a, double b) const noexcept -> double
{
  return a - b;
}

auto Calculator::multiply(double a, double b) const noexcept -> double
{
  return a * b;
}

auto Calculator::divide(double a, double b) const noexcept -> std::optional<double>
{
  // Check for division by zero
  if (std::abs(b) < 1e-10)
  {
    return std::nullopt;
  }
  return a / b;
}

auto Calculator::power(double base, double exponent) const noexcept -> double
{
  return std::pow(base, exponent);
}

auto Calculator::modulo(int a, int b) const noexcept -> std::optional<int>
{
  if (b == 0)
  {
    return std::nullopt;
  }
  return a % b;
}

// ============================================
// Advanced Features Implementation
// ============================================

auto Calculator::executeOperation(Operation op, double a, double b) const noexcept -> ResultType
{
  // Demonstrate [[fallthrough]] attribute
  switch (op)
  {
  case Operation::Add:
    return add(a, b);

  case Operation::Subtract:
    return subtract(a, b);

  case Operation::Multiply:
    return multiply(a, b);

  case Operation::Divide:
  {
    auto result = divide(a, b);
    if (result.has_value())
    {
      return result.value();
    }
    return std::string("Error: Division by zero");
  }

  case Operation::Power:
    return power(a, b);

  case Operation::Modulo:
  {
    auto result = modulo(static_cast<int>(a), static_cast<int>(b));
    if (result.has_value())
    {
      return static_cast<double>(result.value());
    }
    return std::string("Error: Modulo by zero");
  }

  case Operation::Unknown:
    [[fallthrough]]; // Intentional fallthrough to default

  default:
    return std::string("Error: Unknown operation");
  }
}

auto Calculator::parseOperation(std::string_view opStr) noexcept -> Operation
{
  // Use if-else chain to demonstrate std::string_view usage
  if (opStr == "+" || opStr == "add")
  {
    return Operation::Add;
  }
  else if (opStr == "-" || opStr == "subtract")
  {
    return Operation::Subtract;
  }
  else if (opStr == "*" || opStr == "multiply")
  {
    return Operation::Multiply;
  }
  else if (opStr == "/" || opStr == "divide")
  {
    return Operation::Divide;
  }
  else if (opStr == "^" || opStr == "power" || opStr == "**")
  {
    return Operation::Power;
  }
  else if (opStr == "%" || opStr == "mod" || opStr == "modulo")
  {
    return Operation::Modulo;
  }

  return Operation::Unknown;
}

auto Calculator::calculate(Operation op, double a, double b) -> ResultType
{
  if (!pImpl || !pImpl->valid)
  {
    return std::string("Error: Calculator in invalid state");
  }

  // Execute the operation
  ResultType result = executeOperation(op, a, b);

  // Store in history
  std::string historyEntry = formatOperation(op, a, b, result);
  pImpl->history.push_back(std::move(historyEntry));

  return result;
}

// ============================================
// History Management Implementation
// ============================================

auto Calculator::getHistory() const noexcept -> const HistoryContainer &
{
  static const HistoryContainer emptyHistory;
  return pImpl ? pImpl->history : emptyHistory;
}

auto Calculator::clearHistory() noexcept -> void
{
  if (pImpl)
  {
    pImpl->history.clear();
  }
}

auto Calculator::getHistorySize() const noexcept -> std::size_t
{
  return pImpl ? pImpl->history.size() : 0;
}

auto Calculator::hasHistory() const noexcept -> bool
{
  return pImpl && !pImpl->history.empty();
}

// ============================================
// Utility Functions Implementation
// ============================================

auto Calculator::getName() const noexcept -> std::string_view
{
  static const std::string invalidName = "Invalid";
  return pImpl ? std::string_view(pImpl->name) : std::string_view(invalidName);
}

auto Calculator::setName(std::string_view newName) -> void
{
  if (pImpl)
  {
    pImpl->name = newName;
  }
}

auto Calculator::isValid() const noexcept -> bool
{
  return pImpl && pImpl->valid;
}

auto Calculator::getLastResult() const noexcept -> std::optional<std::string>
{
  if (!pImpl || pImpl->history.empty())
  {
    return std::nullopt;
  }
  return pImpl->history.back();
}

// ============================================
// Private Helper Functions
// ============================================

auto Calculator::formatOperation(Operation op, double a, double b, const ResultType &result) const -> std::string
{
  std::ostringstream oss;

  // Format the operation symbol
  const char *opSymbol = "?";
  switch (op)
  {
  case Operation::Add:
    opSymbol = "+";
    break;
  case Operation::Subtract:
    opSymbol = "-";
    break;
  case Operation::Multiply:
    opSymbol = "*";
    break;
  case Operation::Divide:
    opSymbol = "/";
    break;
  case Operation::Power:
    opSymbol = "^";
    break;
  case Operation::Modulo:
    opSymbol = "%";
    break;
  case Operation::Unknown:
    opSymbol = "?";
    break;
  }

  // Format the result using std::visit (C++17 variant visitor)
  oss << a << " " << opSymbol << " " << b << " = ";

  std::visit([&oss](auto &&arg)
             {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, double>)
		{
            oss << arg;
        }
		else if constexpr (std::is_same_v<T, std::string>)
		{
            oss << arg;
        } }, result);

  return oss.str();
}
