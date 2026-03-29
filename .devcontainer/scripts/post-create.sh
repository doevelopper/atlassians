#!/bin/env bash
# =============================================================================
# post-create.sh
# Executed ONCE when the dev-container is first created (before the user
# attaches).  Use it for one-time setup tasks that survive container rebuilds
# only if the workspace volume is preserved.
# =============================================================================
set -euo pipefail

echo "post-create start"
echo "$(date +'%Y-%m-%d %H:%M:%S')    post-create start" >> "$HOME/status"

# ---------------------------------------------------------------------------
# Git identity — read from environment variables set in devcontainer.json
# remoteEnv or from the user's global git configuration on the host.
# ---------------------------------------------------------------------------
if [[ -n "${GIT_AUTHOR_NAME:-}" ]]; then
  git config --global user.name  "${GIT_AUTHOR_NAME}"
fi
if [[ -n "${GIT_AUTHOR_EMAIL:-}" ]]; then
  git config --global user.email "${GIT_AUTHOR_EMAIL}"
fi

# Mark the mounted workspace as safe (avoids git "dubious ownership" warnings)
git config --global --add safe.directory /workspace 2>/dev/null || true

# ---------------------------------------------------------------------------
# GPG commit signing (optional)
# Set GPG_SIGNING_KEY_ID in remoteEnv to activate signed commits.
# ---------------------------------------------------------------------------
if [[ -n "${GPG_SIGNING_KEY_ID:-}" ]]; then
  git config --global user.signingkey "${GPG_SIGNING_KEY_ID}"
  git config --global commit.gpgsign  true
  echo "  GPG commit signing configured (key ${GPG_SIGNING_KEY_ID})."
fi

# ---------------------------------------------------------------------------
# Conan — detect/refresh the default profile
# ---------------------------------------------------------------------------
if command -v conan &>/dev/null; then
  conan profile detect --exist-ok 2>/dev/null || true
fi

# ---------------------------------------------------------------------------
# Python dev dependencies (pip)
# Re-install on every create to ensure the pinned requirement hashes are met.
# ---------------------------------------------------------------------------
REQUIREMENTS=/workspace/.devcontainer/cpp/requirements.txt
if [[ -f "${REQUIREMENTS}" ]]; then
  python3 -m pip install \
    --break-system-packages \
    --require-hashes \
    --no-cache-dir \
    -r "${REQUIREMENTS}" \
    --quiet
  echo "  Python dev dependencies installed."
fi

# ---------------------------------------------------------------------------
# Install pre-commit hooks if a config file is present
# ---------------------------------------------------------------------------
if [[ -f /workspace/.pre-commit-config.yaml ]] && command -v pre-commit &>/dev/null; then
  pre-commit install --install-hooks
  echo "  pre-commit hooks installed."
fi

echo "$(date +'%Y-%m-%d %H:%M:%S')    post-create end" >> "$HOME/status"
echo "post-create end"

