#
# Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
add_executable(test_annex_lib
    "test-environment.cxx"
    $<$<BOOL:${AL_BUILD_GPU}>:test-gpu.cxx>
    "test-process.cxx"
    "test-version.cxx"
)

# Set the defaults.
al_set_target_defaults(test_annex_lib)

# Set the target's dependencies.
target_link_libraries(test_annex_lib
    PRIVATE
        annex-lib::annex-lib
        Catch2::Catch2WithMain
)

# Discover the tests.
catch_discover_tests(test_annex_lib)
