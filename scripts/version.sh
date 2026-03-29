#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
VERSION_FILE="${ROOT_DIR}/VERSION"

version="0.0.0-dev"
if [[ -f "${VERSION_FILE}" ]]; then
	version="$(tr -d ' \n\t\r' < "${VERSION_FILE}")"
fi

if [[ "${1:-}" == "--json" ]]; then
	printf '{"version":"%s","commit":"%s"}\n' "${version}" "$(git -C "${ROOT_DIR}" rev-parse --short=12 HEAD 2>/dev/null || echo unknown)"
	exit 0
fi

echo "${version}"