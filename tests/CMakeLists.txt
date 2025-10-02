cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(xtend_tests)

include(${CMAKE_CURRENT_LIST_DIR}/../xtnd_utils.cmake)

add_subdirectory(test_xtnd_c_c_basic)
add_subdirectory(test_xtnd_c_c_global)
add_subdirectory(test_xtnd_c_c_global_bss)

add_subdirectory(test_xtnd_c_xc_resource)
add_subdirectory(test_xtnd_c_xc_interface)
add_subdirectory(test_xtnd_c_xc_interface_multiclient)
add_subdirectory(test_xtnd_mt_c_xc_timer)

add_subdirectory(test_xtnd_xc_c_basic)

add_subdirectory(test_xtnd_xc_xc_basic)
add_subdirectory(test_xtnd_xc_xc_timer)

#add_subdirectory(test_xtnd_xc_xc_timer_isolation)
#add_subdirectory(test_xtnd_mt_xc_xc_timer_isolation)

