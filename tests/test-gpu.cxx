//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <annex-lib/gpu.hxx>

#include <algorithm>
#include <numeric>

namespace test
{
    TEST_CASE("test.annex-lib.gpu", "[gpu]")
    {
        // Generate a list of sequential numbers.
        std::vector<uint32_t> numbers(1024);
        std::iota(numbers.begin(), numbers.end(), 1);

        // Take a copy for comparison.
        std::vector<uint32_t> before = numbers;

        auto gpu_context = std::make_unique<al::gpu_context>();

        auto const success = al::gpu_double_up(gpu_context.get(), numbers);
        REQUIRE(success);

        // Every element should have been doubled in place.
        std::vector<uint32_t> expected(before.size());
        std::ranges::transform(before, expected.begin(), [](uint32_t const value) { return value * 2; });

        REQUIRE(numbers == expected);
    }
} // namespace test
