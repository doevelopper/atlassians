Feature: Acceptance smoke

  Smoke scenarios are the smallest set of acceptance checks proving the system
  is usable end-to-end-ish and the most important behaviour is intact.

  Background:
    Given I have a calculator named "AcceptanceCalculator"

  @smoke
  Scenario: Calculator adds two numbers
    When I add 1 and 2
    Then the result should be 3
    And the calculator should have 1 history entries

  @smoke
  Scenario Outline: Operation parsing is available for integration layers
    When I parse the operation "<symbol>"
    Then the operation type should be "<operation>"

    Examples:
      | symbol | operation |
      | +      | Add       |
      | /      | Divide    |
