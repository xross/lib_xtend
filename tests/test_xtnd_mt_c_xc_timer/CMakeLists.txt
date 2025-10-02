cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)

add_subdirectory(xtender)

project(test_xtnd_mt_c_xc_timer)

set(APP_INCLUDES src ../)

set(APP_HW_TARGET               XK-EVK-XU316)

set(COMPILER_FLAGS_COMMON       -O3 -Wall -lquadflash)

set(APP_COMPILER_FLAGS_t0          ${COMPILER_FLAGS_COMMON} -DTHREAD_NUM=0)

set(APP_COMPILER_FLAGS_t1          ${COMPILER_FLAGS_COMMON} -DTHREAD_NUM=1)
set(APP_DEPENDENT_MODULES       "lib_xtnd")

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)

XMOS_REGISTER_APP()

add_dependencies(test_xtnd_mt_c_xc_timer_t0 blob_xtnd_mt_c_xc_timer)
add_dependencies(test_xtnd_mt_c_xc_timer_t1 blob_xtnd_mt_c_xc_timer)

