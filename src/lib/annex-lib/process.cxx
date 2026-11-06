//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "process.hxx"
#include "error.hxx"
#include "platform/process.hxx"

namespace al
{
    process::process()
    {
    }

    process::~process()
    {
    }

    auto process::start(std::filesystem::path const& executablePath, std::vector<std::string> const& arguments) -> std::error_code
    {
        if (_platformProcess)
        {
            return make_error_code(error_code::process_already_started);
        }

        _platformProcess = std::make_unique<platform_process>();
        return _platformProcess->start(executablePath, arguments);
    }

    auto process::terminate() -> std::error_code
    {
        if (!_platformProcess)
        {
            return make_error_code(error_code::process_not_started);
        }

        auto const error = _platformProcess->terminate();

        if (!error)
        {
            _stopRequest = stop_request::terminate;
        }

        return error;
    }

    auto process::kill(uint32_t const exitCode) -> std::error_code
    {
        if (!_platformProcess)
        {
            return make_error_code(error_code::process_not_started);
        }

        auto const error = _platformProcess->kill(exitCode);

        if (!error)
        {
            _stopRequest = stop_request::kill;
        }

        return error;
    }

    auto process::wait() -> process_result
    {
        if (!_platformProcess)
        {
            return process_result{ .status = process_result::status::error, .message = "The process has not been started." };
        }

        auto const result = _platformProcess->wait();

        if (!result)
        {
            return process_result{ .status = process_result::status::error, .message = result.error().message() };
        }

        // Only an OS-reported "exited" status carries a meaningful exit code; a signal has none.
        auto const reportedExitCode = result->kind == platform_exit_kind::exited ? result->value : -1;

        // Whether we asked the process to stop takes priority over the raw OS status: on Windows a
        // forceful stop is reported the same way as a normal exit, so this is the only way to tell them apart.
        if (_stopRequest == stop_request::terminate)
        {
            return process_result{ .status = process_result::status::terminated, .exitCode = reportedExitCode };
        }

        if (_stopRequest == stop_request::kill)
        {
            return process_result{ .status = process_result::status::killed, .exitCode = reportedExitCode };
        }

        if (result->kind == platform_exit_kind::exited)
        {
            return process_result{
                .status = reportedExitCode == 0 ? process_result::status::success : process_result::status::non_zero_exit,
                .exitCode = reportedExitCode,
            };
        }

        return process_result{ .status = process_result::status::crashed };
    }
} // namespace al
