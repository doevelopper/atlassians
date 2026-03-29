#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"

source "${ROOT_DIR}/ci/lib/common.sh"

log_section "SBOM"
init_artifacts

report_file="${REPORTS_DIR}/sbom.report"
report_header "${report_file}" "sbom"

sbom_file="${SBOM_DIR}/sbom.spdx.json"

if command_exists syft; then
	log_info "Generating SBOM with syft"
	syft "${ROOT_DIR}" -o spdx-json > "${sbom_file}"
	printf 'generator=syft\n' >> "${report_file}"
else
	log_warn "syft not available, creating minimal fallback SBOM"
	cat > "${sbom_file}" <<EOF
{
	"SPDXID": "SPDXRef-DOCUMENT",
	"spdxVersion": "SPDX-2.3",
	"name": "atlassians-fallback-sbom",
	"creationInfo": {
		"created": "$(date -u +"%Y-%m-%dT%H:%M:%SZ")",
		"creators": ["Tool: atlassians-ci-fallback"]
	},
	"packages": []
}
EOF
	printf 'generator=fallback\n' >> "${report_file}"
fi

log_info "SBOM stage completed"