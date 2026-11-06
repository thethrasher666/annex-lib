// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "shaders.hxx"
#include "shaders/histogram.spv.hxx" // Generated.

namespace al
{
    auto gpu_radix_sort_shader(vk::Device const& device) -> vk::ShaderModule
    {
        return device.createShaderModule(vk::ShaderModuleCreateInfo({}, histogram_spv));
    }
} // namespace al
