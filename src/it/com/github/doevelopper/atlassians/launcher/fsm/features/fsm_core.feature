Feature: Application Lifecycle State Machine (Core Functions)
  As a system administrator
  I want the application to manage its core lifecycle
  So that startup and shutdown are reliable

  Background:
    Given I have an FSM with default configuration

  # ============================================
  # Basic Startup
  # ============================================

  Scenario: FSM can be created and started
    When I create the FSM
    And I start the FSM
    Then the FSM should be in the initial state

  Scenario: Successful pre-validation
    Given I have a started FSM
    When I process "PreValidationPassed" event
    Then the pre-validation should succeed

  Scenario: Configuration loading success
    Given I have a started FSM
    When I complete pre-validation
    And I process "ConfigLoaded" event
    Then the configuration should be loaded

  Scenario: License validation success
    Given I have a started FSM
    When I complete pre-validation
    And I complete configuration loading
    And I process "LicenseValidated" event
    Then the license should be validated

  Scenario: Core initialization success
    Given I have a started FSM
    When I complete all initialization phases
    And I process "CoreInitialized" event
    Then the core should be initialized

  # ===========================================================================
  # Failure Handling
  # ============================================

  Scenario: Pre-validation failure with reason
    Given I have a started FSM
    When I process "PreValidation Failed" event with reason "Permission denied"
    Then the FSM should handle the failure

  Scenario: Configuration load failure
    Given I have a started FSM
    When I complete pre-validation
    And I process "ConfigLoadFailed" event with reason "File not found"
    Then the FSM should handle the configuration error

  # ============================================
  # Shutdown
  # ============================================

  Scenario: Graceful shutdown
    Given I have a started FSM
    When I initiate graceful shutdown with reason "User requested"
    Then the FSM should begin shutdown

  Scenario: Emergency shutdown
    Given I have a started FSM
    When I initiate emergency shutdown
    Then the FSM should stop immediately

  # ============================================
  # State and Context Queries
  # ============================================

  Scenario: Query current state name
    Given I have a started FSM
    Then I should be able to query the current state name

  Scenario: Check operational status
    Given I have a started FSM
Then I should be able to check if FSM is operational

  Scenario: Access FSM context
    Given I have a started FSM
    Then I should be able to access the FSM context

  # ============================================
  # Configuration
  # ============================================

  Scenario: Custom configuration with specific timeouts
    Given I configure the FSM with:
      | setting            | value |
      | configLoadTimeout  | 10000 |
      | shutdownTimeout    | 30000 |
      |maxCoreRecoveryAttempts | 5     |
    When I create the FSM
    Then the FSM should use the custom configuration

  Scenario: Validate valid configuration
    When I validate a configuration with valid settings
    Then the validation should pass with no errors

  Scenario: Validate invalid configuration
    When I validate a configuration with negative recovery attempts
    Then the validation should fail with error message

  # ============================================
  # Logging Control
  # ============================================

  Scenario: Enable verbose logging
    Given I have a started FSM
    When I enable verbose logging
    Then verbose logging should be active

  Scenario: Disable verbose logging
    Given I have a started FSM
    And verbose logging is enabled
    When I disable verbose logging
    Then verbose logging should be inactive

  # ============================================
  # FSM Lifecycle
  # ============================================

  Scenario: Complete FSM lifecycle from creation to shutdown
    Given I create an FSM with default settings
    When I start the FSM
    And I process all startup events successfully
    And I initiate graceful shutdown
    Then the FSM lifecycle should complete successfully
