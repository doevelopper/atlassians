Feature: MDC and NDC Context Management
  As a doevelopper debugging distributed applications
  I want to use Mapped Diagnostic Context (MDC) and Nested Diagnostic Context (NDC)
  So that I can correlate log messages with requests and execution contexts

  Background:
    Given the logging framework is initialized
    And the log pattern includes MDC and NDC values

  @critical @context
  Scenario: Basic MDC usage with RAII scope
    Given I have a CustomLogger instance
    When I create an MDC scope with key "requestId" and value "req-12345"
    And I log an INFO message "Processing request"
    And the MDC scope goes out of scope
    And I log another INFO message "After scope"
    Then the first message should include "requestId=req-12345" in the context
    And the second message should not include the requestId context

  @context
  Scenario: Multiple MDC values in scope
    Given I have a CustomLogger instance
    When I create MDC scope "requestId" = "req-001"
    And I create MDC scope "userId" = "user-42"
    And I create MDC scope "sessionId" = "sess-999"
    And I log "Processing with full context"
    Then the log should include all three MDC values
    And the values should be correctly associated with their keys

  @context
  Scenario: Nested MDC scopes
    Given I have a CustomLogger instance
    When I create MDC scope "operation" = "checkout"
    And within that scope, I create MDC scope "step" = "validate"
    And I log "Validating checkout"
    And the inner scope ends
    And I log "Checkout continuing"
    Then the first message should have both "operation" and "step"
    And the second message should only have "operation"

  @context
  Scenario: MDC inheritance across function calls
    Given I have set MDC value "traceId" = "trace-abc"
    When I call a function that logs a message
    Then the logged message in the function should include "traceId=trace-abc"
    And the MDC context should be inherited by nested calls

  @thread-local @context
  Scenario: MDC is thread-local
    Given I set MDC "threadId" = "thread-1" in thread 1
    And I set MDC "threadId" = "thread-2" in thread 2
    When both threads log messages simultaneously
    Then thread 1's logs should show "threadId=thread-1"
    And thread 2's logs should show "threadId=thread-2"
    And there should be no cross-contamination between threads

  @context @ndc
  Scenario: Basic NDC push and pop
    Given I have a CustomLogger instance
    When I push "ServiceLayer" onto the NDC stack
    And I push "DataAccess" onto the NDC stack
    And I log "Querying database"
    And I pop from NDC stack
    And I log "Service continuing"
    Then the first message should show NDC as "ServiceLayer DataAccess"
    And the second message should show NDC as "ServiceLayer"

  @context @ndc
  Scenario: NDC with RAII scope
    Given I have a CustomLogger instance
    When I create NDC scope "RequestHandler"
    And within that, I create NDC scope "Authentication"
    And I log "Checking credentials"
    And the inner NDC scope ends
    And I log "Handler continuing"
    Then the first message should have "RequestHandler Authentication"
    And the second message should have only "RequestHandler"

  @context
  Scenario: Clear all MDC values
    Given I have set multiple MDC values
    When I call clear() on the MDC
    And I log a message
    Then the log should not contain any MDC values
    And the MDC should be empty for subsequent operations

  @context
  Scenario: MDC with null or empty values
    Given I have a CustomLogger instance
    When I set MDC "emptyKey" to empty string ""
    And I log a message
    Then the MDC should handle the empty value gracefully
    And the log should show "emptyKey=" or handle it appropriately

  @context @web-application
  Scenario: HTTP request correlation with MDC
    Given I have a web application using the logging framework
    When an HTTP request arrives with header "X-Request-ID: req-http-456"
    And I set MDC "requestId" to the header value
    And I log at various points during request processing
    Then all log messages for this request should include "requestId=req-http-456"
    And I can correlate all logs for this single request

  @context @microservices
  Scenario: Distributed tracing context propagation
    Given I have a microservices architecture
    When service A calls service B with traceId "trace-123"
    And service A sets MDC "traceId" = "trace-123"
    And service B receives the traceId and sets its own MDC
    And both services log messages
    Then logs from both services should include the same traceId
    And I can trace the request across service boundaries

  @context @performance
  Scenario: MDC performance impact
    Given I have MDC enabled
    When I log 100000 messages with 5 MDC values each
    Then the performance overhead should be acceptable (< 20% compared to no MDC)
    And the throughput should remain reasonable

  @context
  Scenario: MDC key collision handling
    Given I set MDC "key1" = "value1"
    When I set MDC "key1" = "value2" (overwriting)
    And I log a message
    Then the log should show "key1=value2"
    And the old value should be completely replaced

  @context @cleanup
  Scenario: MDC cleanup on thread termination
    Given I have a thread that sets MDC values
    When the thread completes execution
    Then the MDC values should be automatically cleaned up
    And there should be no memory leaks from MDC storage

  @context @exception
  Scenario: MDC scope cleanup with exceptions
    Given I create an MDC scope "transactionId" = "tx-789"
    When an exception is thrown within the scope
    Then the MDC scope should still be cleaned up (RAII)
    And the MDC value should be removed despite the exception
    And no MDC leakage should occur

  @context @async
  Scenario: MDC propagation to async tasks
    Given I set MDC "parentContext" = "main-thread"
    When I spawn an async task
    And I explicitly copy MDC to the async task
    Then the async task should have access to "parentContext"
    And modifications in the async task should not affect parent thread

  @context @ndc @async
  Scenario: NDC not automatically propagated to new threads
    Given I push "MainThread" to NDC stack
    When I create a new thread
    And I log from the new thread
    Then the new thread should have an empty NDC stack
    And the parent thread's NDC should be unaffected

  @context
  Scenario: MDC with structured logging patterns
    Given I configure pattern to output MDC as JSON
    When I set multiple MDC values
    And I log a message
    Then the log output should contain a valid JSON object with MDC values
    And the JSON should be properly formatted and parseable

  @context @integration
  Scenario: MDC integration with log4cxx filters
    Given I configure a filter that requires MDC "environment" = "production"
    When I set MDC "environment" = "production" and log
    Then the message should pass through the filter
    When I set MDC "environment" = "development" and log
    Then the message should be filtered out

  @context
  Scenario: Get current MDC values programmatically
    Given I have set several MDC values
    When I query the current MDC map
    Then I should receive all currently set key-value pairs
    And the map should accurately reflect the current context

  @context
  Scenario: NDC depth limit handling
    Given I have an NDC stack
    When I push 1000 values onto the NDC stack
    Then the stack should handle the depth gracefully
    Or enforce a reasonable depth limit with clear error
    And should not cause stack overflow

  @context
  Scenario: MDC with special characters in keys and values
    Given I set MDC with key "key-with-dash" = "value with spaces"
    And I set MDC with key "unicode_key" = "🎉 emoji value"
    When I log a message
    Then the special characters should be properly handled
    And the log output should be correctly formatted

  @context @pattern
  Scenario: Conditional MDC output in pattern
    Given I configure pattern to output MDC only if present
    When I log with MDC values set
    Then the MDC should appear in the output
    When I log without MDC values
    Then the MDC section should be omitted or show as empty
    And the log format should remain valid

  @best-practices @context
  Scenario: MDC cleanup best practices with try-finally
    Given I need to ensure MDC cleanup even with errors
    When I use MDC in a try-finally or RAII pattern
    And an error occurs in the try block
    Then the finally block or destructor should clean up MDC
    And MDC should not leak to subsequent operations