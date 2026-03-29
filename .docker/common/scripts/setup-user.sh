#!/usr/bin/env bash
# =============================================================================
# setup-user.sh
# Creates a non-root CI user with a configurable UID/GID.
# Bind-mount this file into a Dockerfile RUN layer at build time.
#
# Environment variables (all optional):
#   CI_USER  – username  (default: ci)
#   CI_UID   – user UID  (default: 1001)
#   CI_GID   – group GID (default: 1001)
# =============================================================================
set -euo pipefail

: "${CI_USER:=ci}"
: "${CI_UID:=1001}"
: "${CI_GID:=1001}"

echo "Creating CI user '${CI_USER}' (${CI_UID}:${CI_GID}) …"

# Create group if it doesn't already exist
if ! getent group "${CI_GID}" >/dev/null 2>&1; then
  groupadd --gid "${CI_GID}" "${CI_USER}"
fi

# Create user if it doesn't already exist
if ! id -u "${CI_USER}" >/dev/null 2>&1; then
  useradd \
    --uid "${CI_UID}" \
    --gid "${CI_GID}" \
    --home "/home/${CI_USER}" \
    --create-home \
    --shell /bin/bash \
    --comment "CI runner" \
    "${CI_USER}"
fi

# Restrict home directory to owner only
chmod 750 "/home/${CI_USER}"

echo "Done — CI user '${CI_USER}' is ready."
