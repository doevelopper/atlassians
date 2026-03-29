# atlassians

Project scaffolding with production-grade CI/CD and local developer infrastructure.

## Quick Start

1. Run local environment diagnostics:
	```bash
	scripts/doctor.sh
	```
2. Execute the same CI pipeline stages used in automation:
	```bash
	make ci
	```
3. Run security checks:
	```bash
	make security
	```

All generated outputs are placed under `artifacts/`.

## Local Development Infrastructure

- Dev Container definitions live in `.devcontainer/`
- CI/runtime/scanner image sources live in `.docker/`
- Shared local and CI stage scripts live in `ci/pipelines/`
- Local command wrappers live in `scripts/`
- Orchestration targets are in `Makefile` and `tasks/*.mk`

## CI Infrastructure

- GitHub CI pipeline: `.github/workflows/ci.yml`
- GitHub security pipeline: `.github/workflows/security.yml`
- GitHub release pipeline: `.github/workflows/release.yml`
- GitLab parity pipeline: `.gitlab-ci.yml`

Each workflow calls the same shell stages in `ci/pipelines/` to keep behavior identical across local runs and CI providers.

## Standard Stage Commands

- Bootstrap: `bash ci/pipelines/bootstrap.sh`
- Lint: `bash ci/pipelines/lint.sh`
- Build: `bash ci/pipelines/build.sh`
- Test: `bash ci/pipelines/test.sh`
- Security: `bash ci/pipelines/security.sh`
- Package: `bash ci/pipelines/package.sh`
- SBOM: `bash ci/pipelines/sbom.sh`
- Provenance: `bash ci/pipelines/provenance.sh`
- Release: `bash ci/pipelines/release.sh`

## Scaffold Behavior

This repository is still evolving with placeholder modules. Pipeline scripts are intentionally scaffold-aware:

- If no build manifest is present, build/test stages skip heavy execution and still produce consistent reports.
- Security and lint stages use optional tools when available and record skipped checks when tooling is absent.
- Package, SBOM, and provenance stages always generate deterministic outputs for traceability.

This lets CI stay green and useful during early structure-first development.
