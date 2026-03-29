#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"

source "${ROOT_DIR}/ci/lib/common.sh"

log_section "Package"
init_artifacts

report_file="${REPORTS_DIR}/package.report"
report_header "${report_file}" "package"

version="$(tr -d ' \n\t\r' < "${ROOT_DIR}/VERSION" 2>/dev/null || true)"
if [[ -z "${version}" ]]; then
	version="0.0.0-dev"
fi

archive_path="${BUILD_DIR}/atlassians-${version}.tar.gz"

if command_exists git; then
	log_info "Packaging source archive from git"
	git -C "${ROOT_DIR}" archive --format=tar.gz --output "${archive_path}" HEAD
else
	log_warn "git not available; packaging workspace snapshot"
	tar -C "${ROOT_DIR}" -czf "${archive_path}" .
fi

sha256sum "${archive_path}" > "${BUILD_DIR}/package.sha256"

printf 'version=%s\n' "${version}" >> "${report_file}"
printf 'archive=%s\n' "${archive_path}" >> "${report_file}"

log_info "Package stage completed"