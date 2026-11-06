// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace al
{
    /// Get the shader for the Radix Sort algorithm.
    /// \param device The Vulkan device.
    /// \return The compiled shader byte code.
    [[nodiscard]] auto gpu_radix_sort_shader(vk::Device const& device) -> vk::ShaderModule;
} // namespace al
