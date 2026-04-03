Feature: Application Lifecycle State Machine
  As a system administrator
  I want the application to manage its lifecycle through well-defined states
  So that startup, operation, and shutdown are reliable and observable

  Background:
    Given I have configured the FSM with default settings
    And logging is properly initialized

  # ============================================
  # Core Initialization Phase
  # ============================================

  Scenario: Successful application startup
    When I start the application FSM
    Then the FSM should enter "PreValidation" state
    When I process "PreValidationPassed" event
    Then the FSM should transition to "ConfigLoading" state
    When I process "ConfigLoaded" event
    Then the FSM should transition to "ConfigActivation" state
    When I process "ConfigActivated" event
    Then the FSM should transition to "LicenseValidation" state
    When I process "LicenseValidated" event
    Then the FSM should transition to "CoreServicesBootstrap" state
    When I process "CoreInitialized" event
    Then the FSM should transition to "PluginDiscovery" state
    When I process "PluginsDiscovered" event
    Then the FSM should transition to "Active" state
    And the application should be operational

  Scenario: Pre-validation failure triggers recovery
    Given the application FSM is started
    When I process "PreValidationFailed" event
    Then the FSM should transition to "CoreRecovering" state
    And the core recovery attempt count should be 1

  Scenario: Configuration load failure triggers recovery
    Given the application FSM is in "ConfigLoading" state
    When I process "ConfigLoadFailed" event
    Then the FSM should transition to "CoreRecovering" state
    And the phase data should contain error information

  Scenario: Successful recovery from core initialization failure
    Given the application FSM is in "CoreRecovering" state
    When I process "CoreRecoverySucceeded" event
    Then the FSM should transition to "PreValidation" state
    And I should be able to proceed with startup

  Scenario: Core recovery exhaustion leads to failure state
    Given the application FSM has exceeded maximum recovery attempts
    When I process "CoreRecoveryExhausted" event
    Then the FSM should transition to "CoreFailed" state
    And the application should not be operational

  # ============================================
  # License Validation
  # ============================================

  Scenario: License validation success
    Given the application FSM is in "LicenseValidation" state
    When I process "LicenseValidated" event
    Then the FSM should transition to "CoreServicesBootstrap" state
    And the license validation phase should be marked complete

  Scenario: License check failure triggers recovery
    Given the application FSM is in "LicenseValidation" state
    When I process "LicenseCheckFailed" event
    Then the FSM should transition to "CoreRecovering" state
    And the error count should be incremented

  # ============================================
  # Plugin System Initialization
  # ============================================

  Scenario: Plugin discovery success
    Given the application FSM is in "PluginDiscovery" state
    When I process "PluginsDiscovered" event
    Then the FSM should transition to "Active" state
    And the plugin system should be initialized

  # ============================================
  # Operational Runtime
  # ============================================

  Scenario: Health degradation during operation
    Given the application FSM is in "Active" state
    When I process "HealthDegraded" event
    Then the FSM should transition to "DegradedMode" state
    And the degraded mode flag should be set

  Scenario: Health restoration from degraded mode
    Given the application FSM is in "DegradedMode" state
    When I process "HealthRestored" event
    Then the FSM should transition to "Active" state
    And the degraded mode flag should be cleared

  # ============================================
  # Shutdown Operations
  # ============================================

  Scenario: Graceful shutdown from active state
    Given the application FSM is in "Active" state
    When I initiate graceful shutdown
    Then the FSM should transition to "PreShutdown" state
    When I process "PreShutdownComplete" event
    Then the FSM should transition to "ShutdownComplete" state
    And the shutdown should be marked as graceful

  Scenario: Graceful shutdown from degraded mode
    Given the application FSM is in "DegradedMode" state
    When I initiate graceful shutdown
    Then the FSM should transition to "PreShutdown" state
    When I process "PreShutdownComplete" event
    Then the FSM should transition to "ShutdownComplete" state

  Scenario: Emergency shutdown
    Given the application FSM is in "Active" state
    When I initiate emergency shutdown
    Then the FSM should stop immediately
    And the shutdown should be marked as forced

  # ============================================
  # Configuration and Context Management
  # ============================================

  Scenario: Custom FSM configuration
    Given I configure the FSM with the following settings:
      | setting                    | value |
      | maxCoreRecoveryAttempts    | 5     |
      | configLoadTimeout          | 10000 |
      | shutdownTimeout            | 60000 |
      | enableStatePersistence     | true  |
    When I create the application FSM
    Then the FSM should use the provided configuration

  Scenario: FSM context tracks transitions
    Given the application FSM is started
    When I execute a successful startup sequence
    Then the FSM context should have recorded all transitions
    And the transition count should be greater than 5

  Scenario: Phase duration tracking
    Given the application FSM is started
    When I complete the "CoreInitializing" phase
    Then the phase duration should be recorded
    And the phase should be marked as completed

  # ============================================
  # Metrics and Diagnostics
  # ============================================

  Scenario: Metrics collection during operation
    Given the application FSM is operational
    When I query FSM metrics
    Then I should see transition counts
    And I should see phase durations
    And I should see error counts

  Scenario: State name retrieval
    Given the application FSM is in "Active" state
    When I query the current state name
    Then I should receive "Active"

  Scenario: State history tracking
    Given the application FSM has processed multiple events
    When I query the state history
    Then I should see a list of previous states

  Scenario: Verbose logging control
    Given the application FSM is created
    When I enable verbose logging
    Then detailed transition logs should be generated
    When I disable verbose logging
    Then only essential logs should be generated

  # ============================================
  # Error Handling and Recovery
  # ============================================

  Scenario: Multiple recovery attempts before failure
    Given the application FSM is configured with 3 max recovery attempts
    And the FSM is in "CoreRecovering" state
    When I process "CoreRecoverySucceeded" event
    And I fail again and trigger recovery
    And I process "CoreRecoverySucceeded" event
    And I fail again and trigger recovery
    And I process "CoreRecoveryExhausted" event
    Then the FSM should transition to "CoreFailed" state

  Scenario: Error information preserved in phase data
    Given the application FSM is in "ConfigLoading" state
    When I process "ConfigLoadFailed" event with error message "File not found"
    Then the phase data should contain the error message "File not found"

  Scenario: Transition failure handling
    Given the application FSM is in "Active" state
    When I process an invalid event "InvalidEvent"
    Then the FSM should remain in "Active" state
    And a warning should be logged

  # ============================================
  # Integration Scenarios
  # ============================================

  Scenario: Complete application lifecycle
    Given I have a properly configured application FSM
    When I start the application FSM
    And I complete all initialization phases successfully
    Then the application should be operational
    When I process some operational events
    And I initiate graceful shutdown
    And I complete the shutdown sequence
    Then the FSM should reach "ShutdownComplete" state
    And all resources should be cleaned up

  Scenario: Startup failure and recovery workflow
    Given I have a properly configured application FSM
    When I start the application FSM
    And I encounter a failure during "CoreServicesBootstrap"
    Then the FSM should transition to recovery
    When the recovery succeeds
    Then I should be able to complete startup successfully
    And reach the "Active" state

  # ============================================
  # Configuration Validation
  # ============================================

  Scenario Outline: Configuration validation
    When I validate configuration with <setting> set to <value>
    Then the validation should <result>

    Examples:
      | setting                 | value  | result |
      | maxCoreRecoveryAttempts | 3      | pass   |
      | maxCoreRecoveryAttempts | -1     | fail   |
      | configLoadTimeout       | 5000   | pass   |
      | configLoadTimeout       | 0      | fail   |
      | shutdownTimeout         | 30000  | pass   |
      | shutdownTimeout         | -1000  | fail   |

  # ============================================
  # State Persistence (if enabled)
  # ============================================

  Scenario: FSM state persistence
    Given the FSM is configured with state persistence enabled
    And the application FSM is in "Active" state
    When I save the FSM state
    Then the state should be persisted to storage
    When I restore the FSM state
    Then the FSM should be in "Active" state

  # ============================================
  # Concurrency and Thread Safety
  # ============================================

  Scenario: Concurrent event processing
    Given the application FSM is in "Active" state
    When I process multiple events concurrently
    Then all events should be processed safely
    And the FSM should remain in a consistent state

  Scenario: Context updates are thread-safe
    Given the application FSM is operational
    When I update context data from multiple threads
    Then all updates should be correctly recorded
    And no data corruption should occur
