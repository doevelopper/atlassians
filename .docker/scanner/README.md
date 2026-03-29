# CI Security Scanner Image (`.docker/scanner`)

Provides a curated set of **DevSecOps** scanning tools for CI pipelines.
Extends `ci-base` to keep the image small — no build toolchain.

## Provides

| Tool | Purpose |
|------|---------|
| Trivy | Container & filesystem vulnerability scanning |
| Grype | Vulnerability scanner (Anchore) |
| Syft | SBOM generation (Anchore) |
| Semgrep | SAST (uses community C/C++ ruleset) |
| hadolint | Dockerfile linter |
| shellcheck | Shell script static analyser |
| REUSE | SPDX / REUSE licence-compliance checker |

## Build

```bash
docker build \
  -f .docker/scanner/Dockerfile \
  --build-arg BUILD_DATE=$(date -u +%Y-%m-%dT%H:%M:%SZ) \
  --build-arg REVISION=$(git rev-parse HEAD) \
  --build-arg BUILD_VERSION=$(cat VERSION) \
  -t ci-scanner:edge .
```

## Checksum maintenance

When bumping `HADOLINT_VERSION`, `GRYPE_VERSION`, or `SYFT_VERSION`:
1. Download the new binary from the GitHub Release page.
2. Run `sha256sum <binary>` to get the digest.
3. Update `ADD --checksum=sha256:<digest>` in the Dockerfile.
4. Update `tool-versions.env`.
