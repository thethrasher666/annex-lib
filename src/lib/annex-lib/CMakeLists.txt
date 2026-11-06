#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Configure files.
configure_file("version.hxx.in" "version.hxx")

# Define the target.
add_library(annex-lib STATIC
    "error.cxx"
    $<$<BOOL:${AL_BUILD_GPU}>:gpu.cxx>
    "process.cxx"
)

# Set the target's include directories.
target_include_directories(annex-lib
    PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}/.."
        "${CMAKE_CURRENT_SOURCE_DIR}/.."

    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}"
)

# Set the target's dependencies.
target_link_libraries(annex-lib
    PRIVATE
        annex-lib::platform
        annex-lib::vulkan
)

# Add alias library.
add_library(annex-lib::annex-lib ALIAS annex-lib)

# Add the platform-specific implementations.
add_subdirectory("posix")
add_subdirectory("vulkan")
add_subdirectory("win32")
