cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)

# Default xtender (single) base address (header/image base)
set(XTEND_IMAGE_ADDR "0x00084000" CACHE STRING
    "Absolute base address where the xtender header (xtend_exports) will be placed")

add_subdirectory(xtender)

project(test_xtnd_c_xc_interface)

set(APP_INCLUDES src ../)

set(APP_HW_TARGET               XK-EVK-XU316)

set(APP_COMPILER_FLAGS          -O3 -Wall -lquadflash -mno-dual-issue
                                -DXTEND_IMAGE_ADDR=${XTEND_IMAGE_ADDR})

set(APP_DEPENDENT_MODULES       "lib_xtnd")

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)

XMOS_REGISTER_APP()

add_dependencies(test_xtnd_c_xc_interface blob_xtend_xc_interface)
