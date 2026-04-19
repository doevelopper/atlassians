Feature: Precision Time Protocol framework
  As an infrastructure engineer
  I want a PTP framework with BMCA and state transitions
  So that synchronized timing components can be composed safely

  Scenario: BMCA elects the best grandmaster
    Given a default PTP engine
    When I provide PTP clock candidates
    Then the selected grandmaster clock id should be "clock-best"

  Scenario: Engine enters listening state on startup
    Given a default PTP engine
    When I start the PTP engine
    Then the PTP engine state should be "Listening"

  Scenario: Engine becomes master after announce in listening state
    Given a started PTP engine
    When I receive an announce message
    Then the PTP engine state should be "Master"
