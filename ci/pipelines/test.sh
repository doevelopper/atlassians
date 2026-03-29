#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"

source "${ROOT_DIR}/ci/lib/common.sh"

log_section "Test"
init_artifacts

report_file="${REPORTS_DIR}/test.report"
junit_file="${TEST_REPORTS_DIR}/junit.xml"
report_header "${report_file}" "test"

build_system="$(detect_build_system)"
printf 'build_system=%s\n' "${build_system}" >> "${report_file}"

case "${build_system}" in
	cmake)
		if ! command_exists ctest; then
			die "ctest is required for CMake tests"
		fi
		run_cmd ctest --test-dir "${BUILD_DIR}" --output-on-failure
		cat > "${junit_file}" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<testsuites tests="1" failures="0" errors="0" skipped="0" time="0">
	<testsuite name="cmake-tests" tests="1" failures="0" errors="0" skipped="0" time="0">
		<testcase classname="pipeline" name="ctest" time="0"/>
	</testsuite>
</testsuites>
EOF
		printf 'status=passed\n' >> "${report_file}"
		;;
	bazel)
		if ! command_exists bazel; then
			die "bazel is required for Bazel tests"
		fi
		run_cmd bazel test //...
		cat > "${junit_file}" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<testsuites tests="1" failures="0" errors="0" skipped="0" time="0">
	<testsuite name="bazel-tests" tests="1" failures="0" errors="0" skipped="0" time="0">
		<testcase classname="pipeline" name="bazel-test" time="0"/>
	</testsuite>
</testsuites>
EOF
		printf 'status=passed\n' >> "${report_file}"
		;;
	*)
		log_info "No executable test harness detected yet, writing scaffold JUnit"
		cat > "${junit_file}" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<testsuites tests="1" failures="0" errors="0" skipped="1" time="0">
	<testsuite name="scaffold-tests" tests="1" failures="0" errors="0" skipped="1" time="0">
		<testcase classname="pipeline" name="no-tests-yet" time="0">
			<skipped message="No CMake/Bazel test manifest available yet"/>
		</testcase>
	</testsuite>
</testsuites>
EOF
		printf 'status=skipped_scaffold\n' >> "${report_file}"
		;;
esac

log_info "Test stage completed"