#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
COMPOSE_FILE="${ROOT_DIR}/.devcontainer/docker-compose.yml"
REPORT_FILE="${ROOT_DIR}/artifacts/reports/devcontainer-smoke.report"
SERVICE="workspace"

SKIP_BUILD=0
SKIP_POST_CREATE=0

usage() {
	cat <<EOF
Usage: $(basename "$0") [options]

Run repeatable local smoke checks for the devcontainer stack:
1) docker compose config render
2) docker compose build for the workspace service
3) one-off post-create sanity execution inside the workspace container

Options:
  --skip-build        Skip container build check
  --skip-post-create  Skip post-create sanity check
  -h, --help          Show this help message
EOF
}

while [[ $# -gt 0 ]]; do
	case "$1" in
		--skip-build)
			SKIP_BUILD=1
			shift
			;;
		--skip-post-create)
			SKIP_POST_CREATE=1
			shift
			;;
		-h|--help)
			usage
			exit 0
			;;
		*)
			echo "[devcontainer-smoke] Unknown option: $1" >&2
			usage >&2
			exit 2
			;;
	esac
done

mkdir -p "$(dirname "${REPORT_FILE}")"
: > "${REPORT_FILE}"

log() {
	local message="$1"
	echo "[devcontainer-smoke] ${message}" | tee -a "${REPORT_FILE}"
}

run_step() {
	local step="$1"
	shift
	log "START ${step}"
	"$@" 2>&1 | tee -a "${REPORT_FILE}"
	log "PASS  ${step}"
}

require_cmd() {
	local cmd="$1"
	if ! command -v "${cmd}" >/dev/null 2>&1; then
		echo "[devcontainer-smoke] Missing required command: ${cmd}" | tee -a "${REPORT_FILE}" >&2
		exit 1
	fi
}

compose() {
	docker compose -f "${COMPOSE_FILE}" "$@"
}

log "Repository root: ${ROOT_DIR}"
log "Timestamp (UTC): $(date -u +"%Y-%m-%dT%H:%M:%SZ")"

if [[ ! -f "${COMPOSE_FILE}" ]]; then
	log "Compose file not found: ${COMPOSE_FILE}"
	exit 1
fi

require_cmd docker

run_step "compose-config" compose config

if [[ "${SKIP_BUILD}" -eq 0 ]]; then
	run_step "compose-build-${SERVICE}" compose build "${SERVICE}"
else
	log "SKIP  compose-build-${SERVICE}"
fi

if [[ "${SKIP_POST_CREATE}" -eq 0 ]]; then
	run_step "post-create-sanity" compose run --rm --no-deps "${SERVICE}" bash -lc '
		set -euo pipefail
		bash /workspace/.devcontainer/scripts/post-create.sh
		test -f "$HOME/status"
		grep -q "post-create start" "$HOME/status"
		grep -q "post-create end" "$HOME/status"
		git config --global --get-all safe.directory | grep -qx "/workspace"
	'
else
	log "SKIP  post-create-sanity"
fi

log "All requested smoke checks passed."
log "Report written to ${REPORT_FILE}"
