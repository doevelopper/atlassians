# Enterprise Delivery Operating Model

This document defines the operating model for a multi-team Bazel-based framework repository where contributors focus on code, tests, integration, and security work while CI/CD performs qualification, packaging, evidence generation, and release automatically.

It is intentionally aligned to the current repository implementation:

- Bazel is the build and test contract.
- GitHub Actions is the primary CI/CD platform.
- GitLab CI remains the secondary pipeline contract for portability.
- `delivery-pipeline.yml`, `nightly-assurance.yml`, `validate.yml`, and `automated-release.yml` are the current automation anchors.
- Release packaging already exists as Bazel `pkg_tar` outputs plus GitHub Release assets; the remaining work is to formalize distribution tiers, approvals, and dashboards.

## 1. Branching Model

### Recommended model

Use a constrained GitFlow-lite model now, with a path to trunk-based release trains later if `develop` becomes a bottleneck.

| Branch | Purpose | Lifetime | Source | Merge Target | Merge Strategy |
|--------|---------|----------|--------|--------------|----------------|
| `main` | Production-ready, releasable history | Permanent | Protected only | N/A | Merge commit only |
| `develop` | Integration branch for the next release train | Permanent | `main` bootstrap only | `release/*` or `main` via release promotion | Squash from work branches |
| `feature/<team>/<ticket>-<slug>` | New feature work | 1-5 days | `develop` | `develop` | Squash |
| `bugfix/<team>/<ticket>-<slug>` | Non-production defect work | 1-3 days | `develop` | `develop` | Squash |
| `release/<major>.<minor>` | Stabilization for a release train | 1-10 days | `develop` | `main` and back-merge to `develop` | Merge commit only |
| `hotfix/<ticket>-<slug>` | Urgent production repair | Same day to 48h | `main` | `main`, then back-merge to `develop` and active `release/*` | Merge commit only |
| `support/<major>.<minor>` | Maintenance line for supported old versions | Long-lived, limited count | `main` tag or prior support branch | Same branch + cherry-pick from hotfixes | Merge commit only |

### Naming rules

- Require ticket identifiers in all non-permanent branches: `feature/platform/ENG-241-bazel-remote-cache`.
- Reserve `release/<major>.<minor>` for stabilization only; no new features after the branch is cut.
- Limit active `support/*` branches to `N` and `N-1` unless regulation requires longer support.
- Delete merged `feature/*` and `bugfix/*` branches automatically.

### Protection rules

Apply GitHub branch protection and rulesets as code-backed governance.

| Branch set | Direct push | Reviews | CODEOWNERS | Required checks | Extra rules |
|------------|-------------|---------|------------|-----------------|-------------|
| `main` | Deny | 2 approvals | Required | Full PR gate, CodeQL, coverage, security, artifact policy | Signed tags only, linear approvals, dismiss stale reviews |
| `develop` | Deny | 1-2 approvals | Required on owned paths | Full PR gate except release-only jobs | Auto-delete branch after merge |
| `release/*` | Deny | 2 approvals incl. release owner | Required | Full PR gate plus release qualification and SBOM | Restrict changes to fixes, docs, versioning |
| `hotfix/*` | Deny | 2 approvals incl. release or platform owner | Required | Expedited full gate | Require incident/ticket link |
| `support/*` | Deny | 2 approvals | Required | Security, build, test, packaging | Cherry-pick only unless approved |

### Merge policy

- Use squash merges from `feature/*` and `bugfix/*` into `develop` to keep the integration branch readable.
- Use merge commits from `release/*`, `hotfix/*`, and `support/*` into protected branches to preserve auditability, rollback points, and promotion context.
- Disable rebase merges on protected branches to avoid rewriting the reviewed commit graph.
- Require PR titles to follow Conventional Commits. The existing PR governance job already checks this.

### Tagging policy

- Stable release tag: `v<major>.<minor>.<patch>`.
- Pre-release tag: `v<major>.<minor>.<patch>-rc.<n>`.
- Emergency patch tag: normal SemVer patch from the hotfix merge on `main`.
- Support branch patch tag: `v<major>.<minor>.<patch>` published from the matching `support/*` line only if that line is formally supported.
- Use annotated tags only; tags must point to commits reachable from `main` or an approved `support/*` branch.

### Mapping to CI triggers

| Event | Workflow intent | Required result |
|-------|-----------------|-----------------|
| Push to `feature/*`, `bugfix/*` | Fast feedback | Build, unit tests, diff quality, lightweight security |
| PR to `develop` | Integration gate | Full required checks before merge |
| Push to `develop` | Post-merge integration confidence | Full build, tests, coverage, SBOM, deeper analysis |
| Push to `release/*` | Stabilization | Same as `develop`, plus release-readiness checks |
| PR or push to `hotfix/*` | Emergency controlled repair | Same as release, shortened human path but not shortened gates |
| Tag `v*` | Production release | Publish signed immutable artifacts and provenance |
| Scheduled nightly | Deep assurance | Flake detection, dependency freshness, long-running scans |

## 2. Software Metrics and Observability

### Mandatory metrics

Track a small set of metrics that directly support merge decisions and release readiness.

| Metric | Initial target | Enforcement point | Tooling recommendation |
|--------|----------------|-------------------|------------------------|
| Unit coverage | `>= 80%` repository baseline, ratchet upward by component | PR gate on changed packages, release gate globally | `bazel coverage`, `llvm-cov`, JUnit/XML summary |
| BDD or integration pass rate | `100%` on protected branches | PR gate, release gate | Bazel test XML + cucumber reports |
| Coverage delta | No PR may reduce owned package coverage by more than 2 points without approval | PR comment + required check | custom script over `bazel coverage` reports |
| Cyclomatic complexity | Warn at `> 10`, fail at `> 15` for new/changed functions | PR gate for changed files | `lizard` |
| Duplication ratio | Warn at `> 3%`, fail at `> 5%` on changed scope | PR gate advisory, weekly hard gate review | `jscpd` |
| Dependency freshness | 0 critical stale dependencies on protected branches | PR and nightly | Dependabot + Renovate or Dependabot only + OSV scan |
| Build duration trend | No sustained regression `> 15%` over 14-day median | Nightly and post-merge dashboard | Bazel Build Event Protocol, BuildBuddy or BES consumer |
| Test flakiness | Any test with pass rate `< 98%` over rolling 30 runs is quarantined or fixed | Nightly and release-readiness gate | Bazel `--runs_per_test`, test history store |
| Security debt | 0 open critical findings on protected branches | PR and release gate | CodeQL, Semgrep, Trivy, Gitleaks |

### How to surface metrics

- PR checks: expose pass/fail outcomes as required GitHub checks.
- PR comments: publish one sticky summary with coverage delta, complexity hotspots, duplication findings, and affected dependency updates.
- Step summaries: keep human-readable run summaries in GitHub Actions for immediate triage.
- Historical dashboard: publish JSON snapshots to a static dashboard or external time-series store.
- Release evidence: archive the metric snapshot alongside the release assets and SBOM.

### Recommended implementation pattern

1. Generate machine-readable metric artifacts in every pipeline run.
2. Normalize them into a single `metrics.json` contract.
3. Upload `metrics.json` as a workflow artifact.
4. On `main`, append the snapshot to a long-lived dashboard store.
5. On nightly, compute trend regressions and open issues automatically when thresholds are breached.

### Repo-specific implementation notes

- Keep `COVERAGE_THRESHOLD=80` as the current baseline because it already exists in the delivery workflow, but ratchet it by package rather than freezing a repository-wide threshold forever.
- The delivery pipeline now emits a `metrics.json` contract plus a human-readable metrics summary artifact. Extend that job over time so it also collects:
  - coverage summary from `bazel coverage`
  - complexity from `lizard`
  - duplication from `jscpd`
  - flaky-test candidate list from rerun data
  - build timings from Bazel profile or BES
- Store weekly snapshots under generated release evidence or a dedicated metrics artifact, not in version-controlled source.

## 3. Release Artifact Packaging and Distribution

### Current state in this repository

This repository already has the foundation of a release pipeline:

- Bazel `pkg_tar` targets for production, test, and integration bundles.
- GitHub Release publication with checksums.
- SPDX and CycloneDX SBOM generation.
- Build provenance attestation via `actions/attest-build-provenance`.
- A machine-readable `release-manifest.json` attached to release assets.

The remaining enterprise-grade gap is distribution standardization, OCI publication, and external registry promotion policy.

### Recommended artifact catalog

| Artifact class | Format | Source of truth | Target registry |
|----------------|--------|-----------------|-----------------|
| Source distribution | `.tar.gz` | `git archive` at release tag | GitHub Release, Artifactory generic repo |
| Runtime binaries | `.tar.gz` from Bazel `pkg_tar` | `//src/main/cpp:...main.package` | GitHub Release, Artifactory generic repo |
| Test and assessment bundle | `.tar.gz` | Bazel `pkg_tar` outputs from test and IT packages | Internal QA registry or GitHub Release |
| OCI runtime image | OCI image digest | Bazel `rules_oci` target | GHCR first, Artifactory/Harbor optional mirror |
| SBOM | SPDX JSON and CycloneDX JSON | Release pipeline | Attached to release and registry metadata |
| Provenance and checksums | in-toto/SLSA attestation + `SHA256SUMS.txt` | Release pipeline | Same location as artifacts |

### Bazel packaging guidance

- Keep `rules_pkg` for tarball generation.
- Add `rules_oci` for containerized runtime deliverables, using Bazel outputs as the only packaging input.
- Do not rebuild artifacts during promotion. Promote by immutable digest or uploaded artifact identifier only.
- Use stamping only for version metadata, never for uncontrolled environmental data.
- Add a single release macro that produces a predictable output contract across languages.

### Publishing flow

1. Build artifacts once from the tagged commit.
2. Generate checksums, SBOMs, and provenance from those exact outputs.
3. Publish release assets to GitHub Release for transparency.
4. Mirror the same immutable assets to the enterprise artifact manager.
5. Publish OCI images by digest to GHCR and optionally mirror to GitLab Container Registry or Artifactory.
6. Record the published URIs and digests in a release manifest artifact.

### Signing and provenance

- Use GitHub OIDC keyless signing with Cosign for OCI images and detached artifacts.
- Keep `actions/attest-build-provenance` for non-container artifacts.
- Attach SBOMs per release and, if OCI images are added, as OCI referrers.
- Target SLSA Level 3 semantics where the hosted runner model and provenance chain allow it.

### Immediate repo actions

- Finish external registry publication by configuring the optional Artifactory mirror or an equivalent enterprise artifact repository.
- Add an OCI packaging target for the primary runtime path using `rules_oci`.
- Add retention and immutability rules in the target registries.

## 4. DevSecOps and Supply Chain Security

### Required controls

| Control | Tooling | Branch enforcement |
|---------|---------|--------------------|
| SAST | CodeQL + Semgrep | Fail on `main`, `develop`, `release/*`, `hotfix/*`; warn on work branches only for low severity |
| SCA | Trivy FS, OSV-Scanner, dependency review | Fail on critical/high on protected branches |
| Secret detection | Gitleaks | Fail everywhere except optional local pre-commit advisory mode |
| SBOM | Anchore SBOM action or Syft | Required on `main`, release tags, nightly |
| Container scan | Trivy image scan | Required for OCI release artifacts |
| License policy | SPDX/CycloneDX analysis + allow/deny list | Fail on protected branches |
| Provenance | GitHub attestations + Cosign | Required on release tags |

### Enforcement policy by branch type

| Branch/event | Vulnerabilities | Static analysis | Secrets | License | Provenance |
|--------------|-----------------|-----------------|---------|---------|------------|
| `feature/*`, `bugfix/*` | Warn on medium, fail on critical | Warn/fail only on changed severe findings | Fail | Warn | Not required |
| PR to protected branch | Fail on high/critical | Fail on new severe findings | Fail | Fail on deny-listed licenses | Not required |
| `develop` | Fail on high/critical | Fail | Fail | Fail | Not required |
| `release/*`, `hotfix/*` | Fail on medium/high/critical unless explicitly waived | Fail | Fail | Fail | Required before release cut |
| Release tag | Zero unresolved critical/high; signed waiver for any exception | Fail | Fail | Fail | Required |

### Security governance rules

- Treat waivers as time-bounded records with approver, expiry date, and linked issue.
- Store security scan outputs as immutable evidence artifacts.
- Pin third-party GitHub Actions to commit SHAs for protected-branch workflows.
- Prefer OIDC federation over long-lived registry credentials.
- Use isolated or ephemeral runners for release, signing, and production deployment jobs.

## 5. Developer Experience and Onboarding

### One-day productivity standard

A new engineer should be able to clone the repository, start the dev environment, run the same Bazel commands as CI, and open one PR in a single working day.

### Required onboarding building blocks

| Area | Standard |
|------|----------|
| Environment bootstrap | Devcontainer plus Bazel hermetic toolchain and `direnv` support |
| Local validation | Single `make verify` or `bazel test //...` entry point aligned with CI |
| Hooks | Pre-commit hooks for format, lint, secret detection, and changed-file tests |
| IDE integration | VS Code recommended extensions, Bazel support, C++ tooling, YAML, Markdown, GitHub Actions |
| Docs as code | Markdown templates, ADR template, runbook template, release checklist |
| Inner loop speed | Remote cache defaults, incremental test targets, changed-file validation |

### Repo-specific actions

- Keep the existing pre-commit hook, but add Gitleaks and a fast changed-target Bazel test step.
- Add a checked-in devcontainer if not already present, with Bazelisk, clang tooling, Java, Node, Python, and GitHub CLI.
- Add an onboarding guide under `src/site/docs/onboarding/` with:
  - workstation prerequisites
  - `direnv allow`
  - Bazel environment setup
  - common targets
  - PR workflow and branch naming
  - how to read CI failures
- Publish a `docs-as-code` contribution standard covering README quality, ADRs, and runbooks.

## 6. CI Pipeline Architecture and Governance

### Workflow topology

| Trigger | Workflow scope | Non-negotiable jobs |
|--------|----------------|---------------------|
| PR | Fast plus decisive merge gate | build, unit/integration tests, coverage, dependency review, SAST, SCA, secrets, policy checks |
| Push to `develop` | Post-merge integration | full build, tests, SBOM, packaging smoke, trend snapshot |
| Push to `main` | Production confidence | full build, tests, security, packaging verification, evidence publication |
| Release tag | Publish once | artifact build, signing, SBOM, provenance, release publication, environment approval |
| Nightly | Deep assurance | flaky-test analysis, dependency drift, long-running sanitizers, heavy scans |
| Manual dispatch | Controlled operations only | backfill, re-run reporting, emergency promotion, audit export |

### Pipeline contract design

- Keep pipeline stages tool-agnostic at the contract level: `quality`, `build`, `test`, `security`, `package`, `publish`, `deploy`, `evidence`.
- Make Bazel the implementation detail for build and test, not the orchestration vocabulary.
- Mirror the same stage names and artifacts in GitLab CI so portability remains procedural, not conceptual.

### Performance and runner guidance

- Enable remote cache for all CI jobs; use remote execution selectively for heavy C++ builds if cost justified.
- Export Bazel Build Event Protocol data for build time, cache hit rate, and critical path analysis.
- Prefer many small parallel jobs over one long workflow for clearer failures and faster retries.
- Use job-level permissions with least privilege.
- Separate untrusted PR execution from trusted release and deployment execution.

### Governance and auditability

- Protect environments in GitHub Actions with required reviewers for staging and production.
- Use OIDC to cloud or artifact backends instead of static secrets.
- Archive release evidence, SBOMs, attestations, and deployment manifests for the required retention period.
- Log every manual approval, rerun, and promotion action.
- Enforce reusable workflow standards for shared logic instead of cloning YAML across workflows.

## 7. Release Governance and Versioning

### Versioning strategy

Use SemVer for framework artifacts.

- `MAJOR`: breaking API or contract changes.
- `MINOR`: backward-compatible feature additions.
- `PATCH`: backward-compatible fixes.
- `-rc.N`: release candidate for staged validation.

### Change log and release notes

- Keep Conventional Commits as the merge title contract.
- Generate release notes automatically from merge history or labels.
- Prefer `release-please` or a thin custom generator if multi-language packaging metadata must stay Bazel-centric.
- Require a human-readable upgrade note section when a PR contains breaking changes.

### Approval workflow

| Stage | Approval model |
|-------|----------------|
| PR merge to `develop` | Standard CODEOWNER review |
| Release branch cut | Release manager or platform owner approval |
| Tag publication | Automated from approved branch state |
| Staging promotion | Automatic after tag, environment-protected |
| Production promotion | Required reviewers in GitHub environment plus successful verification jobs |

### Rollback procedure

1. Redeploy the previous signed artifact or OCI digest. Never rebuild during rollback.
2. Create `hotfix/*` only if the rollback is insufficient and a forward fix is required.
3. Publish a corrective patch release from `main`.
4. Back-merge or cherry-pick to `develop`, active `release/*`, and any supported `support/*` branches.
5. Attach rollback evidence and incident linkage to the release record.

## Recommended 90-Day Roadmap

### Phase 1: Formalize governance

- Convert the branch policy in this document into GitHub rulesets.
- Add required environment approvals for staging and production.
- Add metric artifact generation and a single sticky PR summary.

### Phase 2: Close packaging and distribution gaps

- Publish tarball assets to the enterprise artifact manager.
- Add OCI packaging and signing.
- Emit a signed release manifest.

### Phase 3: Add trend visibility

- Persist metrics and flaky-test history outside ephemeral workflow logs.
- Add dashboards for coverage, build latency, cache hit rate, and vulnerability trend.
- Automatically open issues for breached SLOs.

### Phase 4: Prepare GitLab portability

- Keep the same stage contract and artifact names in `.gitlab-ci.yml`.
- Extract shared scripts into repository-owned shell or Bazel targets.
- Avoid GitHub-only release logic except where the platform provides the attestation primitive.

## Definition of Done for Enterprise Readiness

This repository is operating at the intended model when all of the following are true:

- every protected branch is governed by enforced rulesets rather than tribal knowledge
- every PR receives automated quality, security, and ownership checks
- every release artifact is immutable, signed, attestable, and reproducible from a tag
- every release record contains SBOM, checksums, provenance, test evidence, and rollback reference
- every critical engineering metric is visible as both an immediate gate and a historical trend
- every contributor can bootstrap the environment and pass local verification in less than one working day