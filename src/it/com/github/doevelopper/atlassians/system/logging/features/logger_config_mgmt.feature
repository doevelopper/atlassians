Feature: Configuration Management and Factory Behavior
  As a DevOps engineer
  I want flexible configuration management for the logging framework
  So that I can adapt logging to different environments and requirements

  Background:
    Given the logging framework is not yet initialized

  @factory @critical
  Scenario: Factory creates strategy based on type enum
    Given I use the LoggingInitializationFactory
    When I request a strategy with type "PropertyFile" and path "config.properties"
    Then the factory should return a PropertyFileBasedInitializationStrategy
    And the strategy should be configured with the correct path
    And the strategy should be ready to initialize

  @factory
  Scenario: Factory auto-detection of configuration files
    Given I have "log4cxx.properties" in the current directory
    And I have "log4cxx.xml" in the config directory
    When I call factory.detectStrategy()
    Then the factory should prioritize based on detection rules
    And should return the appropriate strategy type
    And the strategy should use the detected file

  @factory
  Scenario: Factory detection priority order
    Given multiple configuration files exist in search paths
    When the factory performs auto-detection
    Then it should check in priority order:
      | Priority | Location                              | File Pattern      |
      | 1        | Environment variable path             | Any supported     |
      | 2        | ./log4cxx.xml                         | XML format        |
      | 3        | ./log4cxx.properties                  | Properties format |
      | 4        | ~/.config/appname/log4cxx.xml         | User config       |
      | 5        | /etc/appname/log4cxx.xml              | System config     |

  @factory @extensibility
  Scenario: Register custom strategy with factory
    Given I have implemented "MyCustomStrategy" implementing ILoggingInitializationStrategy
    When I register the custom strategy with type identifier "Custom_Database"
    And I request a strategy of type "Custom_Database"
    Then the factory should return an instance of MyCustomStrategy
    And the custom strategy should work correctly for initialization

  @factory @negative
  Scenario: Factory handles unknown strategy type
    When I request a strategy with an unregistered type "UnknownType"
    Then the factory should throw an exception
    And the exception should clearly indicate the type is unknown
    And should suggest available strategy types

  @configuration @reload
  Scenario: Dynamic configuration reload from file
    Given the framework is initialized with XML configuration
    And the XML file is being watched for changes
    When I modify the XML configuration file
    And save the changes
    Then the framework should detect the file change within 5 seconds
    And should reload the configuration automatically
    And the new configuration should take effect without restart

  @configuration @reload
  Scenario: Hot reload without losing messages
    Given the framework is actively logging messages
    When a configuration reload is triggered
    Then no log messages should be lost during reload
    And logging should continue seamlessly
    And there should be a log message indicating successful reload

  @configuration
  Scenario: Fallback configuration chain
    Given primary configuration file "production.xml" doesn't exist
    And fallback configuration file "default.xml" exists
    When I initialize with fallback chain
    Then the framework should use "default.xml"
    And should log a warning about missing primary config
    And should continue with fallback successfully

  @configuration @validation
  Scenario: Pre-initialization configuration validation
    Given I have a configuration file with invalid appender references
    When I validate the configuration before initialization
    Then the validation should fail
    And should report specific validation errors:
      | Error Type           | Description                              |
      | Missing Appender     | Appender "FileAppender" not defined      |
      | Invalid Log Level    | Level "SUPER_DEBUG" is not valid         |
      | Circular Reference   | Logger hierarchy has circular dependency |

  @configuration @properties
  Scenario: Property file with variable substitution
    Given I have a properties file with variables:
      """
      log4j.rootLogger=${log.level},${log.appender}
      log.level=INFO
      log.appender=console
      """
    When I initialize with property substitution enabled
    Then variables should be resolved correctly
    And the root logger should use resolved values

  @configuration @xml
  Scenario: XML configuration with includes
    Given I have a main XML file that includes other XML files
    And the main XML contains: <include file="appenders.xml"/>
    When I initialize with the main XML file
    Then the framework should load and merge included files
    And appenders from included files should be available
    And the merged configuration should be valid

  @configuration @environment
  Scenario Outline: Environment-specific configuration selection
    Given the environment variable "APP_ENV" is set to "<environment>"
    When the factory performs environment-based selection
    Then it should select configuration file "<config_file>"
    And the configuration should match the environment

    Examples:
      | environment | config_file                    |
      | development | log4cxx-development.xml        |
      | staging     | log4cxx-staging.xml            |
      | production  | log4cxx-production.xml         |
      | test        | log4cxx-test.xml               |

  @configuration @programmatic
  Scenario: Fluent API for programmatic configuration
    Given I use the programmatic strategy builder
    When I chain configuration calls:
      """cpp
      builder.setRootLevel(Level::DEBUG)
             .addConsoleAppender()
             .addFileAppender("/var/log/app.log")
             .setPattern("%d{ISO8601} %-5p [%t] %c - %m%n")
             .addFilter(LevelRangeFilter(INFO, ERROR))
             .build()
      """
    Then each method should return the builder for chaining
    And the final build() should create a valid strategy
    And the strategy should contain all configured elements

  @configuration @programmatic
  Scenario: Programmatic configuration validation
    Given I build a programmatic configuration
    But I forget to set a required parameter
    When I call build()
    Then the builder should validate the configuration
    And should throw an exception for missing requirements
    And should indicate what's missing in the error message

  @configuration @merge
  Scenario: Merge multiple configuration sources
    Given I have a base configuration from XML
    And I have overrides from environment variables
    And I have programmatic overrides
    When I initialize with merged configuration
    Then programmatic overrides should take highest priority
    Then environment variables should override XML
    And XML should provide base defaults
    And the final configuration should be the correct merge

  @configuration @security
  Scenario: Secure configuration with encrypted passwords
    Given my XML configuration contains encrypted database passwords
    And I have a decryption key available
    When the configuration is loaded
    Then encrypted values should be decrypted
    And decrypted values should be used securely
    And plaintext passwords should not appear in logs

  @configuration @schema
  Scenario: XML configuration schema validation
    Given I have an XML configuration file
    When I enable XSD schema validation
    And the XML file conforms to the schema
    Then validation should pass
    And the configuration should load successfully

  @configuration @schema @negative
  Scenario: Reject invalid XML against schema
    Given I have an XML file that violates the schema
    When I attempt to load it with validation enabled
    Then schema validation should fail
    And detailed validation errors should be reported
    And the framework should not initialize with invalid config

  @configuration @defaults
  Scenario: Sensible default configuration
    When I initialize without providing any configuration
    Then the framework should use sensible defaults:
      | Aspect          | Default Value                        |
      | Root Level      | INFO                                 |
      | Appender        | ConsoleAppender                      |
      | Pattern         | %d{ISO8601} %-5p %c - %m%n          |
      | Target          | stdout                               |

  @configuration @migration
  Scenario: Backward compatibility with log4j 1.x config
    Given I have a log4j 1.x properties file
    When I attempt to load it with log4cxx 1.6.0
    Then the framework should detect the format
    And should attempt compatibility translation
    Or should provide clear migration guidance
    And should not silently fail with old config

  @configuration @documentation
  Scenario: Self-documenting configuration
    When I request configuration documentation
    Then the framework should provide:
      | Information            | Details                                      |
      | Available appenders    | List of all built-in appender types          |
      | Available layouts      | List of all layout types and their patterns  |
      | Available filters      | List of filter types and their parameters    |
      | Configuration examples | Sample configurations for common scenarios   |

  @configuration @export
  Scenario: Export current configuration
    Given the framework is running with active configuration
    When I request the current configuration export
    Then I should receive the configuration in requested format (XML/Properties/JSON)
    And the exported config should be valid and reloadable
    And should include all current settings

  @configuration @diff
  Scenario: Compare configurations
    Given I have two configuration files: "config-old.xml" and "config-new.xml"
    When I use the configuration diff utility
    Then it should show differences:
      | Change Type | Element              | Old Value | New Value |
      | Modified    | rootLogger level     | INFO      | DEBUG     |
      | Added       | appender             | -         | fileApp   |
      | Removed     | filter               | denyAll   | -         |

  @configuration @templating
  Scenario: Configuration templates
    Given I use a configuration template for microservices
    When I apply the template with service-specific parameters
    Then the framework should generate a complete configuration
    And should fill in template variables with provided values
    And the result should be a valid, ready-to-use configuration

  @configuration @versioning
  Scenario: Configuration versioning and rollback
    Given I have version-controlled configurations
    When I need to rollback to a previous configuration version
    Then I should be able to reload the historical configuration
    And the framework should switch to the previous settings
    And should log the configuration change with version info

  @performance @configuration
  Scenario: Configuration loading performance
    Given I have a complex configuration with 100 loggers and 50 appenders
    When I load and parse the configuration
    Then the loading should complete in less than 1 second
    And memory usage should be reasonable (< 5MB)
    And subsequent logger access should be fast (< 1μs)

  @configuration @monitoring
  Scenario: Configuration health check endpoint
    Given the logging framework is running
    When I query the configuration health endpoint
    Then I should receive status information:
      | Metric                   | Example Value        |
      | Configuration loaded     | true                 |
      | Configuration source     | /etc/app/log4cxx.xml |
      | Number of loggers        | 15                   |
      | Number of appenders      | 5                    |
      | Last reload time         | 2025-01-15 10:30:00  |
      | Configuration valid      | true                 |

  @configuration @portability
  Scenario: Platform-independent configuration
    Given I have a single configuration file
    When I deploy to different platforms (Linux, Windows, macOS)
    Then the configuration should work on all platforms
    And platform-specific paths should be handled transparently
    And no manual adjustments should be needed