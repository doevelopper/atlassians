# CI

This directory contains the shared CI implementation consumed by both local commands and hosted CI platforms.

## Layout

- `lib/`:
	- `common.sh`: shared paths, build-system detection, report helpers
	- `logging.sh`: consistent timestamped logs
	- `git.sh`: git metadata helpers
	- `docker.sh`: optional container build helpers
	- `security.sh`: security tool/report helpers
- `pipelines/`:
	- `bootstrap.sh`
	- `lint.sh`
	- `build.sh`
	- `test.sh`
	- `security.sh`
	- `package.sh`
	- `sbom.sh`
	- `provenance.sh`
	- `release.sh`

## Execution model

All stage scripts are standalone Bash entrypoints and can be run directly from repository root:

```bash
bash ci/pipelines/bootstrap.sh
bash ci/pipelines/lint.sh
bash ci/pipelines/build.sh
bash ci/pipelines/test.sh
bash ci/pipelines/security.sh
bash ci/pipelines/package.sh
bash ci/pipelines/sbom.sh
bash ci/pipelines/provenance.sh
bash ci/pipelines/release.sh
```

`scripts/*.sh` and workflow YAML files call these same stage scripts to avoid logic drift.

## Artifacts

Stages write outputs under `artifacts/`:

- `artifacts/reports/*.report`: stage metadata and status
- `artifacts/test/junit.xml`: deterministic JUnit output
- `artifacts/build/*.tar.gz`: source package archives
- `artifacts/build/package.sha256`: package checksums
- `artifacts/sbom/*.json`: SBOM outputs
- `artifacts/provenance/provenance.json`: provenance metadata

## Scaffold-friendly behavior

The current repository may not yet include finalized build manifests or test harnesses. Stage scripts therefore:

- detect available build systems (`cmake`, `bazel`, or scaffold/no-op)
- keep strict behavior for required tooling
- gracefully skip optional checks while recording status in reports