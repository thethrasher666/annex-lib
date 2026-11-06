//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "gpu.hxx"
#include "vulkan/compute.hxx"

namespace al
{
    gpu_context::gpu_context()
    {
        _gpu = std::make_unique<compute_gpu>();
    }

    gpu_context::~gpu_context()
    {
    }

    auto gpu_context::impl() -> compute_gpu*
    {
        return _gpu.get();
    }

    auto gpu_double_up(gpu_context* context, std::span<uint32_t> const& dataset) -> bool
    {
        return context->impl()->op_double_up(dataset);
    }
} // namespace al
