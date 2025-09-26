cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(xtender_i2c)

set(APP_HW_TARGET XU316-1024-FB265-C32)

set(APP_COMPILER_FLAGS  -O2
                        -mno-dual-issue
                        -nostartfiles
                        -Wl,--image-base,${XTEND_IMAGE_ADDR}
                    )

# This is a hack to ensure that the user_exports.S file is passed to the compiler first.
set(APP_XC_SRCS ./src/xtender_exports.S)
set(APP_C_SRCS ./src/xtender.xc)

set(APP_DEPENDENT_MODULES "lib_i2c")

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../..)

include(${XMOS_SANDBOX_DIR}/lib_xtnd/xtnd_utils.cmake)

XTND_REGISTER_APP()

build_xtend_blob(${CMAKE_CURRENT_LIST_DIR}/bin/xtender_i2c.xe)

