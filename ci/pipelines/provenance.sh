#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"

source "${ROOT_DIR}/ci/lib/common.sh"

log_section "Provenance"
init_artifacts

report_file="${REPORTS_DIR}/provenance.report"
report_header "${report_file}" "provenance"

provenance_file="${PROVENANCE_DIR}/provenance.json"

sha_entries="[]"
if compgen -G "${BUILD_DIR}/*.tar.gz" >/dev/null; then
	entries=()
	while IFS= read -r line; do
		digest="${line%% *}"
		file_path="${line#* }"
		entries+=("{\"file\":\"${file_path}\",\"sha256\":\"${digest}\"}")
	done < <(sha256sum "${BUILD_DIR}"/*.tar.gz)

	sha_entries="[$(IFS=,; echo "${entries[*]}")]"
fi

cat > "${provenance_file}" <<EOF
{
	"project": "atlassians",
	"generatedAt": "$(date -u +"%Y-%m-%dT%H:%M:%SZ")",
	"commit": "$(git_commit_sha_full)",
	"branch": "$(git_branch_name)",
	"builder": "${GITHUB_ACTOR:-local}",
	"workflow": "${GITHUB_WORKFLOW:-local}",
	"artifacts": ${sha_entries}
}
EOF

printf 'provenance_file=%s\n' "${provenance_file}" >> "${report_file}"
log_info "Provenance stage completed"