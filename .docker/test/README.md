# CI Test Image (`.docker/test`)

Extends `ci-build` with testing and coverage tooling.

## Provides

| Component | Detail |
|-----------|--------|
| BATS | Shell testing framework (bats-core, bats-support, bats-assert) |
| Valgrind | Memory error detector |
| gcovr / lcov | C++ coverage report generators |
| lizard | Cyclomatic complexity analyser |
| cppcheck-junit | cppcheck XML → JUnit converter |
| libfaketime | Control system time in tests |

## Build

```bash
# Requires ci-build:edge to be available locally or in a registry.
docker build \
  -f .docker/test/Dockerfile \
  --build-arg BUILD_DATE=$(date -u +%Y-%m-%dT%H:%M:%SZ) \
  --build-arg REVISION=$(git rev-parse HEAD) \
  --build-arg BUILD_VERSION=$(cat VERSION) \
  -t ci-test:edge .
```
