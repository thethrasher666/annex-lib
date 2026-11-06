#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

if (NOT UNIX)
    return()
endif()

# Define the target.
add_library(annex-lib.platform STATIC
    "environment.cxx"
    "process.cxx"
)

# Set the target's defaults.
al_set_target_defaults(annex-lib.platform)

# Set the target's include directories.
target_include_directories(annex-lib.platform
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/.."

    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../.."
)

# Add alias library.
add_library(annex-lib::platform ALIAS annex-lib.platform)
