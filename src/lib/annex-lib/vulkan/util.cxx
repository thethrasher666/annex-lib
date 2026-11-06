// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "util.hxx"

namespace al
{
    auto find_memory_type(vk::PhysicalDevice phys, uint32_t typeBits, vk::MemoryPropertyFlags wanted) -> std::optional<uint32_t>
    {
        auto props = phys.getMemoryProperties();
        for (uint32_t i = 0; i < props.memoryTypeCount; ++i)
        {
            if ((typeBits & (1u << i)) && (props.memoryTypes[i].propertyFlags & wanted) == wanted)
            {
                return i;
            }
        }

        return std::nullopt;
    }

} // namespace al
