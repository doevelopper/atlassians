Feature: Error Handling and Resilience
  As a system operator
  I want the logging framework to handle errors gracefully
  So that logging failures don't crash the application

  Background:
    Given the logging framework is initialized

  @critical @resilience
  Scenario: Logging never throws exceptions
    Given I have a CustomLogger instance
    When I log a message and a critical error occurs in the appender
    Then no exception should be thrown from the logging call
    And the application should continue running
    And the error should be reported to stderr or error handler

  @negative
  Scenario: File system full during logging
    Given I configure a FileAppender to a partition that is almost full
    When the file system runs out of space during logging
    Then the logger should detect the disk full condition
    And an error should be logged to stderr
    And the application should continue running
    And subsequent log messages should be handled gracefully (queued or dropped)

  @negative
  Scenario: Log file permissions denied
    Given I configure a FileAppender to "/root/protected.log"
    And I don't have write permissions to that location
    When I attempt to initialize the logging framework
    Then the initialization should report a permission error
    And the framework should either fallback to console logging
    Or clearly indicate the failure without crashing

  @negative
  Scenario: Network appender connection failure
    Given I have a RemoteAppender configured to send logs to "unreachable.server:514"
    When the network connection fails
    Then the logger should detect the connection failure
    And log messages should be queued or dropped based on configuration
    And the application should not block waiting for network
    And the failure should be reported

  @resilience
  Scenario: Appender failure with multiple appenders
    Given I have three appenders configured: FileAppender, ConsoleAppender, and RemoteAppender
    When the FileAppender fails due to disk error
    Then the ConsoleAppender and RemoteAppender should continue working
    And log messages should still be visible on console and remote
    And the failed appender should be marked as unavailable

  @error-recovery
  Scenario: Automatic recovery from transient errors
    Given I have a FileAppender that encounters a transient error
    When the error condition is resolved (e.g., disk space freed)
    Then the appender should automatically resume logging
    And no messages should be permanently lost
    Or the lost message count should be reported

  @negative
  Scenario: Invalid log level specified
    Given I have a logger instance
    When I attempt to set an invalid log level "SUPER_DEBUG"
    Then the operation should fail gracefully
    And an appropriate error message should be provided
    And the logger should retain its previous valid level

  @negative
  Scenario: Circular reference in logger hierarchy
    Given I attempt to configure a logger hierarchy with circular references
    When the framework detects the circular dependency
    Then it should reject the configuration
    And provide a clear error message about the circular reference
    And not enter an infinite loop

  @resource-exhaustion
  Scenario: Handle file descriptor exhaustion
    Given I configure 1000 FileAppenders
    When the system runs out of file descriptors
    Then the framework should report the resource exhaustion
    And should fail gracefully without crashing
    And should provide guidance on reducing appender count

  @negative
  Scenario: Corrupted configuration file
    Given I have a configuration file with syntax errors
    When I attempt to load the configuration
    Then a clear parsing error should be reported
    And the error should include line number and error details
    And the framework should fallback to default configuration

  @memory @negative
  Scenario: Memory allocation failure handling
    Given I simulate an out-of-memory condition
    When the logger attempts to allocate memory for log messages
    Then it should handle the allocation failure gracefully
    And should not crash the application
    And may drop the message with a warning to stderr

  @thread-safety @negative
  Scenario: Deadlock prevention
    Given I have multiple loggers used by multiple threads
    And complex locking scenarios exist
    When threads log intensively under high contention
    Then no deadlocks should occur
    And logging should complete successfully for all threads
    And reasonable performance should be maintained

  @negative
  Scenario: Invalid format string handling
    Given I have a logger with formatted logging
    When I provide a format string with mismatched parameters
    Then the logger should detect the format mismatch
    And should log an error about the invalid format
    And should not crash or produce corrupted output

  @exception-safety
  Scenario: Exception in custom appender
    Given I have a custom appender that throws an exception
    When the appender's append method throws during logging
    Then the exception should be caught by the framework
    And should not propagate to the caller
    And an error should be logged about the appender failure

  @negative
  Scenario: Recursive logging detection
    Given I have a logging appender that itself logs messages
    When this creates a recursive logging situation
    Then the framework should detect the recursion
    And should break the cycle to prevent infinite recursion
    And should log a warning about the recursive behavior

  @resilience
  Scenario: Graceful degradation under load
    Given the logging system is under extreme load
    And appenders are struggling to keep up
    When the message queue reaches capacity
    Then the framework should implement backpressure
    Or start dropping older messages with appropriate warnings
    And should not cause the application to hang

  @monitoring @error
  Scenario: Error metrics and monitoring
    Given I have error tracking enabled
    When various error conditions occur during logging
    Then error counts should be tracked by type
    And error metrics should be available for monitoring
    And I should be able to query the error statistics

  @configuration @negative
  Scenario: Missing required configuration parameters
    Given I create a strategy that requires specific configuration parameters
    When required parameters are missing
    Then the validation should fail
    And a clear error message should indicate what's missing
    And initialization should be prevented

  @cleanup @error
  Scenario: Proper cleanup after errors
    Given an error occurs during logging initialization
    When the initialization fails
    Then all partially initialized resources should be cleaned up
    And no resource leaks should occur
    And the system should be in a clean state for retry

  @negative
  Scenario: Log message size limits
    Given the logging framework has a maximum message size of 64KB
    When I attempt to log a message larger than the limit
    Then the message should be truncated to the limit
    And a warning should be appended indicating truncation
    And the logger should not crash or allocate excessive memory

  @security @negative
  Scenario: Malicious input handling
    Given I log a message containing malicious content
    When the message contains format string exploits like "%n%n%n"
    Then the message should be safely escaped or sanitized
    And no format string vulnerability should be exploited
    And the log should be written safely

  @async @error
  Scenario: Async queue overflow handling
    Given I have an async appender with a finite queue size
    When the queue becomes full due to slow I/O
    Then the framework should either block the caller
    Or drop messages based on overflow policy
    And should log a warning about the overflow condition

  @negative
  Scenario: Clock/time source failure
    Given the logging framework depends on system time
    When the system clock is unavailable or returns errors
    Then the logger should handle the time failure gracefully
    And should either use a fallback timestamp or skip timestamps
    And should continue logging other parts of the message

  @windows-specific @negative
  Scenario: Windows path handling errors
    Given I'm running on Windows
    When I specify invalid Windows paths with illegal characters
    Then the path validation should catch the errors
    And provide Windows-specific error messages
    And suggest valid path alternatives