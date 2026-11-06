#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

if (NOT AL_BUILD_GPU)
    add_library(annex-lib.vulkan INTERFACE)
    add_library(annex-lib::vulkan ALIAS annex-lib.vulkan)
    return()
endif()

# Compile a single Slang shader and attach it to a target.
#
# Each invocation handles exactly one source file, so each shader chooses its
# own entry point, target format and flags. Call the function once per shader.
#
#   TARGET       - the target the compiled shader is attached to.
#   SOURCE       - the .slang file to compile, relative to the current source directory.
#   ENTRY_POINT  - the entry point to compile. When omitted, slangc uses the
#                  [shader(...)] attributes in the source to find the entry points.
#   OUTPUT_NAME  - the name of the compiled artefact. Defaults to the source file
#                  name with the extension replaced by the target format's.
#                  Give an explicit name when compiling one source more than once.
#   OUTPUT_DIR   - the directory the artefact is written to.
#                  Defaults to "${CMAKE_CURRENT_BINARY_DIR}/shaders".
#   TARGET_FORMAT- the slangc target format. Defaults to "spirv".
#   INCLUDE_DIRS - directories searched for imported modules.
#   FLAGS        - additional flags passed through to slangc.
function(target_slang_shader TARGET)
    set(optionKeywords)

    set(oneValueKeywords
        SOURCE
        ENTRY_POINT
        OUTPUT_NAME
        OUTPUT_DIR
        TARGET_FORMAT
    )

    set(multiValueKeywords
        INCLUDE_DIRS
        FLAGS
    )

    cmake_parse_arguments(args
        "${optionKeywords}"
        "${oneValueKeywords}"
        "${multiValueKeywords}"
        ${ARGN}
    )

    if(NOT TARGET ${TARGET})
        message(FATAL_ERROR "target_slang_shader: '${TARGET}' is not a target.")
    endif()

    if(NOT args_SOURCE)
        message(FATAL_ERROR "target_slang_shader(${TARGET}): SOURCE is required.")
    endif()

    if(args_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "target_slang_shader(${TARGET}): unexpected arguments: ${args_UNPARSED_ARGUMENTS}")
    endif()

    # Apply the defaults.
    if(NOT args_OUTPUT_DIR)
        set(args_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
    endif()

    if(NOT args_TARGET_FORMAT)
        set(args_TARGET_FORMAT spirv)
    endif()

    if(NOT args_OUTPUT_NAME)
        get_filename_component(stem "${args_SOURCE}" NAME_WLE)

        # Pick the extension that matches the target format, falling back to
        # the format's own name for anything not listed here.
        set(ext "${args_TARGET_FORMAT}")
        if(args_TARGET_FORMAT STREQUAL "spirv")
            set(ext "spv")
        elseif(args_TARGET_FORMAT STREQUAL "spirv-asm")
            set(ext "spvasm")
        elseif(args_TARGET_FORMAT STREQUAL "dxil-asm")
            set(ext "dxil-asm")
        elseif(args_TARGET_FORMAT STREQUAL "metallib")
            set(ext "metallib")
        endif()

        set(args_OUTPUT_NAME "${stem}.${ext}")
    endif()

    # Build the include flags.
    set(inc_flags "")
    foreach(dir IN LISTS args_INCLUDE_DIRS)
        list(APPEND inc_flags -I "${dir}")
    endforeach()

    # Only pass -entry when one was requested; slangc falls back to the
    # [shader(...)] attributes in the source otherwise.
    set(entry_flags "")
    if(args_ENTRY_POINT)
        list(APPEND entry_flags -entry ${args_ENTRY_POINT})
    endif()

    set(input "${CMAKE_CURRENT_SOURCE_DIR}/${args_SOURCE}")
    set(output "${args_OUTPUT_DIR}/${args_OUTPUT_NAME}")
    set(header "${output}.hxx")
    set(depfile "${output}.d")

    # Compile the shader. The depfile lets the build system track the modules
    # imported by the source, so edits to them trigger a recompile.
    add_custom_command(
        OUTPUT
            "${output}"
            "${header}"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory "${args_OUTPUT_DIR}"
        COMMAND
            "${SLANG_EXECUTABLE}" "${input}" ${entry_flags} -target ${args_TARGET_FORMAT} ${inc_flags} ${args_FLAGS} -depfile "${depfile}" -o "${output}"
        COMMAND
            Python::Interpreter "${CMAKE_SOURCE_DIR}/scripts/bin-to-header.py" "${output}" "${header}"
        DEPENDS
            "${input}"
            "${SLANG_EXECUTABLE}"
        DEPFILE
            "${depfile}"
        COMMENT
            "Compiling shader: ${args_SOURCE} -> ${args_OUTPUT_NAME}"
        VERBATIM
        COMMAND_EXPAND_LISTS
    )

    # Hang the shader off an aggregate target, created on the first call, so
    # that building the owning target builds every shader attached to it.
    # Later calls append to that target rather than creating their own.
    if(NOT TARGET ${TARGET}_shaders)
        add_custom_target(${TARGET}_shaders)
        add_dependencies(${TARGET} ${TARGET}_shaders)
    endif()

    set_property(TARGET ${TARGET}_shaders APPEND PROPERTY SOURCES "${output}" "${header}")
endfunction()

# Define the target.
add_library(annex-lib.vulkan STATIC
    "bootstrap.cxx"
    "compute.cxx"
    "shaders.cxx"
    "util.cxx"
)

# Set the target's defaults.
al_set_target_defaults(annex-lib.vulkan)

# Set the compile definitions.
target_compile_definitions(annex-lib.vulkan
    PRIVATE
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
)

# Set the target's include directories.
target_include_directories(annex-lib.vulkan
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/.."
        "${CMAKE_CURRENT_BINARY_DIR}"
)

# Set the target's dependencies.
target_link_libraries(annex-lib.vulkan
    PUBLIC
        Vulkan::Vulkan
)

# Add the shaders.
target_slang_shader(annex-lib.vulkan
    SOURCE
        histogram.slang

    ENTRY_POINT
        computeMain
)

# Add alias library.
add_library(annex-lib::vulkan ALIAS annex-lib.vulkan)
