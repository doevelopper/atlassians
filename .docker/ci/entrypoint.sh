#!/usr/bin/env bash
# =============================================================================
# CI container entrypoint
# Runs inside the CI build container before the user-supplied command.
# - Sources tool-versions.env if present (enables $VAR expansion in scripts)
# - Optionally prints a brief environment summary when CI_DEBUG=true
# =============================================================================
set -euo pipefail

# Source pinned tool versions so they are available as env vars in CI scripts
TOOL_VERSIONS_FILE="/workspace/.docker/ci/tool-versions.env"
if [[ -f "${TOOL_VERSIONS_FILE}" ]]; then
  # Export only lines that look like KEY=VALUE (skip comments and blanks)
  while IFS='=' read -r key value; do
    [[ "${key}" =~ ^[A-Z_][A-Z0-9_]*$ ]] || continue
    export "${key}=${value}"
  done < <(grep -E '^[A-Z_][A-Z0-9_]*=' "${TOOL_VERSIONS_FILE}")
fi

# Debug summary — set CI_DEBUG=true in the CI environment to enable
if [[ "${CI_DEBUG:-false}" == "true" ]]; then
  echo "=== CI Container Environment ==="
  echo "  User         : $(id)"
  echo "  Hostname     : $(hostname)"
  echo "  Workspace    : ${WORKSPACE:-$(pwd)}"
  echo "  Git commit   : ${GITHUB_SHA:-${CI_COMMIT_SHA:-unknown}}"
  echo "  GCC          : $(gcc --version 2>/dev/null | head -1 || echo 'n/a')"
  echo "  Clang        : $(clang --version 2>/dev/null | head -1 || echo 'n/a')"
  echo "  CMake        : $(cmake --version 2>/dev/null | head -1 || echo 'n/a')"
  echo "  ccache       : $(ccache --version 2>/dev/null | head -1 || echo 'n/a')"
  echo "  Conan        : $(conan --version 2>/dev/null || echo 'n/a')"
  echo "================================"
fi

exec "$@"
