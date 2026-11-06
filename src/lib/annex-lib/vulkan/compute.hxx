// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <span>
#include <vulkan/vulkan.hpp>

namespace al
{
    /// A Vulkan implementation of a GPU context.
    class compute_gpu final
    {
    public:
        /// Constructor.
        compute_gpu();

        /// Destructor.
        ~compute_gpu();

        /// Get the Vulkan command buffer.
        /// \return A valid object.
        [[nodiscard]] auto command_buffer() const -> vk::CommandBuffer const&
        {
            return _cmd;
        }

        /// Perform a radix sort.
        /// \param dataset The data set to sort.
        /// \return true on success; false otherwise.
        auto radix_sort(std::span<int32_t> const& dataset) -> bool;

        /// Submit and wait for the job.
        /// \return true on success; false otherwise.
        [[nodiscard]] auto submit() const -> bool;

    private:
        /// A storage buffer and its backing memory.
        struct ScratchBuffer
        {
            vk::Buffer       buffer; ///< The buffer.
            vk::DeviceMemory memory; ///< The buffer's backing memory.
        };

        /// The pipeline (and supporting objects) used to run the Radix Sort shader.
        struct RadixSortPipeline
        {
            vk::DescriptorSetLayout setLayout; ///< The descriptor set layout.
            vk::PipelineLayout      layout;    ///< The pipeline layout.
            vk::ShaderModule        shader;    ///< The shader module.
            vk::Pipeline            pipeline;  ///< The compute pipeline.
        };

        /// Create a host-visible, host-coherent storage buffer.
        /// \param sizeBytes The size of the buffer, in bytes.
        /// \return The created buffer.
        [[nodiscard]] auto create_storage_buffer(vk::DeviceSize sizeBytes) -> ScratchBuffer;

        /// Destroy a storage buffer created by \ref create_storage_buffer.
        /// \param buffer The buffer to destroy.
        void destroy_storage_buffer(ScratchBuffer const& buffer) const;

        /// Create the pipeline used to run the Radix Sort shader.
        /// \return The created pipeline.
        [[nodiscard]] auto create_radix_sort_pipeline() -> RadixSortPipeline;

        /// Destroy a pipeline created by \ref create_radix_sort_pipeline.
        /// \param pipeline The pipeline to destroy.
        void destroy_radix_sort_pipeline(RadixSortPipeline const& pipeline) const;

        /// Record the Radix Sort dispatch into the command buffer.
        /// \param pipeline The pipeline to dispatch.
        /// \param buffer The buffer to sort.
        /// \param elementCount The number of elements in the buffer.
        void record_radix_sort(RadixSortPipeline const& pipeline, vk::Buffer buffer, uint32_t elementCount);

        /// Upload data into mapped, host-visible memory.
        /// \param memory The memory to map.
        /// \param data The data to upload.
        /// \return The mapped pointer.
        template <typename T>
        [[nodiscard]] auto upload(vk::DeviceMemory memory, std::span<T const> data) -> void*;

        /// Download data from mapped, host-visible memory.
        /// \param mapped The mapped pointer.
        /// \param data Where to copy the downloaded data.
        template <typename T>
        void download(void* mapped, std::span<T> data) const;

    private:
        vk::Instance               _instance;                  ///< The Vulkan instance.
        vk::DebugUtilsMessengerEXT _debugUtilsMessenger;       ///< The Vulkan debug utils messenger.
        vk::PhysicalDevice         _physicalDevice;            ///< The Vulkan physical device.
        vk::Device                 _device;                    ///< The Vulkan logical device.
        vk::Queue                  _computeQueue;              ///< The Vulkan compute queue.
        uint32_t                   _computeQueueFamilyIndex{}; ///< The Vulkan compute queue family index.
        vk::CommandPool            _pool;                      ///< The command pool.
        vk::CommandBuffer          _cmd;                       ///< A command buffer.
        vk::Fence                  _fence;                     ///< A fence.
    };
} // namespace al
