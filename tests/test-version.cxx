//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <annex-lib/version.hxx>

namespace test
{
    TEST_CASE("test.annex-lib.version")
    {
        REQUIRE(al::version::major() == 0);
        REQUIRE(al::version::minor() == 0);
        REQUIRE(al::version::patch() == 1);
        REQUIRE(al::version::tweak() == 0);
    }
} // namespace test
