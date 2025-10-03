cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)

project(xtnd_xc_xc_basic)

set(APP_HW_TARGET XU316-1024-QF60B-C24)

set(APP_COMPILER_FLAGS  -O0
                        -mno-dual-issue
                        -nostartfiles
                        -Wl,--image-base,${XTND_BASE_ADDR}
                    )

set(APP_XC_SRCS ./src/xtender_exports.S)
set(APP_C_SRCS ./src/xtender.xc)

set(APP_DEPENDENT_MODULES "")

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../..)

include(${XMOS_SANDBOX_DIR}/lib_xtnd/xtnd_utils.cmake)

XTND_REGISTER_APP()

build_xtend_blob("$<TARGET_FILE:xtnd_xc_xc_basic>" TARGET xtnd_xc_xc_basic GENERATE_HEADER HEADER_OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/../src" HEADER_FILENAME xtend_blob.h ARRAY_SYMBOL xtend_blob_bin)

