# Feature: License Management System
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2024 ACME Technology Corp. All rights reserved.

@license @integration
Feature: License Management System
  As a software product owner
  I want to manage software licenses
  So that I can control access to my product features

  Background:
    Given a clean license manager instance
    And a temporary license storage directory

  # ============================================
  # License Creation Scenarios
  # ============================================

  @license-creation
  Scenario: Create a valid commercial license
    When I create a license with:
      | Property     | Value                |
      | licenseKey   | COMM-2024-001       |
      | productId    | PREMISSES-PRO        |
      | type         | Commercial           |
      | licenseeName | ACME Corporation     |
      | email        | license@acme.com     |
      | validDays    | 365                  |
    Then the license should be created successfully
    And the license type should be "Commercial"
    And the license should be valid

  @license-creation
  Scenario: Create a trial license with limited features
    When I create a trial license for product "PREMISSES-TRIAL" valid for 14 days
    Then the license should be created successfully
    And the license type should be "Trial"
    And the license should expire in approximately 14 days

  @license-creation
  Scenario: Create an enterprise license with multiple features
    When I create an enterprise license with features:
      | Feature            |
      | ADVANCED_ANALYTICS |
      | CUSTOM_REPORTS     |
      | API_ACCESS         |
      | MULTI_TENANCY      |
    Then the license should have 4 enabled features
    And the license should have feature "ADVANCED_ANALYTICS"
    And the license should have feature "MULTI_TENANCY"

  # ============================================
  # License Validation Scenarios
  # ============================================

  @license-validation
  Scenario: Validate a non-expired license
    Given I have a valid commercial license expiring in 30 days
    When I validate the license
    Then the validation should succeed
    And the validation code should be "Success"

  @license-validation
  Scenario: Validate an expired license
    Given I have an expired license
    When I validate the license
    Then the validation should fail
    And the validation code should be "Expired"

  @license-validation
  Scenario: Validate license with correct hardware binding
    Given I have a license bound to the current hardware
    When I validate the license
    Then the validation should succeed

  @license-validation
  Scenario: Validate license with incorrect hardware binding
    Given I have a license bound to hardware "DIFFERENT-HW-ID-12345"
    When I validate the license
    Then the validation should fail
    And the validation code should be "HardwareMismatch"

  # ============================================
  # Feature Validation Scenarios
  # ============================================

  @feature-validation
  Scenario: Check feature availability
    Given I have a license with features:
      | Feature   |
      | FEATURE_A |
      | FEATURE_B |
      | FEATURE_C |
    Then feature "FEATURE_A" should be enabled
    And feature "FEATURE_B" should be enabled
    And feature "FEATURE_D" should not be enabled

  @feature-validation
  Scenario: Validate required features
    Given I have a license with features:
      | Feature   |
      | FEATURE_A |
      | FEATURE_B |
    When I require features:
      | Feature   |
      | FEATURE_A |
      | FEATURE_B |
    Then the feature validation should succeed

  @feature-validation
  Scenario: Fail validation when required feature is missing
    Given I have a license with features:
      | Feature   |
      | FEATURE_A |
    When I require features:
      | Feature   |
      | FEATURE_A |
      | FEATURE_X |
    Then the feature validation should fail
    And the validation message should contain "FEATURE_X"

  # ============================================
  # License Storage Scenarios
  # ============================================

  @license-storage
  Scenario: Save and load a license
    Given I create a commercial license with key "STORAGE-TEST-001"
    When I save the license to storage
    And I load the license with key "STORAGE-TEST-001"
    Then the loaded license should match the original

  @license-storage
  Scenario: List all stored licenses
    Given I have stored the following licenses:
      | LicenseKey       | ProductId    | Type       |
      | LICENSE-001      | PRODUCT-A    | Commercial |
      | LICENSE-002      | PRODUCT-B    | Enterprise |
      | LICENSE-003      | PRODUCT-C    | Trial      |
    When I list all stored licenses
    Then I should get 3 license keys
    And the list should contain "LICENSE-001"
    And the list should contain "LICENSE-002"
    And the list should contain "LICENSE-003"

  @license-storage
  Scenario: Remove a license from storage
    Given I have a stored license with key "TO-DELETE-001"
    When I remove the license with key "TO-DELETE-001"
    Then the license "TO-DELETE-001" should not exist in storage

  @license-storage
  Scenario: Clear all licenses from storage
    Given I have stored the following licenses:
      | LicenseKey  | ProductId | Type       |
      | LICENSE-A   | PRODUCT   | Commercial |
      | LICENSE-B   | PRODUCT   | Commercial |
    When I clear all licenses from storage
    Then the storage should be empty

  # ============================================
  # License Manager Scenarios
  # ============================================

  @license-manager
  Scenario: Check if valid license exists
    Given no licenses are stored
    Then has valid license should return false
    When I store a valid commercial license
    Then has valid license should return true

  @license-manager
  Scenario: Get license status
    Given I have a valid commercial license expiring in 30 days
    When I get the license status
    Then the status should be "Valid"

  @license-manager
  Scenario: Get days until expiration
    Given I have a commercial license expiring in 45 days
    When I get the days until expiration
    Then the days should be approximately 45

  # ============================================
  # Observer Notification Scenarios
  # ============================================

  @observer
  Scenario: Observer receives license saved event
    Given I register a license observer
    When I save a new license
    Then the observer should receive a license event
    And the event type should be "LicenseSaved"

  @observer
  Scenario: Observer receives validation result
    Given I register a license observer
    When I validate a license
    Then the observer should receive a validation result notification

  # ============================================
  # Edge Case Scenarios
  # ============================================

  @edge-cases
  Scenario: Handle license with special characters in name
    When I create a license for licensee "John \"Dev\" O'Brien"
    Then the license should be created successfully
    And the licensee name should be "John \"Dev\" O'Brien"

  @edge-cases
  Scenario: Perpetual license never expires
    When I create a perpetual license without expiration date
    Then the license should not be expired
    And the days until expiration should be greater than 36500

  @edge-cases
  Scenario: Grace period for expired license
    Given I have a license expired 3 days ago
    And the validation allows a 7 day grace period
    When I validate the license
    Then the validation should succeed
