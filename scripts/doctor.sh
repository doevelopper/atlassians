#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

missing=0

echo "[doctor] Repository root: ${ROOT_DIR}"
echo "[doctor] Timestamp: $(date -u +"%Y-%m-%dT%H:%M:%SZ")"

for required in bash git make; do
	if command -v "${required}" >/dev/null 2>&1; then
		echo "[doctor] OK   ${required}"
	else
		echo "[doctor] MISS ${required}"
		missing=$((missing + 1))
	fi
done

for optional in cmake ctest bazel docker shellcheck hadolint yamllint trivy syft semgrep reuse; do
	if command -v "${optional}" >/dev/null 2>&1; then
		echo "[doctor] OPT  ${optional}"
	else
		echo "[doctor] OPT-MISSING ${optional}"
	fi
done

if [[ "${missing}" -gt 0 ]]; then
	echo "[doctor] Missing required tools: ${missing}"
	exit 1
fi

echo "[doctor] Environment is ready for scaffold CI/local workflows"