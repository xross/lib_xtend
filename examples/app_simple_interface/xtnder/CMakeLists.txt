cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(xtnd_interface)

set(APP_HW_TARGET XU316-1024-FB265-C32)

set(APP_COMPILER_FLAGS  -O0
                        -mno-dual-issue
                        -nostartfiles
                        -Wl,--image-base,${XTND_BASE_ADDR}
                    )

# Ensure xtnder_exports.S is first for symbol ordering.
set(APP_XC_SRCS ./src/xtnder_exports.S)
set(APP_C_SRCS ./src/xtnder.xc)

set(APP_DEPENDENT_MODULES "")

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../..)

include(${XMOS_SANDBOX_DIR}/lib_xtnd/xtnd_utils.cmake)

XTND_REGISTER_APP()

build_xtnd_blob($<TARGET_FILE:xtnd_interface> TARGET xtnd_interface)

