#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"

source "${ROOT_DIR}/ci/lib/common.sh"

log_section "Build"
init_artifacts

report_file="${REPORTS_DIR}/build.report"
report_header "${report_file}" "build"

build_system="$(detect_build_system)"
log_info "Build system: ${build_system}"
printf 'build_system=%s\n' "${build_system}" >> "${report_file}"

case "${build_system}" in
	cmake)
		if ! command_exists cmake; then
			die "cmake build selected but cmake is not installed"
		fi
		generator="${CMAKE_GENERATOR:-Ninja}"
		run_cmd cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -G "${generator}" -DCMAKE_BUILD_TYPE=Release
		run_cmd cmake --build "${BUILD_DIR}" --parallel
		printf 'status=passed\n' >> "${report_file}"
		;;
	bazel)
		if ! command_exists bazel; then
			die "bazel build selected but bazel is not installed"
		fi
		run_cmd bazel build //...
		printf 'status=passed\n' >> "${report_file}"
		;;
	make)
		log_info "No dedicated project build manifest detected yet, skipping heavy build"
		printf 'status=skipped_scaffold\n' >> "${report_file}"
		;;
	*)
		log_info "No supported build manifest detected, scaffold mode successful"
		printf 'status=skipped_no_manifest\n' >> "${report_file}"
		;;
esac

log_info "Build stage completed"