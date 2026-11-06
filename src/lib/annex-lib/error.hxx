//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <system_error>

namespace al
{
    /// The possible errors relating to the reading or writing of PAK files.
    enum class error_code
    {
        none,                    ///< No error has occurred.
        process_already_started, ///< The process has already been started and cannot be started again.
        process_not_started      ///< The process has not been started.
    };

    /// Create an error code.
    /// \param code The pak-io related error code.
    /// \return A valid error code.
    [[nodiscard]] auto make_error_code(error_code const code) -> std::error_code;
} // namespace al
