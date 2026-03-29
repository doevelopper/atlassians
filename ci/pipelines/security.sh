#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"

source "${ROOT_DIR}/ci/lib/common.sh"
source "${ROOT_DIR}/ci/lib/security.sh"

log_section "Security"
init_artifacts

report_file="${REPORTS_DIR}/security.report"
report_header "${report_file}" "security"

if security_tool_available reuse; then
	log_info "Running REUSE compliance check"
	if reuse lint; then
		append_security_result "${report_file}" "reuse" "passed"
	else
		append_security_result "${report_file}" "reuse" "failed"
	fi
else
	log_warn "reuse not installed, skipping REUSE compliance check"
	append_security_result "${report_file}" "reuse" "skipped"
fi

if security_tool_available syft; then
	log_info "Generating SBOM via syft"
	syft "${ROOT_DIR}" -o spdx-json > "${SBOM_DIR}/security-sbom.spdx.json"
	append_security_result "${report_file}" "syft" "passed"
else
	log_warn "syft not installed, skipping SBOM generation in security stage"
	append_security_result "${report_file}" "syft" "skipped"
fi

if security_tool_available trivy; then
	log_info "Running trivy filesystem scan"
	trivy fs --quiet --exit-code 0 --format sarif --output "${REPORTS_DIR}/trivy.sarif" "${ROOT_DIR}"
	append_security_result "${report_file}" "trivy" "passed"
else
	log_warn "trivy not installed, skipping vulnerability scan"
	append_security_result "${report_file}" "trivy" "skipped"
fi

if security_tool_available semgrep; then
	log_info "Running semgrep scan"
	semgrep --config auto --error --quiet "${ROOT_DIR}" --json --output "${REPORTS_DIR}/semgrep.json" \
		|| append_security_result "${report_file}" "semgrep" "failed"
	if ! grep -q '^semgrep=' "${report_file}"; then
		append_security_result "${report_file}" "semgrep" "passed"
	fi
else
	log_warn "semgrep not installed, skipping SAST scan"
	append_security_result "${report_file}" "semgrep" "skipped"
fi

log_info "Security stage completed"