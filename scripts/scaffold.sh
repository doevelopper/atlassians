#!/usr/bin/env bash
# scripts/scaffold.sh — Initialize a new project from this template.
#
# Usage:
#   ./scripts/scaffold.sh [OPTIONS]
#
# Options:
#   -n, --name        <NAME>    Project name (snake_case)       [required]
#   -o, --org         <ORG>     GitHub/GitLab organisation name [required]
#   -p, --package     <PKG>     C++ namespace / Java package    [required]
#   -d, --description <DESC>   One-line project description    [optional]
#   -h, --help                  Show this help message
#
# Example:
#   ./scripts/scaffold.sh -n my_service -o acme-corp \
#       -p com.acme.myservice -d "ACME microservice"
#
# The script performs the following replacements throughout the repository:
#   premisses       → <NAME>
#   doevelopper     → <ORG>  (GitHub handle / organisation slug)
#   com/github/doevelopper/premisses → <PKG path>
#
# All replacements are done in-place on tracked files only (git ls-files).
# Run from the repository root.

set -euo pipefail

# ── Helpers ──────────────────────────────────────────────────────────────────
log()  { printf '\033[0;32m[scaffold]\033[0m %s\n' "$*"; }
warn() { printf '\033[0;33m[scaffold]\033[0m WARNING: %s\n' "$*" >&2; }
die()  { printf '\033[0;31m[scaffold]\033[0m ERROR: %s\n' "$*" >&2; exit 1; }

usage() {
    grep '^#' "$0" | sed 's/^# \{0,1\}//'
    exit 0
}

require_tool() {
    command -v "$1" >/dev/null 2>&1 || die "'$1' is required but not found in PATH."
}

# ── Argument parsing ──────────────────────────────────────────────────────────
PROJECT_NAME=""
ORG=""
PACKAGE=""
DESCRIPTION="A scaffolded software project."

while [[ $# -gt 0 ]]; do
    case "$1" in
        -n|--name)        PROJECT_NAME="$2"; shift 2 ;;
        -o|--org)         ORG="$2";          shift 2 ;;
        -p|--package)     PACKAGE="$2";      shift 2 ;;
        -d|--description) DESCRIPTION="$2";  shift 2 ;;
        -h|--help)        usage ;;
        *) die "Unknown option: $1" ;;
    esac
done

[[ -n "${PROJECT_NAME}" ]] || die "--name is required."
[[ -n "${ORG}"          ]] || die "--org is required."
[[ -n "${PACKAGE}"      ]] || die "--package is required."

# Validate: snake_case name
[[ "${PROJECT_NAME}" =~ ^[a-z][a-z0-9_]*$ ]] \
    || die "--name must be lowercase snake_case (e.g. my_service)."

# ── Pre-flight ────────────────────────────────────────────────────────────────
require_tool git
require_tool sed
require_tool find

[[ -d .git ]] || die "Must be run from the repository root (no .git found)."

# ── Derived values ────────────────────────────────────────────────────────────
TEMPLATE_PROJECT="premisses"
TEMPLATE_ORG="doevelopper"
TEMPLATE_PKG_PATH="com/github/doevelopper/premisses"

# Convert package com.acme.myservice → com/acme/myservice
PKG_PATH="${PACKAGE//.//}"

PROJECT_NAME_UPPER="${PROJECT_NAME^^}"   # SCREAMING_SNAKE for macros
PROJECT_NAME_CAMEL="$(echo "${PROJECT_NAME}" | sed -r 's/(^|_)([a-z])/\U\2/g')"

log "Scaffolding project:"
log "  Name        : ${PROJECT_NAME}  (upper: ${PROJECT_NAME_UPPER}, camel: ${PROJECT_NAME_CAMEL})"
log "  Organisation: ${ORG}"
log "  Package     : ${PACKAGE}  (path: ${PKG_PATH})"
log "  Description : ${DESCRIPTION}"

# ── File content replacement ──────────────────────────────────────────────────
log "Replacing template tokens in tracked files…"

# Collect text files tracked by git
mapfile -t FILES < <(git ls-files | xargs grep -rlI \
    -e "${TEMPLATE_PROJECT}" \
    -e "${TEMPLATE_ORG}" \
    -e "premisses" \
    2>/dev/null || true)

if [[ ${#FILES[@]} -eq 0 ]]; then
    warn "No files contained template tokens — nothing to replace."
else
    for f in "${FILES[@]}"; do
        sed -i \
            -e "s|${TEMPLATE_PKG_PATH}|${PKG_PATH}|g" \
            -e "s|${TEMPLATE_ORG}|${ORG}|g" \
            -e "s|${TEMPLATE_PROJECT^^}|${PROJECT_NAME_UPPER}|g" \
            -e "s|${TEMPLATE_PROJECT^}|${PROJECT_NAME_CAMEL}|g" \
            -e "s|${TEMPLATE_PROJECT}|${PROJECT_NAME}|g" \
            -- "${f}"
    done
    log "Replaced tokens in ${#FILES[@]} file(s)."
fi

# ── Directory rename ──────────────────────────────────────────────────────────
log "Renaming namespace directories…"

# Use git mv to preserve history for known namespace paths
rename_dir() {
    local old="$1" new="$2"
    if [[ -d "${old}" ]]; then
        mkdir -p "$(dirname "${new}")"
        git mv "${old}" "${new}"
        log "  Renamed: ${old} -> ${new}"
    fi
}

rename_dir "src/main/cpp/${TEMPLATE_PKG_PATH}"  "src/main/cpp/${PKG_PATH}"
rename_dir "src/test/cpp/${TEMPLATE_PKG_PATH}"  "src/test/cpp/${PKG_PATH}"
rename_dir "src/it/${TEMPLATE_PKG_PATH}"         "src/it/${PKG_PATH}"

# ── Update VERSION ────────────────────────────────────────────────────────────
echo "0.1.0" > VERSION
log "Reset VERSION to 0.1.0"

# ── Update README description ─────────────────────────────────────────────────
if [[ -f README.md ]]; then
    sed -i "1s|.*|# ${PROJECT_NAME_CAMEL}|" README.md
    log "Updated README.md title."
fi

# ── Initial commit ────────────────────────────────────────────────────────────
log "Staging scaffold changes…"
git add -A

log ""
log "Scaffold complete. Review the changes with 'git diff --cached', then:"
log "  git commit -m \"chore: initialise project from pom-scaffolding template\""
