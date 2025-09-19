cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)

add_subdirectory(xtender)

project(test_xtend_global)

set(APP_INCLUDES src)

set(APP_HW_TARGET               XK-EVK-XU316)

set(APP_COMPILER_FLAGS          -O3 -Wall -lquadflash)

set(APP_DEPENDENT_MODULES       "lib_xtend")

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)

XMOS_REGISTER_APP()

xtend_add_test_plugin_blob(test_xtend_global xtender_global)
