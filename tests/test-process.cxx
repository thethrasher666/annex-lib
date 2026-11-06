//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include <catch2/catch_test_macros.hpp>

#include <annex-lib/process.hxx>

namespace test
{
    TEST_CASE("test.annex-lib.process_id")
    {
        REQUIRE(al::process_id() != 0);
    }
} // namespace test
