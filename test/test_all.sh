#!/bin/bash

set -e

TEST_PATH="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
declare -r TEST_PATH

PROJECT_ROOT_PATH="$(readlink -f "${TEST_PATH}/../")"
declare -r PROJECT_ROOT_PATH

BUILD_PATH="${TEST_PATH}/build"
declare -r BUILD_PATH

DIST_PATH="${TEST_PATH}/dist"
declare -r DIST_PATH

build() {
	[[ -d "${BUILD_PATH}" ]] && rm -r "${BUILD_PATH}"
	[[ -d "${DIST_PATH}" ]] && rm -r "${DIST_PATH}"

	pushd "${PROJECT_ROOT_PATH}" >/dev/null
	meson -Dprefix="${DIST_PATH}" "${BUILD_PATH}"
	ninja install -C "${BUILD_PATH}"
	popd >/dev/null
}

run_case() {
	local path="${1}"
	pushd "${path}" >/dev/null
	bash test.sh "${DIST_PATH}/bin"
	popd >/dev/null
}

main() {
	build
	while read -r dir; do
		echo -e "\033[34;3mRun ${dir}\033[0m"
		run_case "${dir}"
		echo -e "\033[32;1mSuccess!\033[0m"
	done < <(find "${TEST_PATH}" -mindepth 1 -maxdepth 1 -type d -name "case*" | sort)
}

main "${@}"
