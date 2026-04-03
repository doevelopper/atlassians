# ApplicationFSM BDD Integration Tests

## Overview

This directory contains Behavior-Driven Development (BDD) integration tests for the ApplicationFSM (Application Lifecycle State Machine). The tests use:

- **Gherkin**: Feature files with Given/When/Then scenarios
- **cucumber-cpp**: C++ step definition framework with wire protocol
- **Ruby Cucumber**: Test runner that connects to C++ wire server

## Directory Structure

```
src/it/com/github/doevelopper/premisses/launcher/fsm/
├── features/
│   ├── application_lifecycle.feature    # Comprehensive lifecycle scenarios
│   ├── fsm_core.feature                # Core FSM functionality tests
│   └── step_definitions/
│       ├── ApplicationFSMSteps.cpp     # C++ step definitions
│       └── cucumber.wire                # Wire protocol configuration
```

## Building

Build the BDD test executable:

```bash
# Configure (if not already done)
cmake --preset gcc-release

# Build FSM wire server executable
cmake --build --preset compile-gcc-release --target Premisses.fsm.wire.steps
```

The executable is created at:
```
target/gcc-release/it/bin/Premisses.fsm.wire.steps
```

## Running Tests

### Method 1: Manual Execution

1. Start the wire server in the background:
```bash
./target/gcc-release/it/bin/Premisses.fsm.wire.steps --port 3902 --verbose &
```

2. Run cucumber from the fsm directory:
```bash
cd src/it/com/github/doevelopper/premisses/launcher/fsm
bundle exec cucumber features/ --format pretty
```

3. Stop the wire server when done:
```bash
pkill -f "Premisses.fsm.wire.steps"
```

### Method 2: Run Specific Feature Files

```bash
# Run only core functionality tests
bundle exec cucumber features/fsm_core.feature

# Run specific scenarios by line number
bundle exec cucumber features/fsm_core.feature:13

# Run scenarios matching a pattern
bundle exec cucumber features/ --name "shutdown"
```

### Method 3: Using CTest (when configured)

```bash
cd target/gcc-release
ctest -R Premisses.fsm.wire
```

## Test Coverage

### fsm_core.feature (18 scenarios, 75 steps)

**Basic Startup (5 scenarios)**
- FSM creation and initialization
- Pre-validation success
- Configuration loading success
- License validation success
- Core initialization success

**Failure Handling (2 scenarios)**
- Pre-validation failure with retry
- Configuration load failure

**Shutdown (2 scenarios)**
- Graceful shutdown
- Emergency shutdown

**State Queries (3 scenarios)**
- Query current state name
- Check operational status
- Access FSM context

**Configuration (4 scenarios)**
- Custom configuration with timeouts
- Validate valid configuration
- Validate invalid configuration
- Custom logging configuration

**Complete Lifecycle (2 scenarios)**
- Full lifecycle from creation to shutdown
- Verbose logging control

### application_lifecycle.feature (comprehensive scenarios)

Additional advanced scenarios covering:
- Pre-validation retries and exhaustion
- Configuration validation errors
- License expiration and grace periods
- Plugin discovery and failures
- Health degradation and restoration
- Core recovery workflows
- Concurrent operations

## Step Definitions

The C++ step definitions in `ApplicationFSMSteps.cpp` map Gherkin steps to ApplicationFSM API calls:

### Background Steps
- `Given I have an FSM with default configuration`

### FSM Creation Steps
- `When I create the FSM`
- `When I start the FSM`
- `Given I have a started FSM`

### Event Processing Steps
- `When I process "{event_name}" event`
- `When I process "{event_name}" event with reason "{reason}"`

### Lifecycle Helper Steps
- `When I complete pre-validation`
- `When I complete configuration loading`
- `When I complete all initialization phases`
- `When I process all startup events successfully`

### Shutdown Steps
- `When I initiate graceful shutdown with reason "{reason}"`
- `When I initiate emergency shutdown`

### Assertion Steps
- `Then the FSM should be in the initial state`
- `Then the pre-validation should succeed`
- `Then the configuration should be loaded`
- `Then the license should be validated`
- `Then the core should be initialized`
- `Then the FSM should handle the failure`
- `Then the FSM should begin shutdown`
- `Then the FSM should stop immediately`

### State Query Steps
- `Then I should be able to query the current state name`
- `Then I should be able to check if FSM is operational`
- `Then I should be able to access the FSM context`

### Configuration Steps
- `Given I configure the FSM with:` (table parameter)
- `When I validate a configuration with valid settings`
- `When I validate a configuration with negative recovery attempts`
- `Then the validation should pass with no errors`
- `Then the validation should fail with error message`

### Logging Steps
- `When I enable verbose logging`
- `When I disable verbose logging`
- `Then verbose logging should be active`

## Wire Protocol

The `cucumber.wire` file configures how Ruby Cucumber connects to the C++ wire server:

```yaml
host: localhost
port: 3902
timeout:
  connect: 10
  invoke: 120
```

## Troubleshooting

### Wire server won't start
- Check if port 3902 is already in use: `lsof -i :3902`
- Kill any existing server: `pkill -f Premisses.fsm.wire.steps`

### Cucumber can't connect to wire server
- Verify the server is running and listening
- Check the wire server log output for errors
- Ensure `cucumber.wire` port matches server port

### Step definitions not found
- Verify the wire server compiled successfully
- Check that `ApplicationFSMSteps.cpp` contains the step regex patterns
- Run with `--verbose` to see wire protocol communication

### Tests timeout
- Increase timeout in `cucumber.wire` (invoke: 300)
- Check for deadlocks in FSM implementation
- Run individual scenarios to isolate the problem

## Code Coverage

To run BDD tests with code coverage:

```bash
# Build with coverage instrumentation
cmake --preset gcc-coverage
cmake --build --preset compile-gcc-coverage --target Premisses.fsm.wire.steps

# Run tests
./target/gcc-coverage/it/bin/Premisses.fsm.wire.steps --port 3902 &
cd src/it/com/github/doevelopper/premisses/launcher/fsm
bundle exec cucumber features/
pkill -f Premisses.fsm.wire.steps

# Generate coverage report
cmake --build --preset compile-gcc-coverage --target code-coverage-report
```

## CI/CD Integration

For automated testing in CI/CD pipelines, use the CTest integration:

```bash
# Build all integration test targets
cmake --build --preset compile-gcc-release --target it-compile

# Run all BDD tests
cd target/gcc-release
ctest -L bdd -V
```

## Writing New Scenarios

1. Add scenarios to `features/*.feature` files using Gherkin syntax
2. Run cucumber to see which step definitions are missing
3. Add corresponding C++ step definitions to `ApplicationFSMSteps.cpp`
4. Rebuild the wire server executable
5. Run the tests again

Example:
```gherkin
Scenario: My new test
  Given I have a started FSM
  When I do something new
  Then something should happen
```

## References

- [Cucumber Documentation](https://cucumber.io/docs/)
- [cucumber-cpp GitHub](https://github.com/cucumber/cucumber-cpp)
- [Gherkin Syntax](https://cucumber.io/docs/gherkin/)
- [BDD Best Practices](https://cucumber.io/docs/bdd/)
