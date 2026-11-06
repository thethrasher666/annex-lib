//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "annex-lib/environment.hxx"

#include <stdlib.h>

namespace al
{
    [[nodiscard]] auto environment_get(std::string_view const name) -> std::optional<std::string>
    {
        if (char const* const result = getenv(name.data()))
        {
            return std::string(result);
        }

        return std::nullopt;
    }
} // namespace al
