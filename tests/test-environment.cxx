//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <annex-lib/environment.hxx>

namespace test
{
    TEST_CASE("test.annex-lib.environment")
    {
        auto result = al::environment_get("PATH");
        REQUIRE(result.has_value());
    }
} // namespace test
