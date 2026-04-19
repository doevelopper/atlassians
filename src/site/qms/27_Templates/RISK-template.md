# RISK-\<PROJ\>-\<NNN\> — Risk Register and Mitigation Plan

| Field | Value |
|---|---|
| **Document ID** | RISK-\<PROJ\>-\<NNN\> |
| **Project** | \<Project Name\> |
| **Version** | 0.1 |
| **Status** | Draft \| Active \| Closed |
| **Owner** | |
| **Date** | YYYY-MM-DD |
| **Review cycle** | Sprint / Monthly / Quarterly |

---

## 1 Risk Scoring Matrix

**Likelihood:** 1 = Rare, 2 = Unlikely, 3 = Possible, 4 = Likely, 5 = Almost Certain  
**Impact:**     1 = Negligible, 2 = Minor, 3 = Moderate, 4 = Major, 5 = Catastrophic  
**Risk Score = Likelihood × Impact** (1–25; thresholds: Low ≤ 5, Medium 6–14, High 15–24, Critical = 25)

## 2 Risk Register

| ID | Category | Description | Likelihood | Impact | Score | Rating | Mitigation | Contingency | Owner | Status |
|---|---|---|---|---|---|---|---|---|---|---|
| RSK-001 | Technical | Dependency with no long-term support | 3 | 3 | 9 | Medium | Pin version; evaluate alternatives | Fork / vendor | @dev-lead | Open |
| RSK-002 | Security | Supply-chain compromise via third-party | 2 | 5 | 10 | Medium | SBOM, sigstore, Dependabot | Rollback + incident response | @sec-eng | Open |
| RSK-003 | Safety | Watchdog timer not triggered in failure path | 1 | 5 | 5 | Low | Fault injection testing | Safe-state hardware cutout | @safety-eng | Open |
| RSK-004 | Schedule | Key engineer unavailable | 3 | 3 | 9 | Medium | Cross-train, documentation | Contractor engagement | @pm | Open |
| RSK-005 | Regulatory | Compliance gap vs. target certification | 2 | 4 | 8 | Medium | Gap analysis, audit trail | Scope reduction | @qa-lead | Open |

_Add rows as new risks are identified. Close rows when risk is resolved or accepted._

## 3 Risk Review Log

| Date | Reviewer | Changes Made |
|---|---|---|
| YYYY-MM-DD | | Initial risk identification |

## 4 Residual Risk Acceptance

_After mitigations, document accepted residual risk and authorising stakeholder signature._

| Risk ID | Residual Score | Accepted by | Date |
|---|---|---|---|
| RSK-001 | 6 | | |

## 5 Revision History

| Version | Date | Author | Change |
|---|---|---|---|
| 0.1 | YYYY-MM-DD | | Initial draft |
