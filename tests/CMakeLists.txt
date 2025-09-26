cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(xtend_tests)

include(${CMAKE_CURRENT_LIST_DIR}/../xtnd_utils.cmake)

add_subdirectory(test_xtend_basic)
add_subdirectory(test_xtend_global)
add_subdirectory(test_xtend_global_bss)
add_subdirectory(test_xtend_resource)
add_subdirectory(test_xtend_interface)
add_subdirectory(test_xtend_interface_multiclient)
add_subdirectory(test_xtend_xc_timer)
add_subdirectory(test_xtend_print)

