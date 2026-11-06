//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "annex-lib/environment.hxx"

#include <memory>
#include <Windows.h>

namespace al
{
    [[nodiscard]] auto environment_get(std::string_view const name) -> std::optional<std::string>
    {
        DWORD size = GetEnvironmentVariableA(name.data(), nullptr, 0);

        if ((ERROR_ENVVAR_NOT_FOUND == size) || (0 == size))
        {
            return std::nullopt;
        }

        std::unique_ptr<char[]> buffer(new char[size]);
        GetEnvironmentVariableA(name.data(), buffer.get(), size);
        return std::string(buffer.get());
    }
} // namespace al
