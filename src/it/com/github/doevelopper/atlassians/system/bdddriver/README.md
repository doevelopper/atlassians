# BDD Driver - Cucumber-CPP Test Driver with Logging Integration

## Overview

The **BDD Driver** (`bdddriver`) provides logging integration and environment management for Behavior-Driven Development (BDD) tests using cucumber-cpp. It uses **static initialization** to set up logging before cucumber-cpp's built-in main() starts, ensuring all BDD scenarios have proper logging from the start.

## Purpose

- **Unified Logging**: Initializes logging framework before BDD tests run via static initialization
- **Automatic Setup**: No custom main() needed - uses cucumber-cpp's built-in wire server
- **Global Environment**: Provides setup/teardown hooks for BDD test lifecycle
- **Consistent Architecture**: Mirrors the `utdriver` pattern used for unit tests
- **Zero Configuration**: Works out-of-the-box with just a library linkage

## Architecture

```
src/it/com/github/doevelopper/premisses/bdddriver/
├── BddEnvironment.hpp              # Global BDD environment (like CustomTestEnvironment)
├── BddEnvironment.cpp              # Setup/teardown implementation
├── BddStaticInit.cpp               # Static initializer (runs before main)
└── README.md                       # This file
```

**Note**: Step executables use cucumber-cpp's built-in main(). The bdddriver library provides static initialization that runs automatically before main() starts.

## Components

### 1. BddDriver

**File**: [BddDriver.hpp](com/github/doevelopper/premisses/bdddriver/BddDriver.hpp) | [BddDriver.cpp](com/github/doevelopper/premisses/bdddriver/BddDriver.cpp)

The main facade that manages the BDD test lifecycle:

- Initializes logging framework
- Parses command line arguments
- Starts cucumber-cpp wire protocol server
- Manages test execution and cleanup

**Key Methods**:

- `setup(argc, argv)` - Initialize BDD framework
- `run()` - Start wire server and execute scenarios
- `execute(argc, argv)` - Combined setup and run
- `teardown()` - Cleanup resources

**Configuration Options**:

```cpp
auto config = BddDriverConfig::Builder()
    .withWirePort(3902)           // Default cucumber wire port
    .withVerboseOutput(true)      // Enable verbose logging
    .withColorOutput(true)        // Colored output
    .withStartupTimeout(10000)    // Startup timeout in ms
    .withStepsFilter(".*")        // Filter step definitions
    .build();
```

### 2. BddEnvironment

**File**: [BddEnvironment.hpp](com/github/doevelopper/premisses/bdddriver/BddEnvironment.hpp) | [BddEnvironment.cpp](com/github/doevelopper/premisses/bdddriver/BddEnvironment.cpp)

Global environment for BDD tests providing:

- **SetUp()**: Called once before any scenarios run
  - Initializes logging
  - Sets up global resources
  - Configures environment

- **TearDown()**: Called once after all scenarios complete
  - Cleans up logging
  - Releases global resources
  - Final cleanup

### 3. Main Entry Point

**File**: [main.cpp](main.cpp)

The BDD test executable entry point that:

1. Initializes logging system
2. Sets up BDD environment
3. Creates and configures BddDriver
4. Starts wire server and waits for cucumber
5. Handles exceptions and returns exit code

## Usage

### In Step Definitions

Step definition files work exactly as before - just link with the bdddriver library:

```cpp
// Calculator.cpp
#include <cucumber-cpp/autodetect.hpp>
#include <com/github/doevelopper/premisses/demo/Calculator.hpp>

GIVEN("^a calculator$")
{
    ScenarioScope<CalculatorContext> context;
    context->calculator = std::make_unique<Calculator>();
    // Logging is already initialized - just use it!
}

// ... more step definitions
```

### In CMakeLists.txt

```cmake
# Create a step definitions executable
# Automatically uses cucumber-cpp's built-in main()
# Logging initialization happens via static initialization in bdddriver
bdd_add_steps_executable(${PROJECT_NAME}.demo.wire.steps
    SOURCES
        com/github/doevelopper/premisses/demo/features/step_definitions/Calculator.cpp
    LINK_LIBS
        ${PROJECT_NAME}.demo
        ${PROJECT_NAME}.bdddriver      # Provides static initialization
        CUCUMBER-CPP::CUCUMBER-CPP     # Provides main()
        GTest::gtest
    CXX_STD 20
)
```

**Key Points**:

- No `MAIN` parameter needed - uses cucumber-cpp's built-in main()
- Link against `${PROJECT_NAME}.bdddriver` for automatic logging initialization
- Link against `CUCUMBER-CPP::CUCUMBER-CPP` for wire server

### Running BDD Tests

BDD tests are normally run via CMake/CTest:

```bash
# Run all BDD tests
ctest -L bdd

# Run specific suite
ctest -R demo.wire

# Run with verbose output
ctest -VV -R acceptance.wire
```

For manual debugging:

```bash
# Terminal 1: Start wire server (logging will initialize automatically)
./target/gcc-debug/it/bin/Premisses.demo.wire.steps --port 3902 --verbose

# Terminal 2: Run cucumber
cd src/it/com/github/doevelopper/premisses/demo
bundle exec cucumber features/
```

## How It Works

### Static Initialization Flow

1. **Before main()**: `BddStaticInitializer` constructor runs
   - Initializes logging framework
   - Sets up BddEnvironment
   - Logs startup message

2. **cucumber-cpp main()** starts
   - Wire protocol server listens on port

- Accepts connections from cucumber runner
  - Executes step definitions
  - **Logging is already working!**

3. **After main()**: `BddStaticInitializer` destructor runs
   - Tears down BddEnvironment
   - Cleans up logging
   - Logs shutdown message

This approach is simpler and more maintainable than a custom main(). The static initialization "tricks" ensure everything is set up before cucumber starts.

## Logging

The BDD driver initializes logging using the same framework as the main application and unit tests:

- Logs BDD lifecycle events (setup, scenarios, teardown)
- Logs wire server status (listening, connections, errors)
- Logs step execution (when verbose mode enabled)
- Integrates with log4cxx for consistent output

**Log Categories**:

- `com.github.doevelopper.premisses.bdd.main` - Main entry point
- `com.github.doevelopper.premisses.bdddriver.BddDriver` - Driver lifecycle
- `com.github.doevelopper.premisses.bdddriver.BddEnvironment` - Environment setup/teardown

## Command Line Options

The BDD step executables support the following options:

```
--port <port>       Wire protocol port (default: 3902)
--verbose, -v       Enable verbose output
--no-color          Disable colored output
--filter <regex>    Filter step definitions by regex
--help, -h          Display help message
```

## Comparison with Unit Test Driver

| Feature         | Unit Test (utdriver)       | BDD Test (bdddriver)        |
| --------------- | -------------------------- | --------------------------- |
| Entry Point     | `src/test/cpp/main.cpp`    | `src/it/main.cpp`           |
| Driver Class    | `TestDriver`               | `BddDriver`                 |
| Environment     | `CustomTestEnvironment`    | `BddEnvironment`            |
| Test Framework  | GoogleTest                 | Cucumber-CPP                |
| Execution Model | Direct test execution      | Wire protocol server        |
| Configuration   | GoogleTest flags           | Wire port, verbosity        |
| Library Target  | `${PROJECT_NAME}.utdriver` | `${PROJECT_NAME}.bdddriver` |

## Integration with Build System

The BDD driver is seamlessly integrated into the CMake build:

1. **Library Build**: `${PROJECT_NAME}.bdddriver` static library
2. **Step Executables**: Linked with bdddriver and step definitions
3. **CTest Integration**: Tests registered with `bdd` label
4. **Report Generation**: HTML reports generated in `${BUILD_DIR}/qa/`
5. **Wire Protocol**: Automatic server start/stop via test harness

## Benefits

1. **Automatic Logging**: No manual initialization needed - it just works
2. **Simpler Architecture**: Uses cucumber-cpp's proven wire server implementation
3. **Less Code**: No custom main() or wire server management
4. **Better Debugging**: Detailed logs help diagnose BDD test failures
5. **Custom Hooks**: Extensible environment for project-specific setup
6. **Code Reuse**: Shares logging framework with all other test layers
7. **Maintainability**: Less custom code means fewer bugs

## Extension Points

### Custom BddEnvironment

```cpp
// MyBddEnvironment.hpp
class MyBddEnvironment : public BddEnvironment
{
protected:
    void onEnvironmentSetup() override
    {
        LOG4CXX_INFO(logger, "Custom BDD setup - initializing test database");
        // Initialize databases, mock services, etc.
    }

    void onEnvironmentTeardown() override
    {
        LOG4CXX_INFO(logger, "Custom BDD teardown - cleaning up test database");
        // Cleanup databases, mock services, etc.
    }
};

// In BddStaticInit.cpp, use MyBddEnvironment instead
```

## Future Enhancements

- [ ] Step definition filtering and selective execution
- [ ] Performance metrics collection
- [ ] Integration with test result reporting
- [ ] Parallel scenario execution support
- [ ] Custom wire protocol extensions (Option B - for advanced use cases)

## Design Decision: Option A vs Option B

This implementation uses **Option A** (Static Initialization):

**✅ Option A: Static Initialization with cucumber-cpp's main** (Current)

- **Pros**: Simple, maintainable, less custom code, proven wire server
- **Cons**: Less control over wire server details
- **Best for**: Most projects, standard BDD testing needs

**Option B: Custom Wire Server Main** (Future Enhancement)

- **Pros**: Full control over wire protocol, custom server configuration
- **Cons**: More complex, more code to maintain, potential bugs
- **Best for**: Advanced scenarios requiring custom wire protocol behavior

Choose Option B only if you need custom wire protocol features not provided by cucumber-cpp.

## See Also

- [Unit Test Driver Documentation](../../test/cpp/com/github/doevelopper/premisses/utdriver/README.md)
- [BDD Feature Files](../features/)
- [Cucumber-CPP Documentation](https://github.com/cucumber/cucumber-cpp)
- [Project Testing Documentation](../../../docs/testing.md)
