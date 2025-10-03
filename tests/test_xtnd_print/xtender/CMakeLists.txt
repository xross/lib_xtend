cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(xtender_print)

set(APP_HW_TARGET XU316-1024-QF60B-C24)

set(APP_COMPILER_FLAGS  -O0
                        -mno-dual-issue
                        -nostartfiles
                        -Wl,--image-base,${XTND_BASE_ADDR}
                    )

# This is a hack to ensure that the xtender_exports.S file is passed to the compiler first.
set(APP_XC_SRCS ./src/xtender_exports.S)
set(APP_C_SRCS ./src/xtender.c)

set(APP_DEPENDENT_MODULES "")

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../..)

include(${XMOS_SANDBOX_DIR}/lib_xtnd/xtnd_utils.cmake)

XTND_REGISTER_APP()

build_xtend_blob("$<TARGET_FILE:xtender_print>" TARGET xtender_print GENERATE_HEADER HEADER_OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/../src" HEADER_FILENAME xtend_blob.h ARRAY_SYMBOL xtend_blob_bin)
