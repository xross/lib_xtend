

function(build_xtend_blob xe_path)
    # Simplified blob builder for xtend plugins.
    # Args:
    #   xe_path                          Path to .xe OR generator expression ($<TARGET_FILE:...>)
    #   [TARGET tgt]                     Target producing the .xe (required if xe_path is a genex)
    #   [BLOB_FILENAME name]             Blob filename (default: xtend_blob.bin)
    #   [EXPORT_DIR dir]                 Directory to place exported/flashable blob (default: dir of literal .xe, else target binary dir)
    #   [NO_EXPORT]                      Do not export copy; only keep internal work-dir blob
    #   [GENERATE_HEADER]                Emit C header (default OFF)
    #   [HEADER_FILENAME name]           Header filename (default: xtend_blob.h)
    #   [HEADER_OUTPUT_DIR dir]          Directory for header (default: EXPORT_DIR if exporting else current binary dir)
    #   [ARRAY_SYMBOL name]              Symbol for data array (default: xtend_blob_bin)
    #   [EXPORT_XB]                       Also copy stripped <base>.xb beside the .xe (default OFF)
    # Behaviour:
    #   - Always creates an internal blob in <binary dir>/xtend_blob_<base>/<BLOB_FILENAME>
    #   - Unless NO_EXPORT, also copies that blob to EXPORT_DIR as the flashable artifact.
    #   - Header (if requested) is generated from the exported blob if present, else from internal blob.
    # Exposed (PARENT_SCOPE and target properties when a target can be identified):
    #   XTEND_BLOB_FILE          -> exported blob if exported, else internal blob
    #   XTEND_BLOB_INTERNAL_FILE -> internal work-dir blob path
    #   XTEND_BLOB_HEADER_FILE   -> header path (if generated)
    #   XTEND_BLOB_ARRAY_SYMBOL  -> array symbol (if header generated)

    set(options GENERATE_HEADER NO_EXPORT EXPORT_XB)
    set(oneValueArgs TARGET BLOB_FILENAME HEADER_FILENAME HEADER_OUTPUT_DIR ARRAY_SYMBOL EXPORT_DIR)
    cmake_parse_arguments(BXB "${options}" "${oneValueArgs}" "" ${ARGN})

    if(NOT xe_path)
        message(FATAL_ERROR "build_xtend_blob: xe_path argument missing")
    endif()

    # Detect generator expression
    set(_xe_is_genex OFF)
    if("${xe_path}" MATCHES "^\\$<")
        set(_xe_is_genex ON)
    endif()

    if(_xe_is_genex AND NOT BXB_TARGET)
        message(FATAL_ERROR "build_xtend_blob: TARGET must be supplied when xe_path is a generator expression")
    endif()

    # Determine base name and canonical xe source token
    if(_xe_is_genex)
        set(_base "${BXB_TARGET}")
        set(_xe_src "${xe_path}")
    else()
        get_filename_component(_xe_abs  "${xe_path}" ABSOLUTE)
        if(NOT EXISTS "${_xe_abs}")
            message(STATUS "[xtend] XE will be produced later: ${_xe_abs}")
        endif()
        get_filename_component(_base    "${_xe_abs}" NAME_WE)
        set(_xe_src "${_xe_abs}")
    endif()

    if(NOT _base)
        message(FATAL_ERROR "build_xtend_blob: unable to infer base name")
    endif()

    # Filenames
    set(_blob_filename "${BXB_BLOB_FILENAME}")
    if(NOT _blob_filename)
        set(_blob_filename "xtend_blob.bin")
    endif()

    # Optional export of stripped xb beside .xe (default OFF). Enable with EXPORT_XB.

    set(_work_dir "${CMAKE_CURRENT_BINARY_DIR}/xtend_blob_${_base}")
    file(MAKE_DIRECTORY "${_work_dir}")

    set(_xb            "${_work_dir}/${_base}.xb")
    set(_raw_image     "${_work_dir}/image_n0c0.bin")
    set(_internal_blob "${_work_dir}/${_blob_filename}")

    # Determine export directory (if exporting)
    set(_do_export ON)
    if(BXB_NO_EXPORT)
        set(_do_export OFF)
    endif()

    if(_do_export)
        if(BXB_EXPORT_DIR)
            set(_export_dir "${BXB_EXPORT_DIR}")
        elseif(NOT _xe_is_genex)
            # Literal path: export beside the .xe
            get_filename_component(_xe_dir "${_xe_src}" DIRECTORY)
            set(_export_dir "${_xe_dir}")
        elseif(BXB_TARGET AND TARGET "${BXB_TARGET}")
            # Use the actual runtime output directory of the target (.xe location) via generator expression
            set(_export_dir "$<TARGET_FILE_DIR:${BXB_TARGET}>")
        else()
            # Fallback
            set(_export_dir "${CMAKE_CURRENT_BINARY_DIR}")
        endif()
        # Only attempt to create if this is a literal path (not a generator expression)
        if(NOT _export_dir MATCHES "^\\$<")
            file(MAKE_DIRECTORY "${_export_dir}")
        endif()
        set(_exported_blob "${_export_dir}/${_blob_filename}")
        if(BXB_EXPORT_XB)
            set(_exported_xb   "${_export_dir}/${_base}.xb")
        endif()
    endif()

    # Header handling
    set(_do_header OFF)
    if(BXB_GENERATE_HEADER)
        set(_do_header ON)
        set(_header_filename "${BXB_HEADER_FILENAME}")
        if(NOT _header_filename)
            set(_header_filename "xtend_blob.h")
        endif()
        if(BXB_HEADER_OUTPUT_DIR)
            set(_header_output_dir "${BXB_HEADER_OUTPUT_DIR}")
        elseif(_do_export)
            set(_header_output_dir "${_export_dir}")
        else()
            set(_header_output_dir "${CMAKE_CURRENT_BINARY_DIR}")
        endif()
        file(MAKE_DIRECTORY "${_header_output_dir}")
        set(_array_symbol "${BXB_ARRAY_SYMBOL}")
        if(NOT _array_symbol)
            set(_array_symbol "xtend_blob_bin")
        endif()
        set(_header_path "${_header_output_dir}/${_header_filename}")
    endif()

    # Dependencies
    set(_deps)
    if(BXB_TARGET AND TARGET "${BXB_TARGET}")
        list(APPEND _deps "${BXB_TARGET}")
    endif()
    # For literal path, if file already exists add as dep (so rebuild when timestamp changes)
    if(NOT _xe_is_genex AND EXISTS "${_xe_src}")
        list(APPEND _deps "${_xe_src}")
    endif()

    # Outputs list (primary outputs for command)
    set(_outputs "${_internal_blob}")
    if(_do_header)
        list(APPEND _outputs "${_header_path}")
    endif()
    # BYPRODUCTS: only include literal paths (avoid generator expressions like $<TARGET_FILE_DIR:...>)
    set(_byproducts)
    if(_do_export)
        if(_exported_blob AND NOT _exported_blob MATCHES "^\\$<")
            list(APPEND _byproducts "${_exported_blob}")
        endif()
        if(_exported_xb AND NOT _exported_xb MATCHES "^\\$<")
            list(APPEND _byproducts "${_exported_xb}")
        endif()
    endif()
    list(APPEND _byproducts "${_xb}" "${_raw_image}")

    set(_comment "Generating ${_blob_filename}")
    if(_do_export)
        set(_comment "${_comment} (export)")
    endif()
    if(_do_header)
        set(_comment "${_comment} (+ header)")
    endif()

    # Command sequence
    set(_cmds
        COMMAND ${CMAKE_COMMAND} -E echo "[xtend] Strip  ${_xe_src} -> ${_xb}"
        COMMAND xobjdump --strip -o "${_xb}" "${_xe_src}"
        COMMAND ${CMAKE_COMMAND} -E echo "[xtend] Split  ${_xb}"
        COMMAND xobjdump --split "${_xb}"
        COMMAND ${CMAKE_COMMAND} -E echo "[xtend] Copy   raw image -> internal blob"
        COMMAND ${CMAKE_COMMAND} -E copy "${_raw_image}" "${_internal_blob}"
    )
    if(_do_export)
        list(APPEND _cmds
            COMMAND ${CMAKE_COMMAND} -E echo "[xtend] Export internal blob -> ${_exported_blob}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_internal_blob}" "${_exported_blob}"
        )
        if(_exported_xb)
            list(APPEND _cmds
                COMMAND ${CMAKE_COMMAND} -E echo "[xtend] Export stripped xb -> ${_exported_xb}"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_xb}" "${_exported_xb}"
            )
        endif()
    endif()
    if(_do_header)
        # Use exported blob when available, else internal
        if(_do_export)
            set(_header_src "${_exported_blob}")
        else()
            set(_header_src "${_internal_blob}")
        endif()
        list(APPEND _cmds
            COMMAND ${CMAKE_COMMAND} -E echo "[xtend] Header ${_header_src} -> ${_header_path} (array=${_array_symbol})"
            COMMAND /bin/sh -c "xxd -i -n ${_array_symbol} \"${_header_src}\" > \"${_header_path}\""
        )
    endif()

    add_custom_command(
        OUTPUT ${_outputs}
        ${_cmds}
        WORKING_DIRECTORY "${_work_dir}"
        DEPENDS ${_deps}
        BYPRODUCTS ${_byproducts}
        COMMENT "${_comment}"
        VERBATIM
    )

    add_custom_target(blob_${_base} ALL DEPENDS ${_outputs})

    # Determine public blob file reference
    if(_do_export)
        set(_public_blob "${_exported_blob}")
        if(_exported_xb)
            set(_public_xb   "${_exported_xb}")
        else()
            # Fall back to internal stripped xb path (not exported) for reference
            set(_public_xb   "${_xb}")
        endif()
    else()
        set(_public_blob "${_internal_blob}")
        set(_public_xb   "${_xb}")
    endif()

    # Export variables to caller
    set(XTEND_BLOB_INTERNAL_FILE "${_internal_blob}" PARENT_SCOPE)
    set(XTEND_BLOB_FILE          "${_public_blob}"  PARENT_SCOPE)
    set(XTEND_XB_FILE            "${_public_xb}"    PARENT_SCOPE)
    if(_do_header)
        set(XTEND_BLOB_HEADER_FILE "${_header_path}" PARENT_SCOPE)
        set(XTEND_BLOB_ARRAY_SYMBOL "${_array_symbol}" PARENT_SCOPE)
    endif()

    # Attach properties to target if possible
    if(BXB_TARGET AND TARGET "${BXB_TARGET}")
        set(_blob_owner "${BXB_TARGET}")
    elseif(TARGET "${_base}")
        set(_blob_owner "${_base}")
    endif()

    if(_blob_owner)
        set_property(TARGET "${_blob_owner}" PROPERTY XTEND_BLOB_FILE "${_public_blob}")
        set_property(TARGET "${_blob_owner}" PROPERTY XTEND_BLOB_INTERNAL_FILE "${_internal_blob}")
        set_property(TARGET "${_blob_owner}" PROPERTY XTEND_XB_FILE "${_public_xb}")
        if(_do_header)
            set_property(TARGET "${_blob_owner}" PROPERTY XTEND_BLOB_HEADER_FILE "${_header_path}")
            set_property(TARGET "${_blob_owner}" PROPERTY XTEND_BLOB_ARRAY_SYMBOL "${_array_symbol}")
        endif()
    endif()
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

