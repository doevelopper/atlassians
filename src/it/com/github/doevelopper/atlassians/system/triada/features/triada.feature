Feature: Calculator Operations
  As a user of the Calculator class
  I want to perform mathematical operations
  So that I can calculate results accurately

  Background:
    Given I have a calculator named "BDDCalculator"

  # ============================================
  # Basic Arithmetic Operations
  # ============================================

  Scenario: Adding two positive numbers
    When I add 5 and 3
    Then the result should be 8

  Scenario: Adding negative numbers
    When I add -10 and -5
    Then the result should be -15

  Scenario: Adding mixed sign numbers
    When I add 10 and -3
    Then the result should be 7

  Scenario: Subtracting two numbers
    When I subtract 5 from 10
    Then the result should be 5

  Scenario: Subtracting to get negative result
    When I subtract 10 from 5
    Then the result should be -5

  Scenario: Multiplying two positive numbers
    When I multiply 6 by 7
    Then the result should be 42

  Scenario: Multiplying by zero
    When I multiply 100 by 0
    Then the result should be 0

  Scenario: Multiplying negative numbers
    When I multiply -4 by -5
    Then the result should be 20

  Scenario: Dividing two numbers
    When I divide 20 by 4
    Then the result should be 5

  Scenario: Division by zero
    When I divide 10 by 0
    Then the operation should fail
    And the error message should contain "Division by zero"

  Scenario: Calculating power
    When I calculate 2 to the power of 8
    Then the result should be 256

  Scenario: Power with zero exponent
    When I calculate 5 to the power of 0
    Then the result should be 1

  Scenario: Calculating modulo
    When I calculate 17 modulo 5
    Then the result should be 2

  Scenario: Modulo by zero
    When I calculate 10 modulo 0
    Then the operation should fail
    And the error message should contain "Modulo by zero"

  # ============================================
  # Operation Parsing
  # ============================================

  Scenario Outline: Parsing operation symbols
    When I parse the operation "<symbol>"
    Then the operation type should be "<operation>"

    Examples:
      | symbol | operation |
      | +      | Add       |
      | -      | Subtract  |
      | *      | Multiply  |
      | /      | Divide    |
      | ^      | Power     |
      | %      | Modulo    |
      | @      | Unknown   |

  Scenario Outline: Parsing operation keywords
    When I parse the operation "<keyword>"
    Then the operation type should be "<operation>"

    Examples:
      | keyword  | operation |
      | add      | Add       |
      | subtract | Subtract  |
      | multiply | Multiply  |
      | divide   | Divide    |
      | power    | Power     |
      | modulo   | Modulo   |

  # ============================================
  # Calculator State Management
  # ============================================

  Scenario: Calculator has a name
    Then the calculator name should be "BDDCalculator"

  Scenario: Changing calculator name
    When I change the calculator name to "NewCalculator"
    Then the calculator name should be "NewCalculator"

  Scenario: Calculator is initially valid
    Then the calculator should be valid

  Scenario: Calculator history is initially empty
    Then the calculator should have 0 history entries
    And the calculator should not have history

  # ============================================
  # History Management
  # ============================================

  Scenario: Operations are stored in history
    When I perform the following calculations:
      | operation | operand1 | operand2 |
      | Add       | 5        | 3        |
      | Multiply  | 4        | 2        |
      | Subtract  | 10       | 7        |
    Then the calculator should have 3 history entries
    And the calculator should have history

  Scenario: Retrieving last calculation result
    When I perform the following calculations:
      | operation | operand1 | operand2 |
      | Add       | 10       | 20       |
      | Multiply  | 5        | 6        |
    Then the last result should contain "5 * 6 = 30"

  Scenario: Clearing calculator history
    When I perform the following calculations:
      | operation | operand1 | operand2 |
      | Add       | 5        | 3        |
      | Multiply  | 4        | 2        |
    And I clear the calculator history
    Then the calculator should have 0 history entries
    And the calculator should not have history

  Scenario: History contains formatted operations
    When I add 15 and 25
    Then the history should contain an entry with "15 + 25 = 40"

  # ============================================
  # Complex Calculation Scenarios
  # ============================================

  Scenario: Performing a sequence of operations
    When I add 10 and 5
    And I multiply the result by 2
    Then the final result should be 30

  Scenario: Mixed operations with different data types
    When I calculate 17 modulo 5
    And I add the result and 10
    Then the final result should be 12

  # ============================================
  # Constexpr Functions
  # ============================================

  Scenario Outline: Calculating factorial
    When I calculate factorial of <number>
    Then the factorial result should be <result>

    Examples:
      | number | result  |
      | 0      | 1       |
      | 1      | 1       |
      | 5      | 120     |
      | 7      | 5040    |

  Scenario Outline: Calculating constant power
    When I calculate <base> to the constant power of <exponent>
    Then the constant power result should be <result>

    Examples:
      | base | exponent | result |
      | 2    | 0        | 1      |
      | 2    | 3        | 8      |
      | 3    | 4        | 81     |
      | 10   | 2        | 100    |

  # ============================================
  # Rule of Five - Copy Semantics
  # ============================================

  Scenario: Copying calculator preserves state
    Given I have a calculator named "Original"
    When I perform the following calculations:
      | operation | operand1 | operand2 |
      | Add       | 5        | 3        |
      | Multiply  | 4        | 2        |
    And I create a copy of the calculator
    Then the copied calculator should have the same name
    And the copied calculator should have 2 history entries
    And the copied calculator should be valid

  Scenario: Copied calculator is independent
    Given I have a calculator named "Original"
    When I add 10 and 5
    And I create a copy of the calculator
    And I change the copied calculator name to "Copy"
    And I clear the copied calculator history
    Then the original calculator name should be "Original"
    And the original calculator should have 1 history entries
    And the copied calculator name should be "Copy"
    And the copied calculator should have 0 history entries

  # ============================================
  # Execute Operation with Variants
  # ============================================

  Scenario: Execute operation returns numeric result
    When I execute operation "Add" with operands 7 and 3
    Then the result should be a numeric value
    And the numeric result should be 10

  Scenario: Execute operation returns error message
    When I execute operation "Divide" with operands 10 and 0
    Then the result should be an error message
    And the error message should be "Error: Division by zero"

  # ============================================
  # Edge Cases
  # ============================================

  Scenario: Adding very large numbers
    When I add 1e100 and 2e100
    Then the result should be greater than 1e100

  Scenario: Dividing by very small number
    When I divide 10 by 1e-11
    Then the operation should fail

  Scenario: Multiplying very small numbers
    When I multiply 1e-150 by 1e-150
    Then the result should be greater than or equal to 0

  Scenario: Power with negative exponent
    When I calculate 2 to the power of -3
    Then the result should be 0.125

  # ============================================
  # Stress Testing
  # ============================================

  Scenario: Performing many operations maintains history
    When I perform 100 addition operations
    Then the calculator should have 100 history entries
    And the calculator should be valid

  Scenario: Repeatedly clearing history works correctly
    When I add 5 and 3
    And I clear the calculator history
    And I add 10 and 20
    And I clear the calculator history
    Then the calculator should have 0 history entries

  # ============================================
  # Integration Scenarios
  # ============================================

  Scenario: Complete calculation workflow
    Given I have a calculator named "WorkflowCalculator"
    When I add 10 and 5
    And I multiply the result by 2
    And I subtract 5 from the result
    Then the final result should be 25
    And the calculator should have 3 history entries
    And the last result should contain "30 - 5 = 25"

  Scenario: Error handling in workflow
    When I add 10 and 5
    And I divide the result by 0
    Then the operation should fail
    And the calculator should have 2 history entries
    And the last result should contain "Error: Division by zero"