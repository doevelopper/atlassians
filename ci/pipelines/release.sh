#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"

source "${ROOT_DIR}/ci/lib/common.sh"

log_section "Release"
init_artifacts

report_file="${REPORTS_DIR}/release.report"
report_header "${report_file}" "release"

dry_run="${DRY_RUN:-false}"
release_mode="${CI_RELEASE:-false}"

if [[ "${dry_run}" == "true" ]]; then
	log_info "Release dry-run mode enabled"
fi

if [[ "${release_mode}" != "true" ]] && [[ "${dry_run}" != "true" ]]; then
	log_warn "CI_RELEASE is not true; producing release metadata only"
fi

version="$(tr -d ' \n\t\r' < "${ROOT_DIR}/VERSION" 2>/dev/null || true)"
if [[ -z "${version}" ]]; then
	version="0.0.0-dev"
fi

release_notes="${ARTIFACTS_DIR}/reports/release-notes.md"
cat > "${release_notes}" <<EOF
# Release ${version}

- Commit: $(git_commit_sha_full)
- Branch: $(git_branch_name)
- Generated: $(date -u +"%Y-%m-%dT%H:%M:%SZ")
- Mode: $( [[ "${dry_run}" == "true" ]] && echo "dry-run" || echo "release" )

## Artifacts

$(ls -1 "${BUILD_DIR}" 2>/dev/null || echo "No build artifacts yet")
EOF

printf 'version=%s\n' "${version}" >> "${report_file}"
printf 'dry_run=%s\n' "${dry_run}" >> "${report_file}"
printf 'release_mode=%s\n' "${release_mode}" >> "${report_file}"
printf 'release_notes=%s\n' "${release_notes}" >> "${report_file}"

log_info "Release stage completed"