//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "error.hxx"

namespace al::error
{
    namespace
    {
        /// Custom error category for error codes.
        class ErrorCategory final : public std::error_category
        {
        public:
            /// Inherited from std::error_category.
            [[nodiscard]] auto name() const noexcept -> char const* final
            {
                return "proc-lib::category";
            }

            /// Inherited from std::error_category.
            [[nodiscard]] auto message(int32_t value) const -> std::string final
            {
                auto const errorCode{ static_cast<error_code>(value) };

                switch (errorCode)
                {
                case error_code::none:
                    return "No error has occurred.";

                case error_code::process_already_started:
                    return "The process has already been started.";

                case error_code::process_not_started:
                    return "The process has not been started.";
                }

                return "No error has occurred.";
            }
        };

        [[nodiscard]] auto category() -> std::error_category const&
        {
            static ErrorCategory instance;
            return instance;
        }
    } // namespace

    auto make_error_code(error_code const code) -> std::error_code
    {
        return std::error_code(static_cast<int32_t>(code), category());
    }
} // namespace al::error
