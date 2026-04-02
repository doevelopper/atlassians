Feature: Logging Framework Initialization
  As a doevelopper
  I want to initialize the logging framework using different strategies
  So that I can configure logging appropriately for different environments

  Background:
    Given the logging framework is not initialized
    And no log files exist in the test directory

  @smoke @critical
  Scenario: Initialize with default strategy
    Given I have a DefaultInitializationStrategy
    When I initialize the logging framework with the default strategy
    Then the logging framework should be initialized successfully
    And the root logger should be available
    And the console appender should be configured
    And the default log level should be INFO

  @integration
  Scenario: Initialize with property file strategy
    Given I have a valid log4cxx properties file "test-log4cxx.properties"
    And the properties file contains configuration for console and file appenders
    When I create a PropertyFileBasedInitializationStrategy with the properties file path
    And I initialize the logging framework with the property file strategy
    Then the logging framework should be initialized successfully
    And the configuration should be loaded from the properties file
    And both console and file appenders should be active
    And log messages should be written to the configured file

  @integration
  Scenario: Initialize with XML configuration file
    Given I have a valid log4cxx XML file "test-log4cxx.xml"
    And the XML file defines multiple appenders and loggers
    When I create an XMLFileBasedInitializationStrategy with the XML file path
    And I initialize the logging framework with the XML strategy
    Then the logging framework should be initialized successfully
    And all appenders defined in XML should be configured
    And logger hierarchy should match the XML configuration
    And log level inheritance should work according to XML definition

  @integration
  Scenario: Initialize with environment-based strategy
    Given the environment variable "LOG4CXX_CONFIG_FILE" is set to "/config/app-log4cxx.properties"
    And the configuration file at that path exists and is valid
    When I create an EnvironmentBasedInitializationStrategy
    And I initialize the logging framework with the environment strategy
    Then the logging framework should be initialized successfully
    And the configuration should be loaded from the environment-specified file
    And the log output should match the environment configuration

  @integration
  Scenario: Environment strategy falls back to default when env var not set
    Given the environment variable "LOG4CXX_CONFIG_FILE" is not set
    When I create an EnvironmentBasedInitializationStrategy
    And I initialize the logging framework with the environment strategy
    Then the logging framework should initialize with default configuration
    And a warning should be logged about missing environment variable
    And the console appender should be configured as fallback

  @integration
  Scenario: Initialize with programmatic strategy
    Given I create a ProgrammaticInitializationStrategy
    And I configure it with a console appender
    And I configure it with a file appender to "logs/programmatic.log"
    And I set the root log level to DEBUG
    And I set the pattern to "%d{ISO8601} [%t] %-5p %c - %m%n"
    When I initialize the logging framework with the programmatic strategy
    Then the logging framework should be initialized successfully
    And the console appender should use the specified pattern
    And the file appender should write to "logs/programmatic.log"
    And the root logger should have DEBUG level

  @critical @idempotency
  Scenario: Double initialization should be prevented
    Given I have a DefaultInitializationStrategy
    And I initialize the logging framework with the default strategy
    When I attempt to initialize the framework again with another strategy
    Then the second initialization should be ignored
    And a warning should be logged about already initialized state
    And the original configuration should remain active

  @negative
  Scenario: Initialize with non-existent property file
    Given I have a PropertyFileBasedInitializationStrategy with path "nonexistent.properties"
    When I attempt to initialize the logging framework with this strategy
    Then a LoggingInitializationException should be thrown
    And the exception message should indicate the file was not found
    And the logging framework should remain uninitialized

  @negative
  Scenario: Initialize with malformed XML configuration
    Given I have an invalid log4cxx XML file "malformed-log4cxx.xml"
    And the XML contains syntax errors
    When I attempt to initialize the logging framework with this XML file
    Then a LoggingInitializationException should be thrown
    And the exception message should indicate XML parsing errors
    And the logging framework should remain uninitialized

  @cleanup
  Scenario: Proper shutdown and cleanup
    Given the logging framework is initialized with default strategy
    And several log messages have been written
    When I call the shutdown method on LoggingInitializer
    Then all appenders should flush their buffers
    And all file handles should be closed properly
    And the framework should be marked as uninitialized
    And subsequent initialization should be allowed

  @integration
  Scenario: Reinitialization after shutdown
    Given the logging framework was initialized and then shut down
    When I initialize the framework again with a different strategy
    Then the new initialization should succeed
    And the new configuration should be active
    And no state from the previous initialization should remain

  @factory
  Scenario: Factory creates correct strategy based on type enum
    Given I use LoggingInitializationFactory
    When I request a strategy of type "PropertyFile" with path "config.properties"
    Then the factory should return a PropertyFileBasedInitializationStrategy instance
    And the strategy should be properly configured with the given path

  @factory
  Scenario: Factory auto-detection of strategy type
    Given I have a configuration file "auto-config.xml" in the current directory
    When I call the factory's detectStrategy method
    Then it should return InitializationStrategyType::XMLFile
    And the detected strategy should work when used for initialization

  @factory
  Scenario: Factory registration of custom strategy
    Given I have implemented a custom strategy "CustomDatabaseInitializationStrategy"
    When I register the custom strategy with the factory using type "Custom"
    And I request a strategy of type "Custom" from the factory
    Then the factory should return an instance of my custom strategy
    And the custom strategy should work for initialization

  @thread-safety
  Scenario: Thread-safe initialization
    Given I have 10 concurrent threads
    And each thread attempts to initialize the logging framework
    When all threads execute simultaneously
    Then only one initialization should succeed
    And all threads should complete without errors
    And the logging framework should be in a valid initialized state
    And no race conditions should occur

  @performance
  Scenario: Initialization performance benchmark
    Given I have a complex XML configuration with 20 loggers and 10 appenders
    When I measure the initialization time
    Then initialization should complete in less than 100 milliseconds
    And memory usage should be reasonable (< 10MB overhead)

  @validation
  Scenario: Strategy validation before initialization
    Given I create a PropertyFileBasedInitializationStrategy with invalid file permissions
    When I call the validate method on the strategy
    Then the validation should fail
    And an appropriate error message should be provided
    And initialization should be prevented if validation fails