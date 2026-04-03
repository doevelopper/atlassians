#ifndef COM_GITHUB_DOEVELOPPER_ATLASSIANS_DEMO_CALCULATOR_HPP
#define COM_GITHUB_DOEVELOPPER_ATLASSIANS_DEMO_CALCULATOR_HPP

#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <variant>
#include <cstddef>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

/**
 * @brief Modern C++ Calculator class demonstrating C++11/14/17/20 features
 *
 * Features demonstrated:
 * - Rule of Five
 * - Trailing return types
 * - [[nodiscard]] attribute
 * - [[fallthrough]] attribute
 * - constexpr
 * - noexcept
 * - std::optional
 * - std::variant
 * - std::string_view
 * - Smart pointers
 * - Move semantics
 * - RAII
 */

namespace com::github::doevelopper::atlassians::demo
{
  class Calculator
  {
    LOG4CXX_DECLARE_STATIC_LOGGER
  public:
    // Type aliases for modern C++ style
    using ResultType = std::variant<double, std::string>;
    using HistoryContainer = std::vector<std::string>;

    // Enum class for operation types (strongly typed enum)
    enum class Operation : std::uint8_t
    {
      Add,
      Subtract,
      Multiply,
      Divide,
      Power,
      Modulo,
      Unknown
    };

    // ============================================
    // Rule of Five: Special Member Functions
    // ============================================

    /**
     * @brief Default constructor
     */
    explicit Calculator(std::string_view name = "DefaultCalculator") noexcept;

    /**
     * @brief Destructor
     */
    ~Calculator() noexcept;

    /**
     * @brief Copy constructor
     */
    Calculator(const Calculator &other);

    /**
     * @brief Copy assignment operator with trailing return type
     */
    auto operator=(const Calculator &other) -> Calculator &;

    /**
     * @brief Move constructor
     */
    Calculator(Calculator &&other) noexcept;

    /**
     * @brief Move assignment operator with trailing return type
     */
    auto operator=(Calculator &&other) noexcept -> Calculator &;

    // ============================================
    // Basic Operations
    // ============================================

    /**
     * @brief Add two numbers
     * @param a First operand
     * @param b Second operand
     * @return Sum of a and b
     * [[nodiscard]] ensures the result is not ignored
     */
    [[nodiscard]] auto add(double a, double b) const noexcept -> double;

    /**
     * @brief Subtract b from a
     */
    [[nodiscard]] auto subtract(double a, double b) const noexcept -> double;

    /**
     * @brief Multiply two numbers
     */
    [[nodiscard]] auto multiply(double a, double b) const noexcept -> double;

    /**
     * @brief Divide a by b
     * @return std::optional containing result, or std::nullopt if division by zero
     */
    [[nodiscard]] auto divide(double a, double b) const noexcept -> std::optional<double>;

    /**
     * @brief Calculate a raised to the power of b
     */
    [[nodiscard]] auto power(double base, double exponent) const noexcept -> double;

    /**
     * @brief Calculate modulo (remainder of integer division)
     */
    [[nodiscard]] auto modulo(int a, int b) const noexcept -> std::optional<int>;

    // ============================================
    // Advanced Features
    // ============================================

    /**
     * @brief Execute operation based on enum
     * Demonstrates [[fallthrough]] attribute
     */
    [[nodiscard]] auto executeOperation(Operation op, double a, double b) const noexcept -> ResultType;

    /**
     * @brief Parse operation from string
     * @param opStr String representation of operation ("+", "-", "*", "/", "^", "%")
     * @return Operation enum value
     */
    [[nodiscard]] static auto parseOperation(std::string_view opStr) noexcept -> Operation;

    /**
     * @brief Perform calculation and store in history
     * @param op Operation to perform
     * @param a First operand
     * @param b Second operand
     * @return Result as variant (double or error string)
     */
    [[nodiscard]] auto calculate(Operation op, double a, double b) -> ResultType;

    // ============================================
    // History Management
    // ============================================

    /**
     * @brief Get calculation history
     */
    [[nodiscard]] auto getHistory() const noexcept -> const HistoryContainer &;

    /**
     * @brief Clear calculation history
     */
    auto clearHistory() noexcept -> void;

    /**
     * @brief Get size of history
     */
    [[nodiscard]] auto getHistorySize() const noexcept -> std::size_t;

    /**
     * @brief Check if history is empty
     */
    [[nodiscard]] auto hasHistory() const noexcept -> bool;

    // ============================================
    // Utility Functions
    // ============================================

    /**
     * @brief Get calculator name
     */
    [[nodiscard]] auto getName() const noexcept -> std::string_view;

    /**
     * @brief Set calculator name
     */
    auto setName(std::string_view newName) -> void;

    /**
     * @brief Check if calculator is in valid state
     */
    [[nodiscard]] auto isValid() const noexcept -> bool;

    /**
     * @brief Get last result from history
     */
    [[nodiscard]] auto getLastResult() const noexcept -> std::optional<std::string>;

    // ============================================
    // Constexpr Functions
    // ============================================

    /**
     * @brief Compile-time factorial calculation
     */
    [[nodiscard]] static constexpr auto factorial(int n) noexcept -> long long
    {
      return (n <= 1) ? 1 : n * factorial(n - 1);
    }

    /**
     * @brief Compile-time power calculation
     */
    [[nodiscard]] static constexpr auto constPower(double base, int exp) noexcept -> double
    {
      return (exp == 0) ? 1.0 : base * constPower(base, exp - 1);
    }

    // ============================================
    // Deleted Functions (Modern C++ best practice)
    // ============================================

    // Prevent accidental conversions
    Calculator(int) = delete;
    Calculator(double) = delete;

  private:
    // ============================================
    // Private Implementation (PIMPL idiom)
    // ============================================

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Helper function to format operation for history
    [[nodiscard]] auto formatOperation(Operation op, double a, double b, const ResultType &result) const -> std::string;
  };

  // ============================================
  // Inline constexpr variables (C++17)
  // ============================================
  inline constexpr double PI = 3.14159265358979323846;
  inline constexpr double E = 2.71828182845904523536;
}

#endif // PREMISSES_CALCULATOR_HPP
