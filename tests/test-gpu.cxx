//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <annex-lib/gpu.hxx>

namespace test
{
    TEST_CASE("test.annex-lib.gpu")
    {
        auto gpu_context = std::make_unique<al::gpu_context>();

        REQUIRE(gpu_context->impl());
    }
} // namespace test
