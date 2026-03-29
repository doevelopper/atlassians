#!/usr/bin/env bash

set -euo pipefail

CI_LIB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${CI_LIB_DIR}/../.." && pwd)"

source "${CI_LIB_DIR}/logging.sh"
source "${CI_LIB_DIR}/git.sh"

ARTIFACTS_DIR="${ROOT_DIR}/artifacts"
BUILD_DIR="${ARTIFACTS_DIR}/build"
REPORTS_DIR="${ARTIFACTS_DIR}/reports"
TEST_REPORTS_DIR="${ARTIFACTS_DIR}/test"
SBOM_DIR="${ARTIFACTS_DIR}/sbom"
PROVENANCE_DIR="${ARTIFACTS_DIR}/provenance"
COVERAGE_DIR="${ARTIFACTS_DIR}/coverage"

ensure_dir() {
	mkdir -p "$1"
}

init_artifacts() {
	ensure_dir "${ARTIFACTS_DIR}"
	ensure_dir "${BUILD_DIR}"
	ensure_dir "${REPORTS_DIR}"
	ensure_dir "${TEST_REPORTS_DIR}"
	ensure_dir "${SBOM_DIR}"
	ensure_dir "${PROVENANCE_DIR}"
	ensure_dir "${COVERAGE_DIR}"
}

command_exists() {
	command -v "$1" >/dev/null 2>&1
}

run_cmd() {
	log_info "Running: $*"
	"$@"
}

detect_build_system() {
	if [[ -f "${ROOT_DIR}/CMakeLists.txt" ]]; then
		echo "cmake"
		return 0
	fi
	if [[ -f "${ROOT_DIR}/WORKSPACE" || -f "${ROOT_DIR}/WORKSPACE.bazel" || -f "${ROOT_DIR}/MODULE.bazel" ]]; then
		echo "bazel"
		return 0
	fi
	if [[ -f "${ROOT_DIR}/Makefile" ]]; then
		echo "make"
		return 0
	fi
	echo "none"
}

write_kv_report() {
	local report_file="$1"
	shift
	: > "${report_file}"
	while [[ $# -gt 1 ]]; do
		printf '%s=%s\n' "$1" "$2" >> "${report_file}"
		shift 2
	done
}

report_header() {
	local report_file="$1"
	local stage="$2"
	cat > "${report_file}" <<EOF
stage=${stage}
timestamp=$(date -u +"%Y-%m-%dT%H:%M:%SZ")
commit=$(git_commit_sha)
branch=$(git_branch_name)
EOF
}