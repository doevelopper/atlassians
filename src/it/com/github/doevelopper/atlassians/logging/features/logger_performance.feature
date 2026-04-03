Feature: Performance Benchmarks and Load Testing
  As a performance engineer
  I want to verify the logging framework performs well under various conditions
  So that it doesn't become a bottleneck in production systems

  Background:
    Given the logging framework is initialized with optimal configuration
    And performance monitoring is enabled

  @performance @benchmark
  Scenario: Single-threaded logging throughput
    Given I have a single thread logging to a buffered file appender
    When I log 1,000,000 simple INFO messages
    Then the operation should complete in less than 10 seconds
    And throughput should exceed 100,000 messages per second
    And CPU usage should be reasonable (< 50% of one core)

  @performance @benchmark
  Scenario: Multi-threaded logging throughput
    Given I have 8 threads logging concurrently
    And each thread will log 100,000 messages
    When all threads log simultaneously
    Then all 800,000 messages should be written correctly
    And the operation should complete in less than 15 seconds
    And overall throughput should exceed 50,000 messages per second
    And no messages should be lost or corrupted

  @performance @latency
  Scenario: Logging latency measurement - P99
    Given I have logging configured with sync file appender
    When I measure latency for 100,000 log calls
    Then the P50 (median) latency should be under 10 microseconds
    And the P95 latency should be under 50 microseconds
    And the P99 latency should be under 100 microseconds
    And the P99.9 latency should be under 1 millisecond

  @performance @async
  Scenario: Async appender performance
    Given I configure an async appender with queue size 10,000
    When I log 500,000 messages rapidly
    Then log method calls should return in under 1 microsecond
    And all messages should eventually be written to disk
    And the async queue should not overflow
    And message ordering should be preserved

  @performance @memory
  Scenario: Memory footprint under normal load
    Given I have 50 active loggers configured
    When the application logs 10,000 messages per second for 1 hour
    Then memory usage should remain stable (no memory leak)
    And RSS should not exceed 100 MB for the logging subsystem
    And memory should be reclaimed after operations complete

  @performance @memory
  Scenario: Memory efficiency with many loggers
    Given I create 10,000 logger instances
    When I measure the memory footprint
    Then total memory usage should be under 50 MB
    And each logger should use minimal memory (< 5 KB average)
    And logger creation should be fast (< 10 microseconds per logger)

  @performance @gc
  Scenario: Minimal garbage collection pressure
    Given I log 1,000,000 messages
    When I monitor memory allocations
    Then heap allocations should be minimized
    And most allocations should be stack-based or pooled
    And GC pressure should be minimal (for managed languages)

  @performance @fileio
  Scenario: File I/O performance with buffering
    Given I configure a file appender with 64 KB buffer
    When I log messages totaling 100 MB
    Then actual file writes should be minimized through buffering
    And I/O operations should be efficient (large sequential writes)
    And fsync calls should be batched appropriately

  @performance @cpu
  Scenario: CPU usage under heavy load
    Given I log at maximum throughput
    When I monitor CPU usage
    Then logging should use no more than 2 CPU cores at 100%
    And CPU usage should scale reasonably with thread count
    And should not cause CPU starvation for other threads

  @performance @scalability
  Scenario: Horizontal scalability
    Given I double the number of logging threads from 4 to 8
    When I measure throughput for each configuration
    Then throughput should increase proportionally (near-linear scaling)
    And per-thread performance should remain consistent
    And lock contention should be minimal

  @performance @filtering
  Scenario: Log level filtering performance
    Given the log level is set to ERROR
    When I make 1,000,000 DEBUG level log calls (filtered out)
    Then all calls should be filtered before string construction
    And the operation should complete in under 1 second
    And filtering overhead should be minimal (< 10 nanoseconds per call)

  @performance @formatting
  Scenario: String formatting overhead
    Given I log messages with format strings and 5 parameters
    When I log 100,000 formatted messages
    Then formatting overhead should be acceptable
    And should be faster than sprintf/printf equivalents
    And should allocate memory efficiently

  @performance @pattern
  Scenario: Pattern layout performance
    Given I configure a complex pattern: "%d{ISO8601} [%t] %-5p %c{2} %F:%L - %m%n"
    When I log 100,000 messages
    Then pattern parsing and rendering should be efficient
    And should complete in under 5 seconds
    And pattern elements should be cached where possible

  @performance @mdc
  Scenario: MDC context overhead
    Given I have 10 MDC values set
    When I log 100,000 messages with MDC context
    Then MDC lookup overhead should be under 5% compared to no MDC
    And MDC context should be efficiently stored (thread-local)
    And MDC should not cause significant performance degradation

  @performance @appender
  Scenario: Multiple appenders performance impact
    Given I configure 5 different appenders (console, 3 files, syslog)
    When I log 100,000 messages
    Then all appenders should receive all messages
    And total time should be close to the slowest appender time
    And parallelization should be used where possible

  @performance @rotation
  Scenario: Log rotation performance
    Given I have a rolling file appender with 10 MB size limit
    When I log messages that trigger 10 rotations
    Then rotation operations should complete quickly (< 100ms each)
    And rotation should not block logging significantly
    And old files should be renamed/compressed efficiently

  @performance @network
  Scenario: Network appender throughput
    Given I configure a network appender (syslog/socket)
    When I log 10,000 messages over the network
    Then the appender should not block on slow network
    And should use connection pooling or keep-alive
    And should batch messages for network efficiency

  @performance @compression
  Scenario: Log compression overhead
    Given I enable GZIP compression on file appender
    When I log 1 GB of log data
    Then compression should reduce file size significantly (> 70%)
    And compression overhead should be acceptable (< 30% time increase)
    And compressed logs should be readable by standard tools

  @stress @performance
  Scenario: Sustained load stress test
    Given the logging framework is under sustained heavy load
    When I log 10,000 messages per second for 24 hours
    Then performance should remain stable throughout
    And no performance degradation should occur over time
    And memory usage should remain constant (no leaks)
    And all messages should be successfully logged

  @stress @performance
  Scenario: Burst traffic handling
    Given the system normally logs 100 messages per second
    When I simulate bursts of 50,000 messages per second for 10 seconds
    Then the burst should be handled without message loss
    And the system should return to normal after the burst
    And no instability should result from the burst

  @performance @cold-start
  Scenario: Cold start initialization time
    Given the logging framework is uninitialized
    When I measure the time to initialize
    Then initialization should complete in under 100 milliseconds
    And first log message should be written within 150ms of init
    And cold start should not significantly delay application startup

  @performance @jit-warmup
  Scenario: JIT warmup and steady-state performance
    Given I log messages during JIT warmup phase
    When I compare warmup performance to steady-state
    Then steady-state performance should be 2-5x better than cold
    And performance should stabilize after ~10,000 messages
    And hot paths should be optimized by JIT/compiler

  @performance @comparison
  Scenario: Performance comparison with raw file I/O
    Given I benchmark logging framework vs raw file writes
    When I write 100,000 identical strings to file
    Then logging framework overhead should be under 3x raw I/O
    And the overhead should be justified by features
    And for most use cases, performance should be acceptable

  @performance @zero-copy
  Scenario: Zero-copy optimization where possible
    Given I log pre-formatted strings without parameters
    When I measure allocations and copies
    Then unnecessary string copies should be minimized
    And string_view or similar should be used where possible
    And move semantics should be leveraged

  @performance @lock-free
  Scenario: Lock-free or low-contention logging
    Given I use a lock-free or low-contention implementation
    When 16 threads log concurrently
    Then lock contention should be minimal
    And wait times should be microseconds or better
    And scalability should be near-linear up to core count

  @performance @resource-limited
  Scenario: Performance on resource-constrained systems
    Given I limit available CPU to 1 core and RAM to 512 MB
    When I log under these constraints
    Then the framework should adapt gracefully
    And should not exhaust available resources
    And should provide reasonable performance given constraints

  @benchmark @comparison
  Scenario: Benchmark against other logging frameworks
    Given I compare with spdlog, Boost.Log, and glog
    When I run standardized benchmarks
    Then log4cxx wrapper should be competitive
    And should be within 2x performance of fastest framework
    And feature set should justify any performance differences

  @performance @profiling
  Scenario: CPU profiling to identify hotspots
    Given I profile logging under heavy load
    When I analyze the CPU profile
    Then no single function should dominate (> 30% time)
    And hotspots should be in expected places (I/O, formatting)
    And lock contention should be minimal (< 5% of time)

  @performance @cache
  Scenario: CPU cache efficiency
    Given I measure cache misses during logging
    When I log from multiple threads
    Then cache miss rate should be reasonable (< 10%)
    And data structures should be cache-friendly
    And false sharing should be avoided

  @performance @regression
  Scenario: Performance regression detection
    Given I have baseline performance metrics from previous version
    When I run the same benchmarks on new version
    Then performance should not regress by more than 5%
    And any significant changes should be documented
    And regression tests should catch performance issues