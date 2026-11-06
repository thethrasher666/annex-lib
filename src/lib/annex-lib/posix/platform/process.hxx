//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <annex-lib/process.hxx>

#include <cstdint>
#include <expected>
#include <sys/types.h>

namespace al
{
    /// The kind of raw exit status reported by the OS for a completed process.
    enum class platform_exit_kind
    {
        exited,  ///< The process ran to completion, or was force-stopped with an OS-reported exit code.
        signaled ///< The process was stopped by a signal, with no OS-reported exit code.
    };

    /// The raw exit status reported by the OS for a completed process. Deliberately does not know
    /// whether a stop was requested by the caller (terminate()/kill()) or not — only Process itself
    /// tracks that, so it can distinguish a requested stop from a crash.
    struct platform_exit_status
    {
        platform_exit_kind kind = platform_exit_kind::exited; ///< The kind of exit status reported by the OS.
        int32_t            value = -1;                        ///< The exit code if kind == exited; the signal number if kind == signaled.
    };

    /// A platform-specific process implementation.
    class platform_process final
    {
    public:
        /// Destructor.
        ~platform_process();

        /// Start the process.
        /// \param executablePath Fully qualified path of the exectuable to run.
        /// \param arguments The arguments to launch the process with.
        /// \return An error code, if any.
        [[nodiscard]] auto start(std::filesystem::path const& executablePath, std::vector<std::string> const& arguments) -> std::error_code;

        /// Ask the process to stop gracefully. See Process::terminate().
        /// \return An error code, if any.
        [[nodiscard]] auto terminate() -> std::error_code;

        /// Stop the process immediately. See Process::kill().
        /// \param exitCode Ignored; POSIX signals carry no exit code.
        /// \return An error code, if any.
        [[nodiscard]] auto kill(uint32_t const exitCode) -> std::error_code;

        /// Wait for the process to complete.
        /// \return The raw OS-reported exit status, or an error code.
        [[nodiscard]] auto wait() -> std::expected<platform_exit_status, std::error_code>;

    private:
        pid_t _processId{ -1 }; ///< The process ID of the child process.
    };
} // namespace al
