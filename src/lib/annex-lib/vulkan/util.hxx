// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace al
{
    /// A storage buffer and its backing memory.
    class ScratchBuffer final
    {
    public:
        /// Constructor.
        /// \param physical The physical Vulkan device.
        /// \param device The Vulkan device.
        /// \param sizeBytes The size of the buffer, in bytes.
        explicit ScratchBuffer(vk::PhysicalDevice const& physical, vk::Device const& device, vk::DeviceSize sizeBytes);

        /// Destructor.
        ~ScratchBuffer();

        /// Get the buffer.
        /// \return A valid object.
        [[nodiscard]] auto buffer() -> vk::Buffer const&
        {
            return _buffer;
        }

        /// Download data from mapped, host-visible memory.
        /// \param mapped The mapped pointer.
        /// \param data Where to copy the downloaded data.
        template <typename T>
        void download(void* mapped, std::span<T> data) const
        {
            std::memcpy(data.data(), mapped, data.size_bytes());
            _device.unmapMemory(_memory);
        }

        /// Upload data into mapped, host-visible memory.
        /// \param data The data to upload.
        /// \return The mapped pointer.
        template <typename T>
        [[nodiscard]] auto upload(std::span<T const> data) -> void*
        {
            void* mapped = _device.mapMemory(_memory, 0, data.size_bytes());
            std::memcpy(mapped, data.data(), data.size_bytes());

            return mapped;
        }

    private:
        vk::Device const& _device; ///< The device.
        vk::Buffer        _buffer; ///< The buffer.
        vk::DeviceMemory  _memory; ///< The buffer's backing memory.
    };

    /// Find a memory type.
    /// \param phys The physical Vulkan device.
    /// \param typeBits The type bits.
    /// \param wanted The type wanted.
    /// \return A memory type on success; nothing otherwise.
    [[nodiscard]] auto find_memory_type(vk::PhysicalDevice phys, uint32_t typeBits, vk::MemoryPropertyFlags wanted) -> std::optional<uint32_t>;
} // namespace al
