#!/usr/bin/env bash

git_commit_sha() {
	if command -v git >/dev/null 2>&1; then
		git rev-parse --short=12 HEAD 2>/dev/null || echo "unknown"
	else
		echo "unknown"
	fi
}

git_commit_sha_full() {
	if command -v git >/dev/null 2>&1; then
		git rev-parse HEAD 2>/dev/null || echo "unknown"
	else
		echo "unknown"
	fi
}

git_branch_name() {
	if [[ -n "${GITHUB_REF_NAME:-}" ]]; then
		echo "${GITHUB_REF_NAME}"
		return 0
	fi

	if command -v git >/dev/null 2>&1; then
		git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "unknown"
	else
		echo "unknown"
	fi
}

git_is_tag_build() {
	if [[ "${GITHUB_REF:-}" == refs/tags/* ]]; then
		return 0
	fi

	if command -v git >/dev/null 2>&1 && git describe --exact-match --tags HEAD >/dev/null 2>&1; then
		return 0
	fi

	return 1
}