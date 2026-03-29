#!/bin/env bash
# =============================================================================
# post-attach.sh
# Executed each time a user opens (attaches to) the dev-container terminal.
# Use it for welcome messages, environment checks, and developer reminders.
# =============================================================================

echo "$(date +'%Y-%m-%d %H:%M:%S')    post-attach start" >> "$HOME/status"

# ---------------------------------------------------------------------------
# Welcome banner
# ---------------------------------------------------------------------------
cat <<EOF

  ┌─────────────────────────────────────────────────────────┐
  │  POM-Scaffolding  ·  C++ Dev Container  (Ubuntu 24.04)  │
  └─────────────────────────────────────────────────────────┘

  Environment
    CONTAINER_FLAVOR : ${CONTAINER_FLAVOR:-unknown}
    CMAKE_GENERATOR  : ${CMAKE_GENERATOR:-Ninja}
    CONAN_HOME       : ${CONAN_HOME:-/opt/conan}
    CCACHE_DIR       : ${CCACHE_DIR:-/cache/.ccache}
    CPM_SOURCE_CACHE : ${CPM_SOURCE_CACHE:-/cache/.cpm}

  Quick start
    cmake -B build -DCMAKE_BUILD_TYPE=Debug   # Configure
    cmake --build build                        # Build
    ctest --test-dir build -V                  # Run tests
    ccache -s                                  # Cache stats

  Useful scripts (from repo root)
    scripts/bootstrap.sh    — install / verify all toolchain deps
    scripts/doctor.sh       — sanity-check the local dev environment
    scripts/lint.sh         — run all linters
    scripts/test.sh         — run the full test suite

EOF

echo "$(date +'%Y-%m-%d %H:%M:%S')    post-attach end" >> "$HOME/status"

