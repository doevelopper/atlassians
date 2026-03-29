Plugin Architecture Framework Implementation

## Context
You are tasked with designing and implementing a robust **Plugin Architecture Framework** for C++ applications that follows enterprise-grade architecture patterns and modern C++ best practices. This framework will be a standalone library that applications can link against to enable dynamic plugin loading, lifecycle management, and inter-plugin communication.

## Project Structure Requirements
Follow the existing project structure pattern:
```
src/
├── main/cpp/com/github/doevelopper/atlassians/plugin/
│   ├── core/              # Core plugin interfaces and base classes
│   ├── loader/            # Dynamic library loading mechanisms
│   ├── registry/          # Plugin registration and discovery
│   ├── lifecycle/         # Plugin lifecycle management
│   ├── messaging/         # Inter-plugin communication
│   ├── security/          # Plugin sandboxing and security
│   └── metadata/          # Plugin metadata and manifests
├── test/cpp/com/github/doevelopper/atlassians/plugin/
└── it/com/github/doevelopper/atlassians/features/
```

## Architecture Requirements

### 1. **Layered Architecture: Interface → Abstraction → Concrete**

Design a comprehensive three-tier architecture:

#### **Interface Layer** (Pure Abstract Interfaces)
- `IPlugin.hpp` - Core plugin interface that all plugins must implement
- `IPluginLoader.hpp` - Interface for loading plugins from various sources
- `IPluginRegistry.hpp` - Interface for plugin registration and discovery
- `IPluginMetadata.hpp` - Interface for plugin metadata access
- `IPluginContext.hpp` - Interface providing runtime context to plugins
- `IPluginHost.hpp` - Interface for host application services
- `IPluginCommunicator.hpp` - Interface for inter-plugin messaging
- `IPluginValidator.hpp` - Interface for plugin validation
- `IPluginDependencyResolver.hpp` - Interface for dependency resolution
- `IPluginSandbox.hpp` - Interface for plugin isolation/security

#### **Abstraction Layer** (Base Classes with Common Logic)
- `BasePlugin.hpp/.cpp` - Template Method pattern for plugin lifecycle
- `BasePluginLoader.hpp/.cpp` - Common loading logic (symbol resolution, etc.)
- `BasePluginRegistry.hpp/.cpp` - Common registry operations
- `AbstractPluginManager.hpp/.cpp` - Facade coordinating all plugin operations
- `BasePluginCommunicator.hpp/.cpp` - Message routing foundation
- `BasePluginValidator.hpp/.cpp` - Common validation workflows

#### **Concrete Layer** (Specific Implementations)
- `DynamicLibraryLoader.hpp/.cpp` - Load plugins from .so/.dll/.dylib files
- `StaticPluginLoader.hpp/.cpp` - Support statically linked plugins
- `JsonPluginMetadata.hpp/.cpp` - JSON-based plugin manifests
- `XMLPluginMetadata.hpp/.cpp` - XML-based plugin manifests
- `FileSystemPluginRegistry.hpp/.cpp` - Discover plugins from filesystem
- `ConfigurablePluginRegistry.hpp/.cpp` - Config-file-based plugin registry
- `EventBusPluginCommunicator.hpp/.cpp` - Event bus for plugin messaging
- `DirectPluginCommunicator.hpp/.cpp` - Direct plugin-to-plugin calls
- `VersionedDependencyResolver.hpp/.cpp` - Semantic versioning dependency resolution
- `BasicPluginSandbox.hpp/.cpp` - Basic plugin isolation
- `AdvancedPluginSandbox.hpp/.cpp` - Advanced sandboxing with process isolation

## Design Patterns to Implement

### Required Patterns:
1. **Abstract Factory Pattern** - Create families of related plugin components
2. **Factory Method Pattern** - Plugin instantiation
3. **Registry Pattern** - Central plugin registration and lookup
4. **Observer Pattern** - Plugin lifecycle events and notifications
5. **Strategy Pattern** - Different plugin loading strategies
6. **Template Method Pattern** - Standardized plugin lifecycle
7. **Dependency Injection** - Inject host services into plugins
8. **Service Locator** - Plugin service discovery
9. **Proxy Pattern** - Plugin isolation and sandboxing
10. **Facade Pattern** - Simplified plugin management API
11. **Chain of Responsibility** - Multi-stage plugin validation
12. **Mediator Pattern** - Inter-plugin communication coordination
13. **Composite Pattern** - Hierarchical plugin structures
14. **Decorator Pattern** - Add functionality wrappers to plugins

## Core Components to Implement

### 1. **Plugin Interface** (`IPlugin.hpp`)
```cpp
namespace com::github::doevelopper::atlassians::plugin {

enum class PluginState {
    Unloaded,
    Loaded,
    Initialized,
    Started,
    Stopped,
    Failed
};

class IPlugin {
public:
    virtual ~IPlugin() = default;

    // Lifecycle methods
    virtual void initialize(IPluginContext& context) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void shutdown() = 0;

    // Metadata access
    virtual const IPluginMetadata& getMetadata() const noexcept = 0;
    virtual PluginState getState() const noexcept = 0;

    // Capability queries
    virtual bool hasCapability(std::string_view capability) const = 0;
    virtual std::vector<std::string> getProvidedServices() const = 0;
    virtual std::vector<std::string> getRequiredServices() const = 0;
};

} // namespace
```

### 2. **Plugin Metadata** (`PluginMetadata.hpp/.cpp`)
```cpp
class PluginMetadata {
    std::string name;
    std::string version;           // Semantic versioning (e.g., "1.2.3")
    std::string description;
    std::string author;
    std::string license;
    std::string apiVersion;        // Required host API version

    std::vector<std::string> providedServices;
    std::vector<PluginDependency> dependencies;
    std::map<std::string, std::string> properties;

    std::optional<std::string> entryPoint;
    std::optional<std::string> icon;
    std::vector<std::string> supportedPlatforms;

    std::chrono::system_clock::time_point buildDate;
    std::string checksum;          // For integrity verification
};

struct PluginDependency {
    std::string pluginName;
    std::string versionConstraint; // e.g., ">=1.2.0, <2.0.0"
    bool optional;
};
```

### 3. **Plugin Context** (`IPluginContext.hpp`)
```cpp
class IPluginContext {
public:
    virtual ~IPluginContext() = default;

    // Service access
    virtual std::shared_ptr<void> getService(std::string_view serviceName) = 0;

    template<typename T>
    std::shared_ptr<T> getService(std::string_view serviceName) {
        return std::static_pointer_cast<T>(getService(serviceName));
    }

    // Configuration access
    virtual std::string getConfigValue(std::string_view key) const = 0;
    virtual std::filesystem::path getPluginDirectory() const = 0;
    virtual std::filesystem::path getDataDirectory() const = 0;

    // Logging
    virtual void log(LogLevel level, std::string_view message) = 0;

    // Event publishing
    virtual void publishEvent(const PluginEvent& event) = 0;
    virtual void subscribeToEvent(std::string_view eventType,
                                    EventHandler handler) = 0;
};
```

### 4. **Plugin Manager** (`PluginManager.hpp/.cpp`)
```cpp
class PluginManager {
public:
    static PluginManager& getInstance();

    // Plugin discovery and loading
    void scanForPlugins(const std::filesystem::path& directory);
    void loadPlugin(const std::filesystem::path& pluginPath);
    void loadPlugin(std::string_view pluginName);
    void unloadPlugin(std::string_view pluginName);

    // Plugin lifecycle management
    void initializePlugin(std::string_view pluginName);
    void startPlugin(std::string_view pluginName);
    void stopPlugin(std::string_view pluginName);
    void restartPlugin(std::string_view pluginName);

    // Plugin queries
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> getAvailablePlugins() const;
    std::shared_ptr<IPlugin> getPlugin(std::string_view name) const;
    PluginState getPluginState(std::string_view name) const;

    // Dependency management
    bool resolveDependencies(std::string_view pluginName);
    std::vector<std::string> getDependencyChain(std::string_view pluginName) const;

    // Hot-reload support
    void enableHotReload(bool enable);
    void reloadPlugin(std::string_view pluginName);

    // Event management
    void registerEventListener(std::string_view eventType, EventListener listener);
    void publishEvent(const PluginEvent& event);

private:
    PluginManager();
    ~PluginManager();

    // Singleton pattern with thread safety
    static std::unique_ptr<PluginManager> instance_;
    static std::once_flag initFlag_;
};
```

### 5. **Plugin Loader** (`DynamicLibraryLoader.hpp/.cpp`)
```cpp
class DynamicLibraryLoader : public BasePluginLoader {
public:
    std::unique_ptr<IPlugin> loadPlugin(const std::filesystem::path& path) override;
    void unloadPlugin(const std::string& pluginName) override;

private:
    using CreatePluginFunc = IPlugin* (*)();
    using DestroyPluginFunc = void (*)(IPlugin*);

    struct LoadedLibrary {
        void* handle;               // dlopen/LoadLibrary handle
        CreatePluginFunc createFunc;
        DestroyPluginFunc destroyFunc;
        std::filesystem::file_time_type lastModified;
    };

    std::map<std::string, LoadedLibrary> loadedLibraries_;

    void* loadLibrary(const std::filesystem::path& path);
    void* getSymbol(void* handle, std::string_view symbolName);
    void unloadLibrary(void* handle);
};
```

### 6. **Plugin Registry** (`PluginRegistry.hpp/.cpp`)
```cpp
class PluginRegistry : public IPluginRegistry {
public:
    void registerPlugin(const PluginMetadata& metadata,
                       std::shared_ptr<IPlugin> plugin);
    void unregisterPlugin(std::string_view name);

    std::vector<PluginMetadata> getAllPluginMetadata() const;
    std::optional<PluginMetadata> getPluginMetadata(std::string_view name) const;

    std::vector<std::string> findPluginsByCapability(std::string_view capability) const;
    std::vector<std::string> findPluginsByService(std::string_view service) const;

    bool isPluginRegistered(std::string_view name) const;

private:
    struct PluginEntry {
        PluginMetadata metadata;
        std::shared_ptr<IPlugin> plugin;
        PluginState state;
        std::chrono::system_clock::time_point registrationTime;
    };

    mutable std::shared_mutex mutex_;
    std::map<std::string, PluginEntry> plugins_;
    std::multimap<std::string, std::string> capabilityIndex_;
    std::multimap<std::string, std::string> serviceIndex_;
};
```

### 7. **Dependency Resolver** (`DependencyResolver.hpp/.cpp`)
```cpp
class DependencyResolver {
public:
    struct ResolutionResult {
        bool success;
        std::vector<std::string> loadOrder;
        std::vector<std::string> missingDependencies;
        std::vector<std::string> circularDependencies;
    };

    ResolutionResult resolve(const std::vector<PluginMetadata>& plugins);
    bool isVersionCompatible(std::string_view version,
                            std::string_view constraint) const;

private:
    std::vector<std::string> topologicalSort(
        const std::map<std::string, std::vector<std::string>>& graph);
    bool detectCircularDependencies(
        const std::map<std::string, std::vector<std::string>>& graph);
};
```

### 8. **Inter-Plugin Communication** (`EventBus.hpp/.cpp`)
```cpp
class EventBus : public IPluginCommunicator {
public:
    void publish(const PluginEvent& event);
    void subscribe(std::string_view eventType, EventHandler handler);
    void unsubscribe(std::string_view eventType, EventHandler handler);

    // Synchronous communication
    template<typename T>
    std::optional<T> request(std::string_view targetPlugin,
                            std::string_view method,
                            const std::any& params);

    // Asynchronous communication
    template<typename T>
    std::future<T> requestAsync(std::string_view targetPlugin,
                               std::string_view method,
                               const std::any& params);

private:
    struct Subscription {
        std::string eventType;
        EventHandler handler;
        std::weak_ptr<IPlugin> subscriber;
    };

    mutable std::shared_mutex mutex_;
    std::multimap<std::string, Subscription> subscriptions_;
    std::queue<PluginEvent> eventQueue_;
    std::thread eventDispatchThread_;
    std::atomic<bool> running_;
};
```

## Modern C++ Best Practices

### Must Use:
- **C++17/20 features**:
  - `std::filesystem` for plugin paths
  - `std::optional` for optional metadata
  - `std::variant` for plugin event payloads
  - `std::any` for type-erased plugin data
  - `std::shared_mutex` for reader-writer locks
  - Concepts (C++20) for plugin interface constraints
  - Modules (C++20) if available

- **Smart pointers**:
  - `std::unique_ptr` for plugin ownership
  - `std::shared_ptr` for shared plugin references
  - `std::weak_ptr` for non-owning plugin references

- **Move semantics**: Enable efficient plugin transfers
- **RAII**: Manage dynamic library handles, locks, threads
- **const correctness**: All query methods marked `const`
- **noexcept**: Mark non-throwing functions (especially destructors)
- **Strong typing**: `enum class` for states, type-safe IDs
- **std::chrono**: For timestamps, timeouts
- **Thread safety**: All public APIs must be thread-safe
- **Concepts**: Define plugin interface requirements

### Error Handling:
```cpp
class PluginException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class PluginLoadException : public PluginException {
public:
    explicit PluginLoadException(const std::filesystem::path& path)
        : PluginException("Failed to load plugin: " + path.string()),
          pluginPath_(path) {}

    const std::filesystem::path& getPluginPath() const { return pluginPath_; }
private:
    std::filesystem::path pluginPath_;
};

class PluginDependencyException : public PluginException {};
class PluginInitializationException : public PluginException {};
class PluginVersionMismatchException : public PluginException {};
class PluginCircularDependencyException : public PluginException {};
class PluginSecurityException : public PluginException {};
```

## Plugin Manifest Format

### JSON Manifest Example (`plugin.json`)
```json
{
  "name": "ExamplePlugin",
  "version": "1.2.3",
  "apiVersion": "2.0.0",
  "description": "Example plugin demonstrating the framework",
  "author": "doevelopper Name",
  "license": "MIT",
  "entryPoint": "libExamplePlugin.so",

  "capabilities": [
    "data_processing",
    "visualization"
  ],

  "providedServices": [
    "com.example.DataProcessor",
    "com.example.ChartRenderer"
  ],

  "dependencies": [
    {
      "name": "CorePlugin",
      "version": ">=1.0.0, <2.0.0",
      "optional": false
    },
    {
      "name": "UtilityPlugin",
      "version": "^1.5.0",
      "optional": true
    }
  ],

  "configuration": {
    "maxThreads": 4,
    "cacheSize": "100MB"
  },

  "platforms": ["linux", "windows", "macos"],
  "buildDate": "2025-01-15T10:30:00Z",
  "checksum": "sha256:abcdef123456..."
}
```

## Plugin Export Macro

### Platform-Specific Export (`PluginExport.hpp`)
```cpp
#if defined(_WIN32) || defined(_WIN64)
    #define PLUGIN_EXPORT __declspec(dllexport)
    #define PLUGIN_IMPORT __declspec(dllimport)
#elif defined(__GNUC__) || defined(__clang__)
    #define PLUGIN_EXPORT __attribute__((visibility("default")))
    #define PLUGIN_IMPORT
#else
    #define PLUGIN_EXPORT
    #define PLUGIN_IMPORT
#endif

#ifdef BUILDING_PLUGIN
    #define PLUGIN_API PLUGIN_EXPORT
#else
    #define PLUGIN_API PLUGIN_IMPORT
#endif

// Standard plugin entry points
extern "C" {
    PLUGIN_API IPlugin* createPlugin();
    PLUGIN_API void destroyPlugin(IPlugin* plugin);
    PLUGIN_API const char* getPluginApiVersion();
}
```

## Example Plugin Implementation

### Sample Plugin (`ExamplePlugin.hpp/.cpp`)
```cpp
// ExamplePlugin.hpp
#pragma once
#include <com/github/doevelopper/atlassians/plugin/core/BasePlugin.hpp>

namespace com::github::doevelopper::atlassians::plugin::examples {

class ExamplePlugin : public BasePlugin {
public:
    ExamplePlugin();
    ~ExamplePlugin() override;

    void initialize(IPluginContext& context) override;
    void start() override;
    void stop() override;
    void shutdown() override;

    const IPluginMetadata& getMetadata() const noexcept override;
    bool hasCapability(std::string_view capability) const override;

private:
    std::unique_ptr<IPluginContext> context_;
    PluginMetadata metadata_;
};

} // namespace

// ExamplePlugin.cpp
#include "ExamplePlugin.hpp"
#include <com/github/doevelopper/atlassians/plugin/core/PluginExport.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::plugin::examples {

ExamplePlugin::ExamplePlugin() {
    metadata_.name = "ExamplePlugin";
    metadata_.version = "1.0.0";
    metadata_.description = "Example plugin";
    // ... set other metadata
}

void ExamplePlugin::initialize(IPluginContext& context) {
    LOG_INFO("Initializing ExamplePlugin");
    context_ = std::make_unique<IPluginContext>(context);

    // Access host services
    auto dataService = context_->getService<IDataService>("DataService");

    // Subscribe to events
    context_->subscribeToEvent("data.updated", [this](const PluginEvent& event) {
        // Handle event
    });

    setState(PluginState::Initialized);
}

void ExamplePlugin::start() {
    LOG_INFO("Starting ExamplePlugin");
    // Start plugin operations
    setState(PluginState::Started);
}

void ExamplePlugin::stop() {
    LOG_INFO("Stopping ExamplePlugin");
    // Stop plugin operations
    setState(PluginState::Stopped);
}

void ExamplePlugin::shutdown() {
    LOG_INFO("Shutting down ExamplePlugin");
    context_.reset();
    setState(PluginState::Unloaded);
}

} // namespace

// Plugin entry points
extern "C" {
    PLUGIN_API IPlugin* createPlugin() {
        return new ExamplePlugin();
    }

    PLUGIN_API void destroyPlugin(IPlugin* plugin) {
        delete plugin;
    }

    PLUGIN_API const char* getPluginApiVersion() {
        return "2.0.0";
    }
}
```

## Testing Requirements

### 1. **Unit Tests** (`src/test/cpp/.../plugin/`)
```cpp
// PluginManagerTest.hpp/.cpp
class PluginManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager_ = &PluginManager::getInstance();
        testPluginDir_ = std::filesystem::temp_directory_path() / "test_plugins";
        std::filesystem::create_directories(testPluginDir_);
    }

    void TearDown() override {
        std::filesystem::remove_all(testPluginDir_);
    }

    PluginManager* manager_;
    std::filesystem::path testPluginDir_;
};

TEST_F(PluginManagerTest, LoadValidPlugin) {
    // Create mock plugin
    auto pluginPath = createMockPlugin("TestPlugin", "1.0.0");

    EXPECT_NO_THROW(manager_->loadPlugin(pluginPath));
    EXPECT_TRUE(manager_->getPlugin("TestPlugin") != nullptr);
    EXPECT_EQ(manager_->getPluginState("TestPlugin"), PluginState::Loaded);
}

TEST_F(PluginManagerTest, RejectInvalidApiVersion) {
    auto pluginPath = createMockPlugin("InvalidPlugin", "1.0.0", "0.5.0");

    EXPECT_THROW(manager_->loadPlugin(pluginPath),
                 PluginVersionMismatchException);
}

TEST_F(PluginManagerTest, ResolveDependencies) {
    // Create plugins with dependencies
    auto plugin1 = createMockPlugin("PluginA", "1.0.0");
    auto plugin2 = createMockPlugin("PluginB", "1.0.0", {{"PluginA", "^1.0.0"}});

    manager_->loadPlugin(plugin1);
    manager_->loadPlugin(plugin2);

    EXPECT_TRUE(manager_->resolveDependencies("PluginB"));

    auto chain = manager_->getDependencyChain("PluginB");
    EXPECT_EQ(chain.size(), 2);
    EXPECT_EQ(chain[0], "PluginA");
    EXPECT_EQ(chain[1], "PluginB");
}
```

Test coverage should include:
- `PluginLoaderTest.hpp/.cpp` - Dynamic library loading tests
- `PluginRegistryTest.hpp/.cpp` - Registry operations tests
- `DependencyResolverTest.hpp/.cpp` - Dependency resolution tests
- `EventBusTest.hpp/.cpp` - Inter-plugin communication tests
- `PluginMetadataTest.hpp/.cpp` - Metadata parsing tests
- `PluginLifecycleTest.hpp/.cpp` - Lifecycle state transitions

### 2. **Integration Tests** (`src/it/.../features/`)

Create BDD scenarios in `plugin_management.feature`:
```gherkin
Feature: Plugin Management

  Scenario: Load and initialize a valid plugin
    Given a plugin directory containing "ExamplePlugin"
    When I scan for plugins
    And I load the "ExamplePlugin"
    And I initialize the "ExamplePlugin"
    Then the plugin state should be "Initialized"
    And the plugin should provide service "com.example.DataProcessor"

  Scenario: Handle missing plugin dependencies
    Given a plugin "PluginB" that depends on "PluginA" version ">=1.0.0"
    And "PluginA" is not available
    When I attempt to load "PluginB"
    Then the load should fail
    And I should receive a "PluginDependencyException"
    And the error message should mention "PluginA"

  Scenario: Hot-reload a plugin after modification
    Given a loaded and started plugin "TestPlugin"
    When the plugin file is modified
    And hot-reload is enabled
    And I trigger a reload of "TestPlugin"
    Then the plugin should be reloaded
    And the plugin state should be "Started"
    And active connections should be preserved

  Scenario: Inter-plugin communication via event bus
    Given loaded plugins "PluginA" and "PluginB"
    And "PluginB" subscribes to event "data.processed"
    When "PluginA" publishes event "data.processed"
    Then "PluginB" should receive the event
    And the event payload should be intact
```

Implement step definitions in `src/it/.../features/steps/PluginManagement.cpp`

## Security Considerations

### Plugin Validation
```cpp
class PluginSecurityValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
    };

    ValidationResult validate(const std::filesystem::path& pluginPath);

private:
    bool verifySignature(const std::filesystem::path& path);
    bool checkChecksum(const std::filesystem::path& path,
                      std::string_view expectedChecksum);
    bool scanForMaliciousCode(const std::filesystem::path& path);
    bool validateManifest(const PluginMetadata& metadata);
    bool checkPermissions(const PluginMetadata& metadata);
};
```

### Plugin Sandboxing
```cpp
class PluginSandbox {
public:
    enum class IsolationLevel {
        None,           // No isolation (same process)
        Thread,         // Thread-based isolation
        Process         // Process-based isolation
    };

    void setIsolationLevel(IsolationLevel level);
    void restrictFileSystem(const std::vector<std::filesystem::path>& allowedPaths);
    void restrictNetwork(bool allowNetwork);
    void setMemoryLimit(size_t maxBytes);
    void setCpuLimit(double maxCpuPercent);

private:
    void enforceRestrictions();
};
```

### Security Requirements:
1. **Code Signing**: Verify plugin signatures before loading
2. **Checksum Verification**: Validate plugin integrity
3. **Permission System**: Declare and enforce plugin permissions
4. **Resource Limits**: Prevent resource exhaustion (CPU, memory, file descriptors)
5. **API Access Control**: Restrict access to sensitive host APIs
6. **Network Isolation**: Control plugin network access
7. **Filesystem Isolation**: Limit plugin filesystem access
8. **Audit Logging**: Log all plugin operations

## CMake Integration

### Main Plugin Library (`src/main/cpp/CMakeLists.txt`)
```cmake
# Plugin Framework Library
add_library(Premisses.plugin.core SHARED
    com/github/doevelopper/atlassians/plugin/core/BasePlugin.cpp
    com/github/doevelopper/atlassians/plugin/core/PluginManager.cpp
    com/github/doevelopper/atlassians/plugin/loader/DynamicLibraryLoader.cpp
    com/github/doevelopper/atlassians/plugin/registry/PluginRegistry.cpp
    com/github/doevelopper/atlassians/plugin/lifecycle/PluginLifecycleManager.cpp
    com/github/doevelopper/atlassians/plugin/messaging/EventBus.cpp
    com/github/doevelopper/atlassians/plugin/metadata/JsonPluginMetadata.cpp
    com/github/doevelopper/atlassians/plugin/security/PluginSecurityValidator.cpp
    com/github/doevelopper/atlassians/plugin/DependencyResolver.cpp
)

target_include_directories(Premisses.plugin.core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(Premisses.plugin.core
    PUBLIC
        ${CMAKE_DL_LIBS}        # Dynamic loading (dlopen)
        Threads::Threads
        nlohmann_json::nlohmann_json
    PRIVATE
        Premisses.logging
)

target_compile_features(Premisses.plugin.core PUBLIC cxx_std_17)

set_target_properties(Premisses.plugin.core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Export symbols for plugins
generate_export_header(Premisses.plugin.core
    BASE_NAME PLUGIN
    EXPORT_FILE_NAME com/github/doevelopper/atlassians/plugin/PluginExport.hpp
)

# Install targets
install(TARGETS Premisses.plugin.core
    EXPORT PremissesPluginTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY com/github/doevelopper/atlassians/plugin/
    DESTINATION include/com/github/doevelopper/atlassians/plugin
    FILES_MATCHING PATTERN "*.hpp"
)
```

### Example Plugin (`examples/ExamplePlugin/CMakeLists.txt`)
```cmake
# Example Plugin
add_library(ExamplePlugin MODULE
    ExamplePlugin.cpp
)

target_link_libraries(ExamplePlugin
    PRIVATE
        Premisses.plugin.core
        Premisses.logging
)

set_target_properties(ExamplePlugin PROPERTIES
    PREFIX ""                   # No "lib" prefix
    SUFFIX ".plugin"           # Custom extension (optional)
    CXX_VISIBILITY_PRESET hidden
)

# Copy plugin manifest
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin.json
    ${CMAKE_BINARY_DIR}/plugins/ExamplePlugin/plugin.json
    COPYONLY
)

# Install plugin
install(TARGETS ExamplePlugin
    LIBRARY DESTINATION plugins/ExamplePlugin
)
```

## Host Application Integration

### Using the Plugin Framework
```cpp
// main.cpp
#include <com/github/doevelopper/atlassians/plugin/PluginManager.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

int main(int argc, char* argv[]) {
    using namespace com::github::doevelopper::atlassians::plugin;

    try {
        // Initialize plugin manager
        auto& pluginManager = PluginManager::getInstance();

        // Configure plugin directories
        std::vector<std::filesystem::path> pluginDirs = {
            "./plugins",
            "/usr/local/share/myapp/plugins",
            std::filesystem::path(std::getenv("HOME")) / ".myapp/plugins"
        };

        // Scan for plugins
        LOG_INFO("Scanning for plugins...");
        for (const auto& dir : pluginDirs) {
            if (std::filesystem::exists(dir)) {
                pluginManager.scanForPlugins(dir);
            }
        }

        // Load and initialize plugins
        auto availablePlugins = pluginManager.getAvailablePlugins();
        LOG_INFO("Found {} plugins", availablePlugins.size());

        for (const auto& pluginName : availablePlugins) {
            try {
                LOG_INFO("Loading plugin: {}", pluginName);
                pluginManager.loadPlugin(pluginName);

                // Resolve dependencies
                if (pluginManager.resolveDependencies(pluginName)) {
                    pluginManager.initializePlugin(pluginName);
                    pluginManager.startPlugin(pluginName);
                    LOG_INFO("Plugin started: {}", pluginName);
                } else {
                    LOG_WARN("Failed to resolve dependencies for: {}", pluginName);
                }
            } catch (const PluginException& e) {
                LOG_ERROR("Failed to load plugin {}: {}", pluginName, e.what());
            }
        }

        // Enable hot-reload in development mode
        if (isDevMode()) {
            pluginManager.enableHotReload(true);
        }

        // Register event listener
        pluginManager.registerEventListener("application.shutdown",
            [&](const PluginEvent& event) {
                LOG_INFO("Shutting down plugins...");
                for (const auto& name : pluginManager.getLoadedPlugins()) {
                    pluginManager.stopPlugin(name);
                    pluginManager.unloadPlugin(name);
                }
            });

        // Run application
        runApplication();

        // Cleanup
        pluginManager.publishEvent(PluginEvent{"application.shutdown"});

    } catch (const std::exception& e) {
        LOG_ERROR("Fatal error: {}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
```

## Advanced Features

### 1. **Hot Reload Support**
```cpp
class HotReloadMonitor {
public:
    void startMonitoring(const std::filesystem::path& pluginDir);
    void stopMonitoring();

    void onFileChanged(FileChangeCallback callback);

private:
    void monitorThread();
    std::map<std::filesystem::path, std::filesystem::file_time_type> fileTimestamps_;
    std::atomic<bool> running_;
    std::thread monitorThread_;
};
```

### 2. **Plugin Versioning and Migration**
```cpp
class PluginMigrationManager {
public:
    void registerMigration(std::string_view fromVersion,
                          std::string_view toVersion,
                          MigrationFunction migration);

    bool migratePlugin(const PluginMetadata& oldMetadata,
                      const PluginMetadata& newMetadata);

private:
    std::map<std::pair<std::string, std::string>, MigrationFunction> migrations_;
};
```

### 3. **Plugin Performance Monitoring**
```cpp
class PluginPerformanceMonitor {
public:
    struct PerformanceMetrics {
        std::chrono::nanoseconds initializationTime;
        std::chrono::nanoseconds averageEventProcessingTime;
        size_t memoryUsage;
        double cpuUsagePercent;
        size_t eventCount;
    };

    void startMonitoring(std::string_view pluginName);
    PerformanceMetrics getMetrics(std::string_view pluginName) const;
};
```

### 4. **Plugin Configuration Management**
```cpp
class PluginConfiguration {
public:
    void loadConfiguration(const std::filesystem::path& configFile);
    void saveConfiguration(const std::filesystem::path& configFile);

    template<typename T>
    T getValue(std::string_view key, const T& defaultValue = T{}) const;

    template<typename T>
    void setValue(std::string_view key, const T& value);

    void bindToPlugin(IPlugin& plugin);
};
```

## Logging Integration

Use existing logging infrastructure:
```cpp
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

void PluginManager::loadPlugin(const std::filesystem::path& path) {
    LOG_INFO("Loading plugin from: {}", path.string());

    try {
        auto metadata = parsePluginMetadata(path);
        LOG4CXX_DEBUG("Plugin metadata: name={}, version={}",
                  metadata.name, metadata.version);

        // Loading logic...

        LOG4CXX_INFO("Successfully loaded plugin: {}", metadata.name);
    } catch (const PluginLoadException& e) {
        LOG4CXX_ERROR("Failed to load plugin: {}", e.what());
        throw;
    }
}
```

## API Documentation

Use Doxygen-style comments:
```cpp
/**
 * @brief Manages the lifecycle and operations of plugins.
 *
 * The PluginManager is responsible for discovering, loading, initializing,
 * and managing plugins throughout their lifecycle. It provides thread-safe
 * operations and supports hot-reloading in development mode.
 *
 * @note This class is a thread-safe singleton.
 *
 * @example
 * @code
 * auto& manager = PluginManager::getInstance();
 * manager.scanForPlugins("./plugins");
 * manager.loadPlugin("MyPlugin");
 * manager.initializePlugin("MyPlugin");
 * @endcode
 */
class PluginManager {
    // ...
};
```

## Deliverables

1. **Complete Plugin Framework Implementation**
   - All interfaces, abstractions, and concrete implementations
   - Thread-safe plugin manager with singleton pattern
   - Dynamic library loader with platform abstraction
   - Dependency resolver with semantic versioning
   - Event bus for inter-plugin communication
   - Plugin registry with capability indexing
   - Security validator and sandbox implementation

2. **Example Plugins**
   - At least 3 example plugins demonstrating different features
   - Simple plugin (minimal implementation)
   - Advanced plugin (with dependencies, services, events)
   - UI plugin (providing visual components)

3. **Comprehensive Testing Suite**
   - Unit tests with >90% code coverage
   - Integration tests with mock plugins
   - BDD scenarios for plugin lifecycle
   - Performance benchmarks

4. **Documentation**
   - API reference documentation (Doxygen)
   - Plugin doevelopper guide
   - Host application integration guide
   - Security best practices
   - Troubleshooting guide

5. **Build System**
   - CMake configuration for framework library
   - CMake helper functions for plugin creation
   - Example plugin build configurations
   - Installation scripts

6. **Tools and Utilities**
   - Plugin validator tool (command-line)
   - Plugin manifest generator
   - Plugin dependency visualizer
   - Performance profiler

## Quality Gates

- ✅ All code compiles without warnings (`-Wall -Wextra -Werror -pedantic`)
- ✅ All unit tests pass
- ✅ All integration tests pass
- ✅ Code coverage exceeds 90%
- ✅ Static analysis (clang-tidy, cppcheck) passes
- ✅ Memory leak check (valgrind, AddressSanitizer) passes
- ✅ Thread safety analysis (ThreadSanitizer) passes
- ✅ API documentation is complete
- ✅ Example plugins build and run successfully
- ✅ Performance benchmarks meet targets

## Performance Targets

- Plugin loading: < 50ms per plugin (without initialization)
- Plugin initialization: < 200ms per plugin
- Event dispatch: < 1ms latency
- Memory overhead: < 5MB per plugin (framework only)
- Dependency resolution: O(n log n) time complexity
- Hot reload: < 100ms for single plugin

## Platform Support

Support the following platforms:
- **Linux**: GCC 9+, Clang 10+
- **Windows**: MSVC 2019+, MinGW-w64
- **macOS**: Clang 12+ (Xcode 12+)

Platform-specific implementations:
- Dynamic library loading (`dlopen` on Unix, `LoadLibrary` on Windows)
- File system monitoring (inotify on Linux, ReadDirectoryChangesW on Windows)
- Process isolation (fork on Unix, CreateProcess on Windows)

## Additional Considerations

- **Backward Compatibility**: Support loading plugins from older API versions
- **Plugin Marketplace**: Design for future plugin marketplace integration
- **License Compliance**: Track plugin licenses and dependencies
- **Crash Isolation**: Prevent plugin crashes from affecting the host application
- **Resource Cleanup**: Ensure proper cleanup even when plugins misbehave
- **Debugging Support**: Provide hooks for debuggers and profilers
- **Telemetry**: Optional telemetry for plugin usage analytics
- **Plugin Updates**: Support seamless plugin updates
- **Multi-Language Support**: Design for potential language bindings (Python, Lua, etc.)


**Style Guide References**

**Class Example:**
`src/main/cpp/com/github/doevelopper/atlassians/demo/Calculator.{cpp,hpp}`

**Unit Test Example:**
`src/test/cpp/com/github/doevelopper/atlassians/demo/CalculatorTest.{cpp,hpp}`

**Logging Include:**
`#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>`

**Calculator step definitions**
`src/it/com/github/doevelopper/atlassians/features/steps/Calculator.cpp`

**Calculator Gherkin scenarios**
`src/it/com/github/doevelopper/atlassians/features/calculator.feature`


Command line used to create build directory
```sh
cmake -E make_directory target
```
Command line used to generate build configurations
```sh
cmake -E chdir target cmake .. -DCMAKE_INSTALL_PREFIX=install
```

Command line used to build project
```sh
cmake --build target --target all --parallel 20
```

Command line used to run some resulted binary
```sh
cmake --build target --target Premisses.main.bin 2>&1 | tail -20 && echo "---RUNNING---" && ./target/src/main/cpp/Premisses.main.bin-1.0.0 2>&1
```

Command line used to run unit test manually
```sh
cmake --build target --target Premisses.test.bin 2>&1 | tail -20 && echo "---RUNNING---" && ./target/src/test/cpp/Premisses.test.bin 2>&1
```
