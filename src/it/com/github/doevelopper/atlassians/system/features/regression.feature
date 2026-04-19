Feature: Acceptance regression

  Regression scenarios cover high-value behaviours and failure modes that must
  not change without intention.

  Background:
    Given I have a calculator named "AcceptanceCalculator"

  @regression
  Scenario: Division by zero is handled
    When I divide 10 by 0
    Then the operation should fail
    And the error message should contain "Division by zero"

  @regression
  Scenario: A calculation is recorded in history
    When I add 15 and 25
    Then the history should contain an entry with "15 + 25 = 40"

  @regression
  Scenario: History can be cleared
    When I add 5 and 3
    And I clear the calculator history
    Then the calculator should have 0 history entries
