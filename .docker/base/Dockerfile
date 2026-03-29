# syntax=docker/dockerfile:1
# =============================================================================
# CI Base Image
# Minimal Ubuntu 24.04 LTS foundation shared by all CI pipeline images.
# Provides: core OS tools + non-root CI user.
#
# Build (from repository root):
#   docker build \
#     -f .docker/base/Dockerfile \
#     --build-arg BUILD_DATE=$(date -u +%Y-%m-%dT%H:%M:%SZ) \
#     --build-arg REVISION=$(git rev-parse HEAD) \
#     --build-arg BUILD_VERSION=$(cat VERSION) \
#     -t ci-base:edge .
# =============================================================================

FROM ubuntu:24.04@sha256:cd1dba651b3080c3686ecf4e3c4220f026b521fb76978881737d24f200828b2b

ARG DEBIAN_FRONTEND=noninteractive
ARG BUILD_DATE
ARG REVISION
ARG BUILD_VERSION
ARG CI_USER=ci
ARG CI_UID=1001
ARG CI_GID=1001

# OCI Image Annotations (https://github.com/opencontainers/image-spec/blob/main/annotations.md)
LABEL \
  org.opencontainers.image.authors="Platform Engineering" \
  org.opencontainers.image.created="${BUILD_DATE}" \
  org.opencontainers.image.description="Minimal CI base image — Ubuntu 24.04 LTS" \
  org.opencontainers.image.licenses="Apache-2.0" \
  org.opencontainers.image.revision="${REVISION}" \
  org.opencontainers.image.source="https://github.com/doevelopper/atlassians" \
  org.opencontainers.image.title="ci-base" \
  org.opencontainers.image.url="https://github.com/doevelopper/atlassians" \
  org.opencontainers.image.vendor="Your Organization" \
  org.opencontainers.image.version="${BUILD_VERSION}"

HEALTHCHECK NONE

SHELL ["/bin/bash", "-Eeuo", "pipefail", "-c"]

# ---------------------------------------------------------------------------
# Install core OS packages
# Package list is kept in .docker/common/apt/packages.json and bind-mounted
# at build time so it can be updated without modifying this Dockerfile.
# ---------------------------------------------------------------------------
# hadolint ignore=DL3008
RUN --mount=type=bind,source=.docker/common/apt/packages.json,target=/tmp/packages.json \
    --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update \
 && apt-get install -y --no-install-recommends jq \
 && jq -r '.[]' /tmp/packages.json \
      | xargs apt-get install -y --no-install-recommends \
 && apt-get clean

# ---------------------------------------------------------------------------
# Install custom CA certificates (drop PEM/CRT files in
# .docker/common/ca-certificates/ before building to activate).
# ---------------------------------------------------------------------------
RUN --mount=type=bind,source=.docker/common/ca-certificates,target=/tmp/ca \
    bash /tmp/ca/install-ca.sh

# ---------------------------------------------------------------------------
# Create non-root CI user (UID/GID 1001 by default)
# ---------------------------------------------------------------------------
RUN --mount=type=bind,source=.docker/common/scripts/setup-user.sh,target=/tmp/setup-user.sh \
    CI_USER="${CI_USER}" CI_UID="${CI_UID}" CI_GID="${CI_GID}" \
    bash /tmp/setup-user.sh

ENV HOME="/home/${CI_USER}" \
    PATH="/home/${CI_USER}/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

USER ${CI_USER}
WORKDIR /workspace
