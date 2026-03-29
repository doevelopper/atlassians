#!/usr/bin/env bash

docker_available() {
	command -v docker >/dev/null 2>&1
}

docker_build_image() {
	local dockerfile="$1"
	local image_tag="$2"
	local context_dir="${3:-.}"
	local version="${4:-dev}"
	local revision="${5:-unknown}"

	if ! docker_available; then
		return 1
	fi

	docker build \
		-f "${dockerfile}" \
		--build-arg "BUILD_DATE=$(date -u +"%Y-%m-%dT%H:%M:%SZ")" \
		--build-arg "REVISION=${revision}" \
		--build-arg "BUILD_VERSION=${version}" \
		-t "${image_tag}" \
		"${context_dir}"
}