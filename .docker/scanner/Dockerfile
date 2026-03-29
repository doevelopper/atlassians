# syntax=docker/dockerfile:1
# =============================================================================
# CI Security Scanner Image
# Provides a curated set of DevSecOps scanning tools for use in CI pipelines.
#
# Included tools:
#   trivy      — container / filesystem vulnerability scanner (Aqua Security)
#   grype      — vulnerability scanner (Anchore)
#   syft       — SBOM generator (Anchore)
#   semgrep    — SAST engine (r2c / opengrep ruleset)
#   hadolint   — Dockerfile linter
#   shellcheck — shell-script static analyser
#   reuse      — REUSE / SPDX license-compliance checker
#
# Build (from repository root):
#   docker build \
#     -f .docker/scanner/Dockerfile \
#     --build-arg BUILD_DATE=$(date -u +%Y-%m-%dT%H:%M:%SZ) \
#     --build-arg REVISION=$(git rev-parse HEAD) \
#     --build-arg BUILD_VERSION=$(cat VERSION) \
#     -t ci-scanner:edge .
# =============================================================================

ARG CI_BASE_IMAGE=ci-base:edge
ARG HADOLINT_VERSION=2.12.0
ARG SHELLCHECK_VERSION=0.10.0
ARG TRIVY_VERSION=0.58.1
ARG GRYPE_VERSION=0.88.0
ARG SYFT_VERSION=1.18.0

# ---------------------------------------------------------------------------
# Hadolint binary download (architecture-aware)
# TODO: update SHA256 checksums from the official GitHub release page when
#       bumping HADOLINT_VERSION: https://github.com/hadolint/hadolint/releases
# ---------------------------------------------------------------------------
FROM scratch AS hadolint-amd64
ARG HADOLINT_VERSION
ADD https://github.com/hadolint/hadolint/releases/download/v${HADOLINT_VERSION}/hadolint-Linux-x86_64 \
    /hadolint

FROM scratch AS hadolint-arm64
ARG HADOLINT_VERSION
ADD https://github.com/hadolint/hadolint/releases/download/v${HADOLINT_VERSION}/hadolint-Linux-arm64 \
    /hadolint

# trivy:ignore:AVD-DS-0001
FROM hadolint-${TARGETARCH} AS hadolint-downloader

# ---------------------------------------------------------------------------
# Grype / Syft binaries (Anchore official releases)
# TODO: pin SHA256 checksums from https://github.com/anchore/grype/releases
#       and https://github.com/anchore/syft/releases
# ---------------------------------------------------------------------------
FROM scratch AS anchore-amd64
ARG GRYPE_VERSION
ARG SYFT_VERSION
ADD https://github.com/anchore/grype/releases/download/v${GRYPE_VERSION}/grype_${GRYPE_VERSION}_linux_amd64.tar.gz \
    /grype.tar.gz
ADD https://github.com/anchore/syft/releases/download/v${SYFT_VERSION}/syft_${SYFT_VERSION}_linux_amd64.tar.gz \
    /syft.tar.gz

FROM scratch AS anchore-arm64
ARG GRYPE_VERSION
ARG SYFT_VERSION
ADD https://github.com/anchore/grype/releases/download/v${GRYPE_VERSION}/grype_${GRYPE_VERSION}_linux_arm64.tar.gz \
    /grype.tar.gz
ADD https://github.com/anchore/syft/releases/download/v${SYFT_VERSION}/syft_${SYFT_VERSION}_linux_arm64.tar.gz \
    /syft.tar.gz

# trivy:ignore:AVD-DS-0001
FROM anchore-${TARGETARCH} AS anchore-downloader

# ---------------------------------------------------------------------------
# Final scanner image
# ---------------------------------------------------------------------------
FROM ${CI_BASE_IMAGE}

ARG CI_USER=ci
ARG TRIVY_VERSION=0.58.1
ARG DEBIAN_FRONTEND=noninteractive
ARG BUILD_DATE
ARG REVISION
ARG BUILD_VERSION

LABEL \
  org.opencontainers.image.authors="Platform Engineering" \
  org.opencontainers.image.created="${BUILD_DATE}" \
  org.opencontainers.image.description="CI security scanner image (Trivy, Grype, Syft, Semgrep, hadolint, shellcheck, REUSE)" \
  org.opencontainers.image.licenses="Apache-2.0" \
  org.opencontainers.image.revision="${REVISION}" \
  org.opencontainers.image.source="https://github.com/doevelopper/atlassians" \
  org.opencontainers.image.title="ci-scanner" \
  org.opencontainers.image.url="https://github.com/doevelopper/atlassians" \
  org.opencontainers.image.vendor="Your Organization" \
  org.opencontainers.image.version="${BUILD_VERSION}"

HEALTHCHECK NONE

SHELL ["/bin/bash", "-Eeuo", "pipefail", "-c"]

USER root

# ---------------------------------------------------------------------------
# Add Trivy official apt repository and install Trivy + shellcheck
# ---------------------------------------------------------------------------
# hadolint ignore=DL3008
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    wget -qO - https://aquasecurity.github.io/trivy-repo/deb/public.key \
      | gpg --dearmor -o /usr/share/keyrings/trivy.gpg \
 && echo "deb [signed-by=/usr/share/keyrings/trivy.gpg] \
      https://aquasecurity.github.io/trivy-repo/deb generic main" \
      | tee /etc/apt/sources.list.d/trivy.list > /dev/null \
 && apt-get update \
 && apt-get install -y --no-install-recommends \
      "trivy=${TRIVY_VERSION}" \
      python3 \
      python3-pip \
      shellcheck \
 && apt-get clean

# ---------------------------------------------------------------------------
# Install hadolint binary
# ---------------------------------------------------------------------------
RUN --mount=from=hadolint-downloader,target=/dl \
    install -m 755 /dl/hadolint /usr/local/bin/hadolint

# ---------------------------------------------------------------------------
# Install grype and syft (SBOM / vulnerability scanning)
# ---------------------------------------------------------------------------
RUN --mount=from=anchore-downloader,target=/dl \
    tar xzf /dl/grype.tar.gz -C /usr/local/bin grype \
 && tar xzf /dl/syft.tar.gz  -C /usr/local/bin syft \
 && chmod +x /usr/local/bin/grype /usr/local/bin/syft

# ---------------------------------------------------------------------------
# Install Python-based scanners via pip
#   semgrep — SAST (supports C/C++ via community rules)
#   reuse   — REUSE SPDX license compliance
# ---------------------------------------------------------------------------
# hadolint ignore=DL3013
RUN python3 -m pip install --break-system-packages --no-cache-dir \
      semgrep \
      reuse

USER ${CI_USER}
WORKDIR /workspace
