#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

include("FetchContent")

# Catch2.
FetchContent_Declare(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.15.2
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Catch2)

# Doxygen.
if(AL_BUILD_DOCS)
    find_package(Doxygen REQUIRED)
endif()

# Vulkan SDK.
if(AL_BUILD_GPU)
    find_package(Python COMPONENTS Interpreter)
    find_program(SLANG_EXECUTABLE slangc PATHS ENV{VULKAN_SDK})
    find_package(Vulkan REQUIRED)
endif(AL_BUILD_GPU)
