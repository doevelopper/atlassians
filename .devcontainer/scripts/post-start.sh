#!/bin/env bash
# =============================================================================
# post-start.sh
# Executed each time the dev-container starts (after every rebuild or restart).
# Use it for tasks that need to run on every container startup.
# =============================================================================
set -euo pipefail

echo "post-start start"
echo "$(date +'%Y-%m-%d %H:%M:%S')    post-start start" >> "$HOME/status"

# ---------------------------------------------------------------------------
# Refresh the CA trust store in case new certificates were dropped in
# .docker/common/ca-certificates/ or /usr/local/share/ca-certificates/
# ---------------------------------------------------------------------------
if [[ "$(id -u)" -eq 0 ]]; then
  update-ca-certificates 2>/dev/null || true
fi

# ---------------------------------------------------------------------------
# Ensure the Conan profile exists and is up to date
# ---------------------------------------------------------------------------
if command -v conan &>/dev/null; then
  conan profile detect --exist-ok 2>/dev/null || true
fi

# ---------------------------------------------------------------------------
# Restore ccache stats summary so developers can track cache hit rates
# ---------------------------------------------------------------------------
if command -v ccache &>/dev/null; then
  echo "  ccache statistics:"
  ccache -s 2>/dev/null | grep -E "(hit|miss|cache size)" || true
fi

echo "$(date +'%Y-%m-%d %H:%M:%S')    post-start end" >> "$HOME/status"
echo "post-start end"
