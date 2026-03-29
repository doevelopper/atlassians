#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"

source "${ROOT_DIR}/ci/lib/common.sh"

log_section "Bootstrap"
init_artifacts

report_file="${REPORTS_DIR}/bootstrap.report"
report_header "${report_file}" "bootstrap"

for tool in bash git make; do
	if command_exists "${tool}"; then
		log_info "Found required tool: ${tool}"
		printf 'tool_%s=present\n' "${tool}" >> "${report_file}"
	else
		die "Required tool is missing: ${tool}"
	fi
done

for optional in cmake ctest bazel shellcheck hadolint yamllint reuse syft trivy semgrep; do
	if command_exists "${optional}"; then
		log_info "Optional tool available: ${optional}"
		printf 'optional_%s=present\n' "${optional}" >> "${report_file}"
	else
		log_warn "Optional tool missing: ${optional}"
		printf 'optional_%s=missing\n' "${optional}" >> "${report_file}"
	fi
done

build_system="$(detect_build_system)"
printf 'detected_build_system=%s\n' "${build_system}" >> "${report_file}"
log_info "Detected build system: ${build_system}"

log_info "Bootstrap completed"