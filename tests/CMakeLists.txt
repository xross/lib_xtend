cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(xtend_tests)

# Reusable helper using build_user_xplug.sh script:
#   xplug_add_test_plugin_blob(<TEST_EXECUTABLE_TARGET> <PLUGIN_TARGET>
#                              [HEADER_BASENAME name] [OUTPUT_DIR dir])
# Writes header + blob directly into the test application's src directory (or OUTPUT_DIR).
function(xtend_add_test_plugin_blob TEST_TARGET PLUGIN_TARGET)

  set(XPLUG_HEADER_BASENAME xtend_blob)

  # Test app source directory (assumes this function called from that dir's parent)
  set(XPLUG_OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/../${TEST_TARGET}/src")

  set(BLOB_HDR "${XPLUG_OUTPUT_DIR}/${XPLUG_HEADER_BASENAME}.h")
  set(BLOB_BIN "${XPLUG_OUTPUT_DIR}/xtend_blob.bin") # script fixed name

  add_custom_command(
    OUTPUT "${BLOB_HDR}"
    COMMAND ${CMAKE_COMMAND} -E echo "[xtend] Generating blob via build_user_xtend.sh for ${TEST_TARGET} (plugin ${PLUGIN_TARGET})"
    COMMAND bash ${CMAKE_SOURCE_DIR}/build_user_xtend.sh $<TARGET_FILE:${PLUGIN_TARGET}> "${BLOB_HDR}"
    WORKING_DIRECTORY "${XPLUG_OUTPUT_DIR}"
    DEPENDS ${PLUGIN_TARGET}
    BYPRODUCTS "${BLOB_BIN}"
    VERBATIM
    COMMENT "[xtend] Creating ${XPLUG_HEADER_BASENAME}.h in ${XPLUG_OUTPUT_DIR}"
  )

  add_custom_target(${TEST_TARGET}_xtend_blob
    DEPENDS "${BLOB_HDR}" )

  add_dependencies(${TEST_TARGET} ${TEST_TARGET}_xtend_blob)
  target_include_directories(${TEST_TARGET} PRIVATE "${XPLUG_OUTPUT_DIR}")
endfunction()

add_subdirectory(test_xtend_basic)
add_subdirectory(test_xtend_global)
add_subdirectory(test_xtend_resource)
add_subdirectory(test_xtend_print)

