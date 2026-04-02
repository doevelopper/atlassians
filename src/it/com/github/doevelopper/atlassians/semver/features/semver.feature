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


# features/version_comparison.feature
@version @comparison
Feature: Semantic Version Comparison and Precedence
  As a software doevelopper
  I want to compare semantic versions according to SemVer rules
  So that I can determine version precedence correctly

  Background:
    Given the SemVer 2.0.0 specification

  @precedence
  Scenario: Compare major versions
    Given a version "1.0.0"
    And a version "2.0.0"
    When I compare the versions
    Then "1.0.0" should be less than "2.0.0"

  @precedence
  Scenario: Compare minor versions
    Given a version "1.1.0"
    And a version "1.2.0"
    When I compare the versions
    Then "1.1.0" should be less than "1.2.0"

  @precedence
  Scenario: Compare patch versions
    Given a version "1.0.1"
    And a version "1.0.2"
    When I compare the versions
    Then "1.0.1" should be less than "1.0.2"

  @precedence
  Scenario: Prerelease has lower precedence than release
    Given a version "1.0.0-alpha"
    And a version "1.0.0"
    When I compare the versions
    Then "1.0.0-alpha" should be less than "1.0.0"

  @precedence @critical
  Scenario: Complete prerelease precedence chain from spec
    Given the following versions in order:
      | version          |
      | 1.0.0-alpha      |
      | 1.0.0-alpha.1    |
      | 1.0.0-alpha.beta |
      | 1.0.0-beta       |
      | 1.0.0-beta.2     |
      | 1.0.0-beta.11    |
      | 1.0.0-rc.1       |
      | 1.0.0            |
    Then each version should be less than the next

  @precedence
  Scenario: Numeric prerelease identifiers compared numerically
    Given a version "1.0.0-alpha.2"
    And a version "1.0.0-alpha.11"
    When I compare the versions
    Then "1.0.0-alpha.2" should be less than "1.0.0-alpha.11"

  @precedence
  Scenario: Alphanumeric identifiers compared lexically
    Given a version "1.0.0-alpha"
    And a version "1.0.0-beta"
    When I compare the versions
    Then "1.0.0-alpha" should be less than "1.0.0-beta"

  @precedence
  Scenario: Numeric identifier has lower precedence than alphanumeric
    Given a version "1.0.0-1"
    And a version "1.0.0-alpha"
    When I compare the versions
    Then "1.0.0-1" should be less than "1.0.0-alpha"

  @precedence
  Scenario: Larger prerelease identifier set has higher precedence
    Given a version "1.0.0-alpha"
    And a version "1.0.0-alpha.1"
    When I compare the versions
    Then "1.0.0-alpha" should be less than "1.0.0-alpha.1"

  @build_metadata
  Scenario: Build metadata ignored in precedence
    Given a version "1.0.0+build1"
    And a version "1.0.0+build2"
    When I compare the versions
    Then the versions should be equal

  @build_metadata
  Scenario: Build metadata ignored with prerelease
    Given a version "1.0.0-alpha+001"
    And a version "1.0.0-alpha+999"
    When I compare the versions
    Then the versions should be equal

  @equality
  Scenario Outline: Version equality
    Given a version "<version1>"
    And a version "<version2>"
    When I compare the versions
    Then the versions should be equal

    Examples:
        | version1 | version2 |
        | 1.2.3    | 1.2.3    |
        | 1.0.0    | 1.0.0    |
        | 0.0.0    | 0.0.0    |


# features/version_constraints.feature
@constraints
Feature: Version Constraints for Dependency Management
  As a package manager
  I want to evaluate version constraints
  So that I can determine compatible dependency versions

  Background:
    Given the SemVer 2.0.0 specification

  @exact
  Scenario Outline: Exact version constraint
    Given a constraint "<constraint>"
    When I check if version "<version>" satisfies the constraint
    Then the result should be <satisfied>

    Examples:
        | constraint | version | satisfied |
        | 1.2.3      | 1.2.3   | true      |
        | 1.2.3      | 1.2.4   | false     |
        | =1.2.3     | 1.2.3   | true      |
        | =1.2.3     | 1.2.2   | false     |

  @comparison
  Scenario Outline: Greater than constraint
    Given a constraint ">1.2.3"
    When I check if version "<version>" satisfies the constraint
    Then the result should be <satisfied>

    Examples:
        | version | satisfied |
        | 1.2.4   | true      |
        | 1.3.0   | true      |
        | 2.0.0   | true      |
        | 1.2.3   | false     |
        | 1.2.2   | false     |
        | 1.0.0   | false     |

  @comparison
  Scenario Outline: Greater or equal constraint
    Given a constraint ">=1.2.3"
    When I check if version "<version>" satisfies the constraint
    Then the result should be <satisfied>

    Examples:
        | version | satisfied |
        | 1.2.3   | true      |
        | 1.2.4   | true      |
        | 1.3.0   | true      |
        | 2.0.0   | true      |
        | 1.2.2   | false     |

  @comparison
  Scenario Outline: Less than constraint
    Given a constraint "<2.0.0"
    When I check if version "<version>" satisfies the constraint
    Then the result should be <satisfied>

    Examples:
        | version | satisfied |
        | 1.9.9   | true      |
        | 1.2.3   | true      |
        | 0.1.0   | true      |
        | 2.0.0   | false     |
        | 2.0.1   | false     |

  @caret
  Scenario Outline: Caret range constraint
    Given a constraint "^<base_version>"
    When I check if version "<test_version>" satisfies the constraint
    Then the result should be <satisfied>

    Examples:
        | base_version | test_version | satisfied |
        | 1.2.3        | 1.2.3        | true      |
        | 1.2.3        | 1.2.4        | true      |
        | 1.2.3        | 1.3.0        | true      |
        | 1.2.3        | 1.9.9        | true      |
        | 1.2.3        | 2.0.0        | false     |
        | 1.2.3        | 1.2.2        | false     |
        | 0.2.3        | 0.2.3        | true      |
        | 0.2.3        | 0.2.4        | true      |
        | 0.2.3        | 0.3.0        | false     |
        | 0.0.3        | 0.0.3        | true      |
        | 0.0.3        | 0.0.4        | false     |

  @tilde
  Scenario Outline: Tilde range constraint
    Given a constraint "~<base_version>"
    When I check if version "<test_version>" satisfies the constraint
    Then the result should be <satisfied>

    Examples:
        | base_version | test_version | satisfied |
        | 1.2.3        | 1.2.3        | true      |
        | 1.2.3        | 1.2.4        | true      |
        | 1.2.3        | 1.2.9        | true      |
        | 1.2.3        | 1.3.0        | false     |
        | 1.2.3        | 1.2.2        | false     |
        | 1.2          | 1.2.0        | true      |
        | 1.2          | 1.2.9        | true      |
        | 1.2          | 1.3.0        | false     |

  @wildcard
  Scenario Outline: Wildcard constraint
    Given a constraint "<constraint>"
    When I check if version "<version>" satisfies the constraint
    Then the result should be <satisfied>

    Examples:
        | constraint | version | satisfied |
        | 1.2.*      | 1.2.0   | true      |
        | 1.2.*      | 1.2.5   | true      |
        | 1.2.*      | 1.2.99  | true      |
        | 1.2.*      | 1.3.0   | false     |
        | 1.*        | 1.0.0   | true      |
        | 1.*        | 1.9.9   | true      |
        | 1.*        | 2.0.0   | false     |
        | *          | 0.0.1   | true      |
        | *          | 99.99.99| true      |

  @range
  Scenario Outline: Range constraint with AND logic
    Given a constraint ">=1.2.3 <2.0.0"
    When I check if version "<version>" satisfies the constraint
    Then the result should be <satisfied>

    Examples:
        | version | satisfied |
        | 1.2.3   | true      |
        | 1.5.0   | true      |
        | 1.9.9   | true      |
        | 2.0.0   | false     |
        | 1.2.2   | false     |

  @compound
  Scenario Outline: Compound constraint with OR logic
    Given a constraint ">=1.0.0 <1.5.0 || >=2.0.0 <3.0.0"
    When I check if version "<version>" satisfies the constraint
    Then the result should be <satisfied>

    Examples:
        | version | satisfied |
        | 1.0.0   | true      |
        | 1.4.9   | true      |
        | 1.5.0   | false     |
        | 1.9.9   | false     |
        | 2.0.0   | true      |
        | 2.5.0   | true      |
        | 3.0.0   | false     |

  @prerelease
  Scenario: Prerelease versions and constraints
    Given a constraint ">=1.0.0"
    When I check if version "1.0.0-alpha" satisfies the constraint
    Then the result should be false
    When I check if version "1.0.0" satisfies the constraint
    Then the result should be true


# features/version_operations.feature
@operations
Feature: Version Operations
  As a software doevelopper
  I want to perform operations on versions
  So that I can manage version increments and modifications

  Background:
    Given the SemVer 2.0.0 specification

  @increment
  Scenario: Increment major version
    Given a version "1.2.3"
    When I increment the major version
    Then the resulting version should be "2.0.0"

  @increment
  Scenario: Increment major version resets minor and patch
    Given a version "1.9.9"
    When I increment the major version
    Then the resulting version should be "2.0.0"

  @increment
  Scenario: Increment minor version
    Given a version "1.2.3"
    When I increment the minor version
    Then the resulting version should be "1.3.0"

  @increment
  Scenario: Increment minor version resets patch
    Given a version "1.2.9"
    When I increment the minor version
    Then the resulting version should be "1.3.0"

  @increment
  Scenario: Increment patch version
    Given a version "1.2.3"
    When I increment the patch version
    Then the resulting version should be "1.2.4"

  @increment
  Scenario: Increment removes prerelease and build metadata
    Given a version "1.2.3-alpha+build123"
    When I increment the patch version
    Then the resulting version should be "1.2.4"
    And there should be no prerelease identifier
    And there should be no build metadata

  @modification
  Scenario: Add prerelease identifier
    Given a version "1.2.3"
    When I add prerelease identifier "alpha.1"
    Then the resulting version should be "1.2.3-alpha.1"

  @modification
  Scenario: Add build metadata
    Given a version "1.2.3"
    When I add build metadata "20240101"
    Then the resulting version should be "1.2.3+20240101"

  @modification
  Scenario: Remove prerelease identifier
    Given a version "1.2.3-beta"
    When I remove the prerelease identifier
    Then the resulting version should be "1.2.3"

  @modification
  Scenario: Remove build metadata
    Given a version "1.2.3+build"
    When I remove the build metadata
    Then the resulting version should be "1.2.3"

  @immutability
  Scenario: Version operations create new instances
    Given a version "1.2.3"
    When I increment the major version
    Then the original version should remain "1.2.3"
    And a new version object should be created

  @queries
  Scenario Outline: Version property queries
    Given a version "<version>"
    When I query if it is a prerelease
    Then the result should be <is_prerelease>
    When I query if it is stable
    Then the result should be <is_stable>
    When I query if it is initial development
    Then the result should be <is_initial_dev>

    Examples:
        | version        | is_prerelease | is_stable | is_initial_dev |
        | 0.1.0          | false         | false     | true           |
        | 0.9.9          | false         | false     | true           |
        | 1.0.0          | false         | true      | false          |
        | 1.0.0-alpha    | true          | false     | false          |
        | 2.1.3          | false         | true      | false          |
        | 2.1.3-rc.1     | true          | false     | false          |

  @string_conversion
  Scenario Outline: Convert version to string
    Given a version created from "<input>"
    When I convert it to string
    Then the string should be "<output>"

    Examples:
        | input                        | output                       |
        | 1.2.3                        | 1.2.3                        |
        | 1.0.0-alpha                  | 1.0.0-alpha                  |
        | 1.0.0+build                  | 1.0.0+build                  |
        | 1.0.0-beta.1+exp.sha.5114f85 | 1.0.0-beta.1+exp.sha.5114f85 |


# features/version_validation.feature
@validation
Feature: Version Validation
  As a software system
  I want to validate version strings and components
  So that I can ensure data integrity

  Background:
    Given the SemVer 2.0.0 specification

  @validation
  Scenario Outline: Validate version components
    When I validate major=<major> minor=<minor> patch=<patch>
    Then the validation should <result>

    Examples:
        | major | minor | patch | result  |
        | 0     | 0     | 0     | succeed |
        | 1     | 2     | 3     | succeed |
        | 999   | 999   | 999   | succeed |
        | -1    | 0     | 0     | fail    |
        | 0     | -1    | 0     | fail    |
        | 0     | 0     | -1    | fail    |

  @validation
  Scenario Outline: Validate prerelease identifiers
    When I validate prerelease identifier "<identifier>"
    Then the validation should <result>

    Examples:
        | identifier  | result  |
        | alpha       | succeed |
        | alpha.1     | succeed |
        | 0.3.7       | succeed |
        | x-y-z       | succeed |
        | x.7.z.92    | succeed |
        |             | fail    |
        | 01          | fail    |
        | alpha.      | fail    |
        | .alpha      | fail    |
        | alpha..beta | fail    |

  @validation
  Scenario Outline: Validate build metadata
    When I validate build metadata "<metadata>"
    Then the validation should <result>

    Examples:
        | metadata            | result  |
        | 001                 | succeed |
        | 20130313144700      | succeed |
        | exp.sha.5114f85     | succeed |
        | 21AF26D3----117B344092BD | succeed |
        |                     | fail    |
        | build.              | fail    |
        | .build              | fail    |

  @regex
  Scenario Outline: Validate against official SemVer regex
    When I validate version string "<version>" against SemVer regex
    Then the validation should <result>

    Examples:
        | version                      | result  |
        | 1.2.3                        | succeed |
        | 1.0.0-alpha.beta             | succeed |
        | 1.0.0+build.123              | succeed |
        | 0.0.4                        | succeed |
        | 1.2.3-0.3.7                  | succeed |
        | 1.2.3-x.7.z.92               | succeed |
        | 1.0.0-alpha+001              | succeed |
        | 1.0.0+20130313144700         | succeed |
        | 1.0.0-beta+exp.sha.5114f85   | succeed |
        | v1.2.3                       | fail    |
        | 1.2                          | fail    |
        | 01.2.3                       | fail    |
        | 1.02.3                       | fail    |