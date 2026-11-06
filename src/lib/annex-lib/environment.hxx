//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace al
{
    /// Get the contents of the specified variable from the environment block of the calling process.
    /// \param name The name of the environment variable.
    /// \return The value of the environment variable on success; nothing otherwise.
    [[nodiscard]] auto environment_get(std::string_view const name) -> std::optional<std::string>;

} // namespace al
