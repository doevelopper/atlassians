# CI Base Image (`.docker/base`)

Minimal **Ubuntu 24.04 LTS** foundation shared by all CI pipeline images.

## Provides

| Component | Detail |
|-----------|--------|
| OS | Ubuntu 24.04 LTS (pinned digest) |
| Core tools | `git`, `curl`, `wget`, `jq`, `gnupg2`, `ca-certificates`, `openssh-client`, `unzip` |
| Non-root user | `ci` (UID/GID 1001) |
| Custom CAs | Drop `.pem` / `.crt` files in `.docker/common/ca-certificates/` |

## Build

```bash
# From repository root:
docker build \
  -f .docker/base/Dockerfile \
  --build-arg BUILD_DATE=$(date -u +%Y-%m-%dT%H:%M:%SZ) \
  --build-arg REVISION=$(git rev-parse HEAD) \
  --build-arg BUILD_VERSION=$(cat VERSION) \
  -t ci-base:edge .
```

## Image hierarchy

```
ubuntu:24.04
  └── ci-base          (.docker/base)    ← this image
        ├── ci-build   (.docker/ci)
        │     ├── ci-test    (.docker/test)
        │     └── <devcontainer extends ci-build>
        └── ci-scanner (.docker/scanner)
```

## Customisation

- **Packages** — edit `.docker/common/apt/packages.json`
- **CA certificates** — add `*.pem` / `*.crt` to `.docker/common/ca-certificates/`
- **Non-root user** — override `CI_USER`, `CI_UID`, `CI_GID` build args
