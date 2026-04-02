Feature: Custom Appenders
  As a security-conscious doevelopper
  I want to use custom appenders like EncryptedFileAppender
  So that sensitive log data is protected at rest

  Background:
    Given the logging framework is initialized
    And I have a test encryption key configured

  @critical @security
  Scenario: EncryptedFileAppender encrypts log messages
    Given I configure an EncryptedFileAppender to "logs/encrypted.log"
    And the appender uses AES-256 encryption
    When I log sensitive data "Password: secret123"
    Then the log file should contain encrypted data
    And the raw file content should not be human-readable
    And the sensitive data should not appear in plaintext in the file

  @integration @security
  Scenario: Decrypt and verify encrypted logs
    Given I have an EncryptedFileAppender configured with a known key
    When I log multiple messages with sensitive data
    And I use the decryption utility with the same key
    Then I should be able to decrypt and read all logged messages
    And the decrypted content should match the original log messages
    And the message order should be preserved

  @security
  Scenario: EncryptedFileAppender with different encryption algorithms
    Given I configure an EncryptedFileAppender with algorithm "<algorithm>"
    When I log a message "Test message for <algorithm>"
    Then the message should be encrypted using <algorithm>
    And decryption with the correct algorithm should succeed
    And decryption with wrong algorithm should fail

    Examples:
      | algorithm   |
      | AES-128-CBC |
      | AES-256-CBC |
      | AES-256-GCM |

  @negative @security
  Scenario: Decryption fails with wrong key
    Given I have an EncryptedFileAppender with encryption key "key1"
    And I log several messages
    When I attempt to decrypt the log file using key "key2"
    Then the decryption should fail
    And an appropriate error should be reported
    And no partial plaintext should be leaked

  @security
  Scenario: Key rotation support
    Given I have an EncryptedFileAppender with key "old-key"
    And I log messages "Message 1" and "Message 2"
    When I rotate the encryption key to "new-key"
    And I log message "Message 3" with the new key
    Then messages 1 and 2 should be encrypted with old key
    And message 3 should be encrypted with new key
    And both sets should be decryptable with their respective keys

  @security @configuration
  Scenario: Load encryption key from environment variable
    Given the environment variable "LOG_ENCRYPTION_KEY" is set to "env-secret-key"
    When I configure an EncryptedFileAppender to load key from environment
    Then the appender should use the key from the environment variable
    And logging should work correctly with this key

  @security @configuration
  Scenario: Load encryption key from keystore file
    Given I have a keystore file "keys/encryption.keystore"
    And the keystore contains a key with alias "log-encryption-key"
    When I configure an EncryptedFileAppender to load key from keystore
    Then the appender should retrieve the correct key from the keystore
    And encryption should work with the keystore key

  @negative @security
  Scenario: Missing encryption key handling
    Given I configure an EncryptedFileAppender
    But no encryption key is provided
    When I attempt to log a message
    Then an appropriate exception should be thrown
    Or the appender should fail to initialize
    And clear error message about missing key should be provided

  @performance @security
  Scenario: Encryption performance overhead
    Given I have a standard FileAppender and an EncryptedFileAppender
    When I log 10000 messages to each appender
    Then the encrypted appender should complete within 2x the time of standard appender
    And the performance overhead should be acceptable (< 100% increase)

  @integration
  Scenario: Encrypted appender with log rotation
    Given I configure an EncryptedFileAppender with size-based rotation
    And the rotation threshold is 1MB
    When I log messages until file size exceeds 1MB
    Then the appender should rotate to a new encrypted file
    And both the old and new files should be properly encrypted
    And the rotation should not corrupt any messages

  @security
  Scenario: Encrypted appender file permissions
    Given I configure an EncryptedFileAppender to "logs/secure.log"
    When the log file is created
    Then the file permissions should be restrictive (0600 or equivalent)
    And only the file owner should have read/write access
    And group and other permissions should be denied

  @thread-safety @security
  Scenario: Concurrent encryption operations
    Given I have an EncryptedFileAppender
    And I have 20 threads logging concurrently
    When each thread logs 500 encrypted messages
    Then all messages should be encrypted correctly
    And no encryption operations should interfere with each other
    And the final encrypted file should be valid and complete

  @integration
  Scenario: Combining encrypted and standard appenders
    Given I configure both an EncryptedFileAppender and a ConsoleAppender
    When I log a message "Multi-appender test"
    Then the message should appear encrypted in the file
    And the message should appear in plaintext on the console
    And both appenders should operate independently

  @buffering @security
  Scenario: Encrypted appender buffering behavior
    Given I configure an EncryptedFileAppender with buffering enabled
    And the buffer size is 100 messages
    When I log 50 messages
    Then the messages should be held in buffer
    And the encrypted file should not yet contain all messages
    When I flush the appender
    Then all buffered messages should be encrypted and written to file

  @error-handling @security
  Scenario: Encryption failure handling
    Given I configure an EncryptedFileAppender
    And I simulate an encryption error (e.g., corrupted key)
    When I attempt to log a message
    Then the appender should handle the error gracefully
    And an error should be logged to stderr or error stream
    And the application should not crash

  @custom-appender
  Scenario: Custom appender registration
    Given I have implemented a CustomDatabaseAppender
    When I register the appender with log4cxx
    And I configure a logger to use the CustomDatabaseAppender
    Then log messages should be written to the database
    And the custom appender should integrate seamlessly with the framework

  @custom-appender
  Scenario: Custom appender with custom layout
    Given I have a custom JSON layout implementation
    And I configure it with a FileAppender
    When I log messages with various fields
    Then the log file should contain valid JSON format
    And each log entry should be a properly formatted JSON object

  @appender-lifecycle
  Scenario: Appender initialization and cleanup
    Given I configure an EncryptedFileAppender
    When the logging framework initializes
    Then the appender should be properly initialized
    And encryption resources should be allocated
    When the framework shuts down
    Then the appender should close all resources
    And encryption contexts should be properly cleaned up

  @negative
  Scenario: Invalid encryption configuration
    Given I configure an EncryptedFileAppender with invalid encryption parameters
    When I attempt to initialize the appender
    Then the initialization should fail with a clear error message
    And the error should indicate what configuration is invalid

  @compliance
  Scenario: Encrypted logs meet compliance requirements
    Given I configure an EncryptedFileAppender for GDPR compliance
    And I log personally identifiable information (PII)
    Then the PII should be encrypted at rest
    And the encryption should meet required standards (e.g., AES-256)
    And unauthorized access should not reveal plaintext PII

  @disaster-recovery
  Scenario: Encrypted log backup and restore
    Given I have encrypted logs from the past week
    When I back up the encrypted log files
    And I restore them on a different system
    And I provide the correct decryption key
    Then I should be able to decrypt and read all restored logs
    And no data should be lost in the backup/restore process

  @monitoring
  Scenario: Appender health monitoring
    Given I have multiple appenders configured
    When I query the health status of each appender
    Then I should receive status information for each appender
    And the status should indicate if the appender is functional
    And any errors or warnings should be reported

  @dynamic-configuration
  Scenario: Add appender at runtime
    Given the logging framework is running with default appenders
    When I programmatically add an EncryptedFileAppender at runtime
    Then the new appender should start receiving log messages
    And existing appenders should continue to work
    And no restart should be required

  @dynamic-configuration
  Scenario: Remove appender at runtime
    Given I have multiple appenders configured
    When I programmatically remove a specific appender at runtime
    Then that appender should stop receiving messages
    And the appender should be properly cleaned up
    And other appenders should be unaffected