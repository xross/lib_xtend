cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)

add_subdirectory(xtender)

project(test_xtnd_xc_xc_basic)

set(APP_INCLUDES src ../)

set(APP_HW_TARGET               XK-EVK-XU316)

set(APP_COMPILER_FLAGS          -O3 -Wall -lquadflash -DXTND_BASE_ADDR=${XTND_BASE_ADDR})

set(APP_DEPENDENT_MODULES       "lib_xtnd")

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)

XMOS_REGISTER_APP()

add_dependencies(test_xtnd_xc_xc_basic blob_xtnd_xc_xc_basic)
