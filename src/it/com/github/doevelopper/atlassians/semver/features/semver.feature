# features/version_parsing.feature
@version @parsing
Feature: Semantic Version Parsing
  As a software doevelopper
  I want to parse semantic version strings
  So that I can work with version information programmatically

  Background:
    Given the SemVer 2.0.0 specification

  @positive
  Scenario: Parse simple semantic version
    When I parse the version string "1.2.3"
    Then the parsing should succeed
    And the major version should be 1
    And the minor version should be 2
    And the patch version should be 3
    And there should be no prerelease identifier
    And there should be no build metadata

  @positive
  Scenario: Parse version with prerelease
    When I parse the version string "1.0.0-alpha"
    Then the parsing should succeed
    And the major version should be 1
    And the minor version should be 0
    And the patch version should be 0
    And the prerelease identifier should be "alpha"
    And there should be no build metadata

  @positive
  Scenario: Parse version with build metadata
    When I parse the version string "1.0.0+20130313144700"
    Then the parsing should succeed
    And the build metadata should be "20130313144700"
    And there should be no prerelease identifier

  @positive
  Scenario: Parse version with prerelease and build metadata
    When I parse the version string "1.0.0-beta+exp.sha.5114f85"
    Then the parsing should succeed
    And the prerelease identifier should be "beta"
    And the build metadata should be "exp.sha.5114f85"

  @positive
  Scenario Outline: Parse complex prerelease identifiers
    When I parse the version string "<version>"
    Then the parsing should succeed
    And the prerelease identifier should be "<prerelease>"

    Examples:
        | version                  | prerelease      |
        | 1.0.0-alpha.1            | alpha.1         |
        | 1.0.0-0.3.7              | 0.3.7           |
        | 1.0.0-x.7.z.92           | x.7.z.92        |
        | 1.0.0-x-y-z.--           | x-y-z.--        |
        | 2.0.0-rc.1+build.123     | rc.1            |

  @positive
  Scenario: Parse initial development version
    When I parse the version string "0.1.0"
    Then the parsing should succeed
    And the version should be marked as initial development

  @positive
  Scenario: Parse first stable version
    When I parse the version string "1.0.0"
    Then the parsing should succeed
    And the version should be marked as stable

  @negative
  Scenario Outline: Reject invalid version strings
    When I parse the version string "<invalid_version>"
    Then the parsing should fail
    And the error should indicate "<error_type>"

    Examples:
        | invalid_version | error_type           |
        | 1.2             | Invalid format       |
        | 1               | Invalid format       |
        | a.b.c           | Invalid format       |
        | 01.2.3          | Leading zero         |
        | 1.02.3          | Leading zero         |
        | 1.2.03          | Leading zero         |
        | -1.2.3          | Negative number      |
        | 1.-2.3          | Negative number      |
        | 1.2.-3          | Negative number      |
        | 1.2.3-          | Empty prerelease     |
        | 1.2.3+          | Empty build metadata |
        | 1.2.3-alpha.    | Empty identifier     |
        | 1.2.3-.alpha    | Empty identifier     |
        | 1.2.3-01        | Leading zero prerel  |

  @edge_cases
  Scenario Outline: Parse edge case versions
    When I parse the version string "<version>"
    Then the parsing should succeed

    Examples:
        | version                  |
        | 0.0.0                    |
        | 0.0.1                    |
        | 999999.999999.999999     |
        | 1.0.0-0                  |
        | 1.0.0-0.0.0              |

