# GitHub Actions DevSecOps CI/CD Blueprint

This blueprint operationalizes branch-based CI/CD controls for this repository using GitHub Actions, Bazel, and security scanning.

For the full enterprise operating model covering branching governance, metrics, release distribution, onboarding, and versioning, see [Enterprise Delivery Operating Model](enterprise-delivery-operating-model.md).

## Branching Model to Pipeline Stages

| Branch Type | Trigger | Goal | Required Gates |
|-------------|---------|------|----------------|
| feature/* | push | fast feedback | Context, Format and Lint (Diff), Build and Test, Semgrep SAST, Trivy SCA, Secret Scan |
| bugfix/* | push | fast feedback + regression confidence | Context, Format and Lint (Diff), Build and Test, Semgrep SAST, Trivy SCA, Secret Scan |
| develop | push | integration confidence | Context, Format and Lint (Diff), Build and Test, ASan and UBSan, Coverage, Semgrep SAST, Trivy SCA, Secret Scan |
| pull_request to main/develop/release/hotfix | pull_request | merge gate | Context, Format and Lint (Diff), Build and Test, ASan and UBSan, Coverage, Dependency Review, Semgrep SAST, Trivy SCA, Secret Scan, CodeQL |
| main | push | production confidence | Context, Build and Test, ASan and UBSan, Coverage, Semgrep SAST, Trivy SCA, Secret Scan, CodeQL |
| release/* | push | release hardening | Context, Build and Test, ASan and UBSan, Semgrep SAST, Trivy SCA, Secret Scan, CodeQL |
| hotfix/* | push | emergency patch with guardrails | Context, Format and Lint (Diff), Build and Test, ASan and UBSan, Semgrep SAST, Trivy SCA, Secret Scan, CodeQL |
| nightly (cron) | schedule | deep assurance | Full Static Analysis, TSan, MSan (best-effort), Valgrind Memcheck, Deep Security, SBOM, DAST Preview (if STAGING_BASE_URL configured) |

## Workflows Added

- .github/workflows/delivery-pipeline.yml
- .github/workflows/nightly-assurance.yml

## Security and Supply Chain Controls

- SAST: Semgrep plus CodeQL (C/C++)
- Secrets: Gitleaks
- SCA and vulnerabilities: Trivy FS scan, dependency review on PR, OSV in nightly
- Supply chain posture: OSSF Scorecard nightly
- Artifact transparency: nightly SPDX SBOM artifact
- DAST: OWASP ZAP baseline enabled when STAGING_BASE_URL secret is configured

## Required Branch Protection Checks

Configure branch protection rules in GitHub for main, release/*, and hotfix/* with these required checks:

- Delivery Pipeline / Context
- Delivery Pipeline / Build and Test
- Delivery Pipeline / ASan and UBSan
- Delivery Pipeline / Semgrep SAST
- Delivery Pipeline / Trivy SCA
- Delivery Pipeline / Secret Scan
- Delivery Pipeline / CodeQL

For pull requests, also require:

- Delivery Pipeline / Format and Lint (Diff)
- Delivery Pipeline / Coverage
- Delivery Pipeline / Dependency Review

## Mapping to Your Draft Matrix

- clang-format: enforced in diff gate and nightly full scan
- cpplint: enforced in diff gate and nightly full scan
- clang-tidy, cppcheck, IWYU: modeled as nightly deep static analysis and diff advisory path
- compiler warnings with -Wall -Wextra -Werror: enforced in Build and Test
- ASan + UBSan: enforced for PR, develop, main, release, hotfix
- TSan/MSan/Valgrind: nightly-focused due runtime cost
- Semgrep, secret scan, dependency review, Trivy: integrated into delivery pipeline
- DAST: nightly/staging-preview mode with secret-gated target URL

## Repository-specific note

The current sanitizer bazelrc references a missing toolchain label (//tools/lrte:toolchain). The implemented workflows use direct sanitizer compiler and linker flags to avoid hard dependency on that missing label.

## Next Hardening Steps

1. Add a dedicated sanitizer-capable toolchain package to enable strict bazel --config asan/tsan/msan workflows.
2. Publish SARIF from Semgrep/cppcheck for first-class code scanning annotations.
3. Add deployment environments with required reviewers and protected secrets for staging and production promotion.
4. Add signed provenance attestations (SLSA level uplift) for release artifacts.
