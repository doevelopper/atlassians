# CI Build Image (`.docker/ci`)

Full **C++ build toolchain** for CI pipelines, extending `ci-base`.
Tool versions are pinned in `tool-versions.env` and kept in sync with
`.devcontainer/cpp/Dockerfile`.

## Provides

| Component | Version |
|-----------|---------|
| GCC / G++ | 14 |
| Clang / LLVM | 20 |
| CMake | via pip (see `tool-versions.env`) |
| Ninja | system apt |
| Conan | via pip |
| ccache | 4.13.1 (verified via minisign) |
| CPM.cmake | 0.42.1 |

## Build

```bash
# Build ci-base first, then:
docker build \
  -f .docker/ci/Dockerfile \
  --build-arg BUILD_DATE=$(date -u +%Y-%m-%dT%H:%M:%SZ) \
  --build-arg REVISION=$(git rev-parse HEAD) \
  --build-arg BUILD_VERSION=$(cat VERSION) \
  -t ci-build:edge .
```

## Files

| File | Purpose |
|------|---------|
| `Dockerfile` | Multi-stage image build |
| `tool-versions.env` | Pinned versions — single source of truth |
| `entrypoint.sh` | Sources `tool-versions.env` and runs the CI command |

## GitHub Actions example

```yaml
jobs:
  build:
    runs-on: ubuntu-latest
    container:
      image: ghcr.io/ORGANIZATION/REPOSITORY/ci-build:edge
    steps:
      - uses: actions/checkout@v4
      - run: cmake -B build && cmake --build build
```
