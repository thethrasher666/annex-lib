// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace al
{
    /// Find a memory type.
    /// \param phys The physical Vulkan device.
    /// \param typeBits The type bits.
    /// \param wanted The type wanted.
    /// \return A memory type on success; nothing otherwise.
    [[nodiscard]] auto find_memory_type(vk::PhysicalDevice phys, uint32_t typeBits, vk::MemoryPropertyFlags wanted) -> std::optional<uint32_t>;
} // namespace al
