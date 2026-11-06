// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "util.hxx"

namespace al
{
    ScratchBuffer::ScratchBuffer(vk::PhysicalDevice const& physical, vk::Device const& device, vk::DeviceSize sizeBytes) : _device(device)
    {
        _buffer = _device.createBuffer(vk::BufferCreateInfo({}, sizeBytes, vk::BufferUsageFlagBits::eStorageBuffer, vk::SharingMode::eExclusive));

        auto       req = _device.getBufferMemoryRequirements(_buffer);
        auto const memoryType = *find_memory_type(physical, req.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

        _memory = _device.allocateMemory(vk::MemoryAllocateInfo(req.size, memoryType));
        _device.bindBufferMemory(_buffer, _memory, 0);
    }

    ScratchBuffer::~ScratchBuffer()
    {
        _device.freeMemory(_memory);
        _device.destroyBuffer(_buffer);
    }

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
