#!/usr/bin/env bash
#set -euo pipefail
trap 'echo "[ERROR] line $LINENO: $BASH_COMMAND" >&2' ERR

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "${script_dir}/../.." && pwd)"

pushd "${repo_root}" >/dev/null
./build_user_xplug.sh ./examples/app_test/xplugin/bin/xplugin.xe ./examples/app_test/src/xplugin_blob.h
popd >/dev/null

pushd "${repo_root}/examples/app_test" >/dev/null
make -C build
xobjdump -D ./bin/app_test.xe > bin.txt 2>&1
xsim ./bin/app_test.xe -t > trace.txt 2>&1
xsim ./bin/app_test.xe
popd >/dev/null
