cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(xtend_tests)

include(${CMAKE_CURRENT_LIST_DIR}/../xtnd_utils.cmake)

# Helper: generate plugin blob header for a test target.
# Usage:
#   xtend_add_test_plugin_blob(<TEST_EXEC_TARGET> <PLUGIN_TARGET>
#                              [HEADER_BASENAME name] [OUTPUT_DIR dir])
# Behaviour:
#   - Generates <HEADER_BASENAME>.h containing xtend_blob.bin as C array.
#   - Header + blob emitted once per (plugin,target path) even if multiple
#     test variant targets depend on same plugin (e.g. _t0 / _t1 variants).
#   - Default HEADER_BASENAME: xtend_blob
#   - Default OUTPUT_DIR: <test source dir>/src
function(xtend_add_test_plugin_blob TEST_TARGET PLUGIN_TARGET)
  set(oneValueArgs HEADER_BASENAME OUTPUT_DIR)
  cmake_parse_arguments(XTB "" "HEADER_BASENAME;OUTPUT_DIR" "" ${ARGN})

  if(NOT TARGET ${TEST_TARGET})
    message(FATAL_ERROR "xtend_add_test_plugin_blob: test target '${TEST_TARGET}' not found")
  endif()
  if(NOT TARGET ${PLUGIN_TARGET})
    message(FATAL_ERROR "xtend_add_test_plugin_blob: plugin target '${PLUGIN_TARGET}' not found")
  endif()

  if(NOT XTB_HEADER_BASENAME)
    set(XTB_HEADER_BASENAME xtend_blob)
  endif()

  # SOURCE_DIR of the test target (same for variant targets produced by flags suffixes)
  get_target_property(_test_src_dir ${TEST_TARGET} SOURCE_DIR)
  if(NOT _test_src_dir)
    message(FATAL_ERROR "xtend_add_test_plugin_blob: cannot determine SOURCE_DIR for ${TEST_TARGET}")
  endif()

  if(XTB_OUTPUT_DIR)
    set(_out_dir "${XTB_OUTPUT_DIR}")
  else()
    set(_out_dir "${_test_src_dir}/src")
  endif()
  file(MAKE_DIRECTORY "${_out_dir}")

  # Build header + blob via shared helper (once per plugin target)
  # Use the plugin's produced .xe path; rely on build_xtend_blob for idempotency
  get_target_property(_plugin_exe_dir ${PLUGIN_TARGET} BINARY_DIR)
  if(NOT _plugin_exe_dir)
    message(FATAL_ERROR "xtend_add_test_plugin_blob: cannot determine BINARY_DIR for plugin ${PLUGIN_TARGET}")
  endif()

  # Use generator expression for target file so we don't assume directory layout
  set(_plugin_xe "$<TARGET_FILE:${PLUGIN_TARGET}>")

  set(_gen_target xtend_blob_gen_${PLUGIN_TARGET})
  if(NOT TARGET ${_gen_target})
    build_xtend_blob("${_plugin_xe}" TARGET ${PLUGIN_TARGET} GENERATE_HEADER HEADER_OUTPUT_DIR "${_out_dir}" HEADER_FILENAME "${XTB_HEADER_BASENAME}.h" ARRAY_SYMBOL xtend_blob_bin)
    add_custom_target(${_gen_target} DEPENDS blob_${PLUGIN_TARGET})
  endif()

  add_dependencies(${TEST_TARGET} ${_gen_target})
  target_include_directories(${TEST_TARGET} PRIVATE "${_out_dir}")
endfunction()

add_subdirectory(test_xtend_basic)
add_subdirectory(test_xtend_global)
add_subdirectory(test_xtend_global_bss)
add_subdirectory(test_xtend_resource)
add_subdirectory(test_xtend_interface)
add_subdirectory(test_xtend_interface_multiclient)
add_subdirectory(test_xtend_xc_timer)
add_subdirectory(test_xtend_print)

