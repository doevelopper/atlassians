# syntax=docker/dockerfile:1
# hadolint global ignore=DL3006
# =============================================================================
# CI Test Image
# Extends ci-build with shell and C++ testing tools.
# Provides: BATS, Valgrind, gcovr, lcov, libfaketime, lizard.
#
# Build (from repository root):
#   docker build \
#     -f .docker/test/Dockerfile \
#     --build-arg BUILD_DATE=$(date -u +%Y-%m-%dT%H:%M:%SZ) \
#     --build-arg REVISION=$(git rev-parse HEAD) \
#     --build-arg BUILD_VERSION=$(cat VERSION) \
#     -t ci-test:edge .
# =============================================================================

ARG CI_BUILD_IMAGE=ci-build:edge
ARG BATS_CORE_VERSION=1.13.0
ARG BATS_SUPPORT_VERSION=0.3.0
ARG BATS_ASSERT_VERSION=2.2.4

# ---------------------------------------------------------------------------
# Download BATS framework tarballs (checksums verified from GitHub releases)
# ---------------------------------------------------------------------------
FROM scratch AS bats-downloader
ARG BATS_CORE_VERSION
ARG BATS_SUPPORT_VERSION
ARG BATS_ASSERT_VERSION

ADD --checksum=sha256:a122d4080a26c1da986bd0e7202b1630eb661a624915ef244f496fdd306e85fb \
    https://github.com/bats-core/bats-core/archive/refs/tags/v${BATS_CORE_VERSION}.tar.gz \
    /bats-core.tar.gz
ADD --checksum=sha256:7815237aafeb42ddcc1b8c698fc5808026d33317d8701d5ec2396e9634e2918f \
    https://github.com/bats-core/bats-support/archive/refs/tags/v${BATS_SUPPORT_VERSION}.tar.gz \
    /bats-support.tar.gz
ADD --checksum=sha256:e305df20c4c36cba4570e10f1cd824efb1ae71d88b5ef474550781ebee04192f \
    https://github.com/bats-core/bats-assert/archive/refs/tags/v${BATS_ASSERT_VERSION}.tar.gz \
    /bats-assert.tar.gz

# ---------------------------------------------------------------------------
# Extract BATS tarballs using the CI build image
# ---------------------------------------------------------------------------
FROM ${CI_BUILD_IMAGE} AS bats-extractor

USER root
SHELL ["/bin/bash", "-Eeuo", "pipefail", "-c"]

RUN --mount=from=bats-downloader,target=/dl \
    tar xzf /dl/bats-core.tar.gz    && mv bats-core-*    /bats-core \
 && tar xzf /dl/bats-support.tar.gz && mv bats-support-* /bats-support \
 && tar xzf /dl/bats-assert.tar.gz  && mv bats-assert-*  /bats-assert

# ---------------------------------------------------------------------------
# Final test image
# ---------------------------------------------------------------------------
FROM ${CI_BUILD_IMAGE}

ARG CI_USER=ci
ARG DEBIAN_FRONTEND=noninteractive
ARG BUILD_DATE
ARG REVISION
ARG BUILD_VERSION

LABEL \
  org.opencontainers.image.authors="Platform Engineering" \
  org.opencontainers.image.created="${BUILD_DATE}" \
  org.opencontainers.image.description="CI test image — C++ + BATS test toolchain" \
  org.opencontainers.image.licenses="Apache-2.0" \
  org.opencontainers.image.revision="${REVISION}" \
  org.opencontainers.image.source="https://github.com/doevelopper/atlassians" \
  org.opencontainers.image.title="ci-test" \
  org.opencontainers.image.url="https://github.com/doevelopper/atlassians" \
  org.opencontainers.image.vendor="Your Organization" \
  org.opencontainers.image.version="${BUILD_VERSION}"

HEALTHCHECK NONE

SHELL ["/bin/bash", "-Eeuo", "pipefail", "-c"]

USER root

# ---------------------------------------------------------------------------
# Install additional test-only packages
# ---------------------------------------------------------------------------
# hadolint ignore=DL3008
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update \
 && apt-get install -y --no-install-recommends \
      libfaketime \
      valgrind \
 && apt-get clean

# ---------------------------------------------------------------------------
# Install BATS (from pre-extracted tarballs)
# ---------------------------------------------------------------------------
RUN --mount=from=bats-extractor,source=/bats-core,target=/src/bats-core \
    --mount=from=bats-extractor,source=/bats-support,target=/src/bats-support \
    --mount=from=bats-extractor,source=/bats-assert,target=/src/bats-assert \
    bash /src/bats-core/install.sh /usr/local \
 && cp -r /src/bats-support /usr/local/bats-support \
 && cp -r /src/bats-assert  /usr/local/bats-assert

# ---------------------------------------------------------------------------
# Install Python-based testing / metrics tools
# lizard  — cyclomatic-complexity analyser
# cppcheck-junit — cppcheck XML → JUnit converter
# ---------------------------------------------------------------------------
# hadolint ignore=DL3013
RUN python3 -m pip install --break-system-packages --no-cache-dir \
      lizard \
      cppcheck-junit

USER ${CI_USER}
WORKDIR /workspace

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["bash"]
