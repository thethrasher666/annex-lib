//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <memory>
#include <span>

namespace al
{
    class compute_gpu; ///< Forward declaration.

    /// Context for peforming actions with a GPU.
    class gpu_context final
    {
    public:
        /// Constructor.
        gpu_context();

        /// Destructor.
        ~gpu_context();

        /// Get the implementation.
        /// \return A valid object.
        [[nodiscard]] auto impl() -> compute_gpu*;

    private:
        std::unique_ptr<compute_gpu> _gpu;
    };

    /// Radix Sort is a linear sorting algorithm (for fixed length digit counts) that sorts elements by processing them digit by digit.
    /// \param context A GPU context.
    /// \param dataset The data set to sort.
    /// \return true on success; false otherwise.
    [[nodiscard]] auto gpu_radix_sort(gpu_context* context, std::span<int32_t> const& dataset) -> bool;
} // namespace al
