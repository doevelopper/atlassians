#!/usr/bin/env bash

security_tool_available() {
	command -v "$1" >/dev/null 2>&1
}

append_security_result() {
	local report_file="$1"
	local tool_name="$2"
	local status="$3"
	printf '%s=%s\n' "${tool_name}" "${status}" >> "${report_file}"
}