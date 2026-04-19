Feature: Acceptance test plan (TP)

  This folder is the top-level acceptance test plan.
  It is intentionally smaller and more business-focused than the demo module suite in:
    src/it/com/github/doevelopper/premisses/demo

  The goal is to provide a stable, end-to-end-ish set of scenarios that:
  - validates the “happy path” (smoke)
  - validates key error handling (non-regression)
  - validates persistence of observable behaviour (history / outputs)

  Scenarios are split by intent:
  - smoke.feature
  - regression.feature
