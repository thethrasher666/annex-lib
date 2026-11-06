//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "gpu.hxx"
#include "vulkan/compute.hxx"
#include "vulkan/shaders.hxx"
#include "vulkan/util.hxx"

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

    auto gpu_radix_sort(gpu_context* context, std::span<int32_t> const& dataset) -> bool
    {
        return context->impl()->radix_sort(dataset);
    }
} // namespace al
