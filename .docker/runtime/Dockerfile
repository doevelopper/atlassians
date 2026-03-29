# syntax=docker/dockerfile:1
# =============================================================================
# Runtime Image
# Minimal production image for deploying the compiled C++ artefact.
# Contains only the runtime shared libraries required by the application —
# no build tools, no compilers, no debug utilities.
#
# Typical usage (multi-stage build in your application Dockerfile):
#
#   FROM ci-build:edge AS builder
#   WORKDIR /workspace
#   RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
#
#   FROM ci-runtime:edge
#   COPY --from=builder /workspace/build/<binary> /app/<binary>
#   ENTRYPOINT ["/app/<binary>"]
#
# Build (from repository root):
#   docker build \
#     -f .docker/runtime/Dockerfile \
#     --build-arg BUILD_DATE=$(date -u +%Y-%m-%dT%H:%M:%SZ) \
#     --build-arg REVISION=$(git rev-parse HEAD) \
#     --build-arg BUILD_VERSION=$(cat VERSION) \
#     -t ci-runtime:edge .
# =============================================================================

FROM ubuntu:24.04@sha256:cd1dba651b3080c3686ecf4e3c4220f026b521fb76978881737d24f200828b2b

ARG DEBIAN_FRONTEND=noninteractive
ARG BUILD_DATE
ARG REVISION
ARG BUILD_VERSION
# Non-root user with a fixed UID/GID for predictable file-system permissions
ARG RUNTIME_USER=nonroot
ARG RUNTIME_UID=65532
ARG RUNTIME_GID=65532

LABEL \
  org.opencontainers.image.authors="Platform Engineering" \
  org.opencontainers.image.created="${BUILD_DATE}" \
  org.opencontainers.image.description="Minimal runtime image for C++ applications (Ubuntu 24.04 LTS)" \
  org.opencontainers.image.licenses="Apache-2.0" \
  org.opencontainers.image.revision="${REVISION}" \
  org.opencontainers.image.source="https://github.com/doevelopper/atlassians" \
  org.opencontainers.image.title="ci-runtime" \
  org.opencontainers.image.url="https://github.com/doevelopper/atlassians" \
  org.opencontainers.image.vendor="Your Organization" \
  org.opencontainers.image.version="${BUILD_VERSION}"

HEALTHCHECK NONE

SHELL ["/bin/bash", "-Eeuo", "pipefail", "-c"]

# ---------------------------------------------------------------------------
# Install only the runtime shared libraries the application depends on.
# Extend this list based on your binary's ldd output.
# ---------------------------------------------------------------------------
# hadolint ignore=DL3008
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update \
 && apt-get install -y --no-install-recommends \
      ca-certificates \
      libgcc-s1 \
      libgomp1 \
      libstdc++6 \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

# ---------------------------------------------------------------------------
# Install custom CA certificates (optional — same pattern as other images)
# ---------------------------------------------------------------------------
RUN --mount=type=bind,source=.docker/common/ca-certificates,target=/tmp/ca \
    bash /tmp/ca/install-ca.sh

# ---------------------------------------------------------------------------
# Create a dedicated non-root runtime user (high UID to avoid conflicts)
# ---------------------------------------------------------------------------
RUN groupadd --gid "${RUNTIME_GID}" "${RUNTIME_USER}" \
 && useradd \
      --uid "${RUNTIME_UID}" \
      --gid "${RUNTIME_GID}" \
      --no-create-home \
      --shell /sbin/nologin \
      --comment "Non-root runtime user" \
      "${RUNTIME_USER}"

# Application artefacts go here
WORKDIR /app

# Drop to non-root for all subsequent layers and at runtime
USER ${RUNTIME_USER}

# ---------------------------------------------------------------------------
# Copy compiled binary from a builder stage — customise for your project.
# Example:
#   COPY --from=builder --chown=${RUNTIME_USER}:${RUNTIME_USER} \
#        /workspace/build/my-app /app/my-app
#   ENTRYPOINT ["/app/my-app"]
# ---------------------------------------------------------------------------
