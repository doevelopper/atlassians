#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo "[format] Formatting sources in ${ROOT_DIR}"

if command -v clang-format >/dev/null 2>&1; then
	mapfile -t cpp_files < <(find "${ROOT_DIR}" -type f \( -name '*.c' -o -name '*.cc' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp' \))
	if [[ ${#cpp_files[@]} -gt 0 ]]; then
		clang-format -i "${cpp_files[@]}"
		echo "[format] Formatted ${#cpp_files[@]} C/C++ files"
	fi
else
	echo "[format] clang-format not installed, skipping C/C++ formatting"
fi

if command -v shfmt >/dev/null 2>&1; then
	mapfile -t sh_files < <(find "${ROOT_DIR}/ci" "${ROOT_DIR}/scripts" -type f -name '*.sh')
	if [[ ${#sh_files[@]} -gt 0 ]]; then
		shfmt -w "${sh_files[@]}"
		echo "[format] Formatted ${#sh_files[@]} shell files"
	fi
else
	echo "[format] shfmt not installed, skipping shell formatting"
fi