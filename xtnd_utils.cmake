

function(build_xtend_blob xe_path)
    # Args:
    #   xe_path              Absolute or relative path to the .xe file (expected final location)
    #   [BLOB_NAME name]     Optional output blob filename (default: xtend_blob.bin)
    #   [TARGET tgt]         Optional CMake target that builds the .xe (for stronger dependency)

    set(oneValueArgs BLOB_NAME TARGET)
    cmake_parse_arguments(BXB "" "BLOB_NAME;TARGET" "" ${ARGN})

    if(NOT xe_path)
        message(FATAL_ERROR "build_xtend_blob: xe_path argument missing")
    endif()

    # Resolve paths / names
    get_filename_component(_xe_abs  "${xe_path}" ABSOLUTE)
    get_filename_component(_xe_dir  "${_xe_abs}" DIRECTORY)
    get_filename_component(_base    "${_xe_abs}" NAME_WE)

    if(NOT EXISTS "${_xe_dir}")
        # Create the bin dir so copy at build time won’t fail
        file(MAKE_DIRECTORY "${_xe_dir}")
    endif()

    if(NOT EXISTS "${_xe_abs}")
        message(STATUS "[xtend] XE will be produced later: ${_xe_abs}")
    endif()

    set(_blob_name "${BXB_BLOB_NAME}")
    if(NOT _blob_name)
        set(_blob_name "xtend_blob.bin")
    endif()

    # Dedicated working directory inside the build tree to keep it clean
    set(_work_dir "${CMAKE_CURRENT_BINARY_DIR}/xtend_blob_${_base}")
    file(MAKE_DIRECTORY "${_work_dir}")

    # Internal (tracked) artifacts live ONLY in build tree
    set(_xb            "${_work_dir}/${_base}.xb")
    set(_raw_image     "${_work_dir}/image_n0c0.bin")          # Assumes node0/tile0
    set(_internal_blob "${_work_dir}/${_blob_name}")

    # External (final) blob copied next to the .xe in bin/
    set(_external_blob "${_xe_dir}/${_blob_name}")

    # Collect dependencies
    set(_deps "${_xe_abs}")
    if(BXB_TARGET)
        if(TARGET "${BXB_TARGET}")
            list(APPEND _deps "${BXB_TARGET}")
        else()
            message(WARNING "build_xtend_blob: TARGET '${BXB_TARGET}' not found; relying on file dependency only")
        endif()
    endif()

    # Produce internal blob, then copy to final destination
    add_custom_command(
        OUTPUT "${_internal_blob}"
        COMMAND ${CMAKE_COMMAND} -E echo "[xtend] Strip  ${_xe_abs} -> ${_xb}"
        COMMAND xobjdump --strip -o "${_xb}" "${_xe_abs}"
        COMMAND ${CMAKE_COMMAND} -E echo "[xtend] Split  ${_xb}"
        COMMAND xobjdump --split "${_xb}"
        COMMAND ${CMAKE_COMMAND} -E echo "[xtend] Copy   raw image -> internal blob"
        COMMAND ${CMAKE_COMMAND} -E copy "${_raw_image}" "${_internal_blob}"
        COMMAND ${CMAKE_COMMAND} -E echo "[xtend] Export internal blob -> ${_external_blob}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_internal_blob}" "${_external_blob}"
        WORKING_DIRECTORY "${_work_dir}"
        DEPENDS ${_deps}
        BYPRODUCTS "${_xb}" "${_raw_image}" "${_external_blob}"
        COMMENT "Generating ${_blob_name} (build dir intermediates, final in bin)"
        VERBATIM
    )

    # Unique target for this blob (avoid name clashes if multiple)
    add_custom_target(blob_${_base} ALL DEPENDS "${_internal_blob}")

    # Expose both paths if caller wants them
    set(XTEND_BLOB_INTERNAL "${_internal_blob}" PARENT_SCOPE)
    set(XTEND_BLOB          "${_external_blob}" PARENT_SCOPE)
endfunction()

# Capture (absolute) location of utils once
if(NOT DEFINED XTND_UTILS_DIR)
    get_filename_component(XTND_UTILS_DIR "${CMAKE_CURRENT_LIST_DIR}" REALPATH)
endif()

# Default toggle (respect -D overrides)
if(NOT DEFINED XTEND_ENABLE_LIBC_LOCK_INIT)
    set(XTEND_ENABLE_LIBC_LOCK_INIT ON CACHE BOOL
        "Auto-init libc hardware lock in all xtend plugins")
endif()

function(xtnd_inject_common_runtime)
    if(NOT XTEND_ENABLE_LIBC_LOCK_INIT)
        return()
    endif()

    if(NOT DEFINED APP_C_SRCS)
        set(APP_C_SRCS "")
    endif()

    # Absolute path to the runtime source
    set(_runtime_abs "${XTND_UTILS_DIR}/runtime/xtnd_lock_init.c")
    if(NOT EXISTS "${_runtime_abs}")
        message(FATAL_ERROR "xtnd_inject_common_runtime: missing ${_runtime_abs}")
    endif()

    # Compute a path RELATIVE to the plugin CMakeLists directory
    file(RELATIVE_PATH _runtime_rel "${CMAKE_CURRENT_LIST_DIR}" "${_runtime_abs}")

    # Force using the relative path (avoid absolute to stop xcommon re-prefixing)
    if(_runtime_rel STREQUAL "" OR _runtime_rel MATCHES "^/")
        message(FATAL_ERROR "Unexpected relative path result: '${_runtime_rel}'")
    endif()

    list(FIND APP_C_SRCS "${_runtime_rel}" _have)
    if(_have EQUAL -1)
        list(APPEND APP_C_SRCS "${_runtime_rel}")
        set(APP_C_SRCS "${APP_C_SRCS}" PARENT_SCOPE)
    endif()
endfunction()

macro(XTND_REGISTER_APP)
    xtnd_inject_common_runtime()
    XMOS_REGISTER_APP()
endmacro()

