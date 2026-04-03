Feature: Logging Operations
  As a doevelopper
  I want to log messages at different levels with various formats
  So that I can track application behavior and diagnose issues

  Background:
    Given the logging framework is initialized with default configuration
    And a test file appender is configured to "logs/test-output.log"

  @smoke @critical
  Scenario: Log messages at all severity levels
    Given I have a CustomLogger instance for "TestLogger"
    When I log a TRACE message "Detailed trace information"
    And I log a DEBUG message "Debug diagnostic data"
    And I log an INFO message "Informational message"
    And I log a WARN message "Warning about potential issue"
    And I log an ERROR message "Error occurred during processing"
    And I log a FATAL message "Critical system failure"
    Then all six messages should appear in the log file
    And each message should have the correct severity level prefix
    And messages should appear in chronological order

  @integration
  Scenario: Log level filtering
    Given the root logger is set to INFO level
    And I have a CustomLogger instance for "FilterTest"
    When I log a TRACE message "Should not appear"
    And I log a DEBUG message "Should not appear either"
    And I log an INFO message "Should appear"
    And I log a WARN message "Should definitely appear"
    Then the log file should contain only INFO and WARN messages
    And TRACE and DEBUG messages should not be in the log file

  @integration
  Scenario: Hierarchical logger configuration
    Given I have configured logger "com.myapp" at DEBUG level
    And I have configured logger "com.myapp.service" at INFO level
    And the root logger is at WARN level
    When I get a logger "com.myapp.service.impl"
    And I log a DEBUG message "Debug from impl"
    And I log an INFO message "Info from impl"
    Then only the INFO message should appear in the log
    And the logger should inherit INFO level from "com.myapp.service"

  @formatting
  Scenario Outline: Formatted logging with parameters
    Given I have a CustomLogger instance for "FormattingTest"
    When I log an INFO message with format "<format>" and parameters <params>
    Then the log file should contain the formatted message "<expected>"

    Examples:
      | format                        | params                  | expected                     |
      | User %s logged in             | "john.doe"              | User john.doe logged in      |
      | Processing %d items           | 42                      | Processing 42 items          |
      | Temperature: %.2f°C           | 23.456                  | Temperature: 23.46°C         |
      | Status: %s, Code: %d          | "OK", 200               | Status: OK, Code: 200        |
      | %s executed in %d ms          | "Query", 150            | %s executed in 150 ms        |

  @modern-cpp
  Scenario: Variadic template formatting
    Given I have a CustomLogger instance with variadic template support
    When I call info("User {} logged in from {}", "alice", "192.168.1.100")
    And I call debug("Request took {}ms with status {}", 234, "success")
    Then the log should contain "User alice logged in from 192.168.1.100"
    And the log should contain "Request took 234ms with status success"

  @performance
  Scenario: Level checking before expensive operations
    Given the logger level is set to WARN
    And I have an expensive computation that takes 100ms
    When I check if DEBUG is enabled before computing
    And I only compute the debug message if DEBUG is enabled
    Then the expensive computation should not be executed
    And the log operation should complete in under 1ms

  @integration
  Scenario: Static logger factory methods
    When I get a logger using CustomLogger::getLogger("MyLogger")
    Then a logger with name "MyLogger" should be returned
    And the logger should be ready for use

  @integration
  Scenario: Root logger retrieval
    When I get the root logger using CustomLogger::getRootLogger()
    Then the root logger should be returned
    And it should be the parent of all other loggers

  @exception-handling
  Scenario: Logging with exceptions
    Given I have a CustomLogger instance
    And an exception occurred in the application
    When I log the exception with message "Failed to process request"
    Then the log should contain the error message
    And the log should contain the exception type
    And the log should contain the stack trace (if available)

  @thread-safety
  Scenario: Concurrent logging from multiple threads
    Given I have a CustomLogger instance
    And I have 50 threads that will log concurrently
    When each thread logs 1000 messages
    Then all 50000 messages should appear in the log
    And no messages should be corrupted or interleaved
    And no data races should occur

  @boundary
  Scenario: Logging very large messages
    Given I have a CustomLogger instance
    When I log a message that is 10MB in size
    Then the message should be logged successfully
    And the message should not be truncated
    Or the system should handle the large message gracefully with appropriate warning

  @boundary
  Scenario: Logging with special characters
    Given I have a CustomLogger instance
    When I log a message containing "Special chars: \n\t\r\"'<>&"
    Then the message should be properly escaped in the log
    And the log file should remain valid and parseable

  @performance @benchmark
  Scenario: Logging throughput measurement
    Given I have a CustomLogger with file appender
    When I log 100000 simple INFO messages as fast as possible
    Then the operation should complete in less than 5 seconds
    And the throughput should be at least 20000 messages per second
    And memory usage should remain stable

  @lazy-initialization
  Scenario: Logger lazy initialization
    Given the logging framework is initialized
    But no logger instances have been created yet
    When I first request a logger "LazyLogger"
    Then the logger should be created on first use
    And subsequent requests should return the same instance

  @null-safety
  Scenario: Logging null or empty messages
    Given I have a CustomLogger instance
    When I attempt to log a null message
    Then the logger should handle it gracefully
    And should log a placeholder like "(null)" or skip silently
    And should not crash or throw exceptions

  @integration
  Scenario: Logger name validation
    When I attempt to create a logger with name ""
    Then a valid logger should be returned (root logger)

    When I attempt to create a logger with name "valid.logger.name"
    Then a logger with that hierarchical name should be created

    When I attempt to create a logger with special chars in name "logger$%^&"
    Then the logger creation should either succeed with sanitized name
    Or throw an appropriate validation exception

  @configuration
  Scenario: Runtime log level change
    Given I have a logger "DynamicLogger" at INFO level
    And I have logged an INFO message "Initial message"
    When I change the logger level to DEBUG at runtime
    And I log a DEBUG message "After level change"
    Then both messages should appear in the log
    And the level change should take effect immediately

  @pattern
  Scenario: Custom log message patterns
    Given I configure the pattern to "%d{yyyy-MM-dd HH:mm:ss} | %5p | %c | %m%n"
    When I log an INFO message "Pattern test"
    Then the log output should match the custom pattern format
    And the timestamp should be in the specified format
    And the level should be right-aligned in 5 characters

  @context
  Scenario: Logging with location information
    Given the logger is configured to include location information
    When I log a message from a specific file and line number
    Then the log should include the source file name
    And the log should include the line number
    And the log should include the function/method name

  @async
  Scenario: Asynchronous logging behavior
    Given I configure an async appender with buffer size 1000
    When I log 500 messages rapidly
    Then the log method calls should return immediately
    And messages should be written to file asynchronously
    And all messages should eventually appear in the log file
    And message order should be preserved

  @filter
  Scenario: Custom log filters
    Given I have configured a custom filter that blocks messages containing "SECRET"
    When I log "This is a normal message"
    And I log "This contains SECRET data"
    And I log "Another normal message"
    Then the log should contain the first and third messages
    But the log should not contain the message with "SECRET"

  @multiple-loggers
  Scenario: Multiple loggers in same application
    Given I have logger "ModuleA" configured
    And I have logger "ModuleB" configured
    And I have logger "ModuleC" configured
    When each logger logs messages independently
    Then all messages should appear in the appropriate log destinations
    And logger names should be correctly identified in output
    And no crosstalk should occur between loggers