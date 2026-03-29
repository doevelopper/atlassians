#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"

source "${ROOT_DIR}/ci/lib/common.sh"

log_section "Lint"
init_artifacts

report_file="${REPORTS_DIR}/lint.report"
report_header "${report_file}" "lint"

failures=0

if command_exists shellcheck; then
	mapfile -t shell_files < <(find "${ROOT_DIR}/ci" "${ROOT_DIR}/scripts" -type f -name '*.sh' | sort)
	if [[ ${#shell_files[@]} -gt 0 ]]; then
		log_info "Running shellcheck on ${#shell_files[@]} shell scripts"
		if ! shellcheck "${shell_files[@]}"; then
			failures=$((failures + 1))
			printf 'shellcheck=failed\n' >> "${report_file}"
		else
			printf 'shellcheck=passed\n' >> "${report_file}"
		fi
	else
		log_warn "No shell scripts found for shellcheck"
		printf 'shellcheck=skipped\n' >> "${report_file}"
	fi
else
	log_warn "shellcheck not installed, skipping shell lint"
	printf 'shellcheck=skipped\n' >> "${report_file}"
fi

if command_exists hadolint; then
	mapfile -t dockerfiles < <(find "${ROOT_DIR}" -type f -name 'Dockerfile' | sort)
	if [[ ${#dockerfiles[@]} -gt 0 ]]; then
		log_info "Running hadolint on ${#dockerfiles[@]} Dockerfiles"
		if ! hadolint "${dockerfiles[@]}"; then
			failures=$((failures + 1))
			printf 'hadolint=failed\n' >> "${report_file}"
		else
			printf 'hadolint=passed\n' >> "${report_file}"
		fi
	else
		log_warn "No Dockerfiles found for hadolint"
		printf 'hadolint=skipped\n' >> "${report_file}"
	fi
else
	log_warn "hadolint not installed, skipping Dockerfile lint"
	printf 'hadolint=skipped\n' >> "${report_file}"
fi

if command_exists yamllint; then
	yaml_files=(
		"${ROOT_DIR}/.github/workflows/ci.yml"
		"${ROOT_DIR}/.github/workflows/security.yml"
		"${ROOT_DIR}/.github/workflows/release.yml"
		"${ROOT_DIR}/.github/workflows/container-images.yml"
		"${ROOT_DIR}/.gitlab-ci.yml"
		"${ROOT_DIR}/.yamllint.yml"
		"${ROOT_DIR}/.hadolint.yaml"
		"${ROOT_DIR}/.markdownlint.yaml"
	)

	existing_yaml_files=()
	for file in "${yaml_files[@]}"; do
		if [[ -f "${file}" ]]; then
			existing_yaml_files+=("${file}")
		fi
	done

	yaml_files=("${existing_yaml_files[@]}")
	if [[ ${#yaml_files[@]} -gt 0 ]]; then
		log_info "Running yamllint on ${#yaml_files[@]} YAML files"
		if ! yamllint "${yaml_files[@]}"; then
			failures=$((failures + 1))
			printf 'yamllint=failed\n' >> "${report_file}"
		else
			printf 'yamllint=passed\n' >> "${report_file}"
		fi
	else
		printf 'yamllint=skipped\n' >> "${report_file}"
	fi
else
	log_warn "yamllint not installed, skipping YAML lint"
	printf 'yamllint=skipped\n' >> "${report_file}"
fi

if [[ "${failures}" -gt 0 ]]; then
	die "Lint stage failed (${failures} failing tool groups)"
fi

log_info "Lint completed successfully"