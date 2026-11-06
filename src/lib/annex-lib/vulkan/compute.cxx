// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "compute.hxx"

#include "bootstrap.hxx"
#include "shaders.hxx"
#include "util.hxx"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace al
{
    compute_gpu::compute_gpu()
    {
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
        _instance = init_instance();

        VULKAN_HPP_DEFAULT_DISPATCHER.init(_instance);
#if !defined(NDEBUG)
        _debugUtilsMessenger = init_debug_utils_messenger(_instance);
#endif // #if !defined(NDEBUG)

        QueueFamilies queueFamilies;
        if (auto const result = pick_physical_device(_instance, &queueFamilies); result)
        {
            _physicalDevice = result.value();
            _device = create_logical_device(_physicalDevice, queueFamilies);

            _computeQueueFamilyIndex = queueFamilies.compute;
            _computeQueue = _device.getQueue(_computeQueueFamilyIndex, 0);
        }

        _pool = _device.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eTransient, _computeQueueFamilyIndex));
        _cmd = _device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(_pool, vk::CommandBufferLevel::ePrimary, 1)).front();
        _fence = _device.createFence({});
    }

    /// Destructor.
    compute_gpu::~compute_gpu()
    {
        if (_device)
        {
            _device.waitIdle();

            _device.destroyFence(_fence);
            _device.destroyCommandPool(_pool); // frees _cmd

            _device.destroy();
        }

        if (_debugUtilsMessenger)
        {
            _instance.destroyDebugUtilsMessengerEXT(_debugUtilsMessenger, nullptr, vk::detail::DispatchLoaderDynamic(_instance, vkGetInstanceProcAddr));
        }

        _instance.destroy();
    }

    auto compute_gpu::radix_sort(std::span<int32_t> const& dataset) -> bool
    {
        auto const  scratch = create_storage_buffer(dataset.size_bytes());
        void* const mapped = upload<int32_t>(scratch.memory, dataset);

        auto const pipeline = create_radix_sort_pipeline();
        record_radix_sort(pipeline, scratch.buffer, static_cast<uint32_t>(dataset.size()));

        auto const result = submit();
        if (result)
        {
            download<int32_t>(mapped, dataset);
        }

        _device.unmapMemory(scratch.memory);
        destroy_radix_sort_pipeline(pipeline);
        destroy_storage_buffer(scratch);

        return result;
    }

    auto compute_gpu::create_storage_buffer(vk::DeviceSize sizeBytes) -> ScratchBuffer
    {
        auto             buffer = _device.createBuffer(vk::BufferCreateInfo({}, sizeBytes, vk::BufferUsageFlagBits::eStorageBuffer, vk::SharingMode::eExclusive));
        auto             req = _device.getBufferMemoryRequirements(buffer);
        auto const       memoryType = *find_memory_type(_physicalDevice, req.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        vk::DeviceMemory memory = _device.allocateMemory(vk::MemoryAllocateInfo(req.size, memoryType));

        _device.bindBufferMemory(buffer, memory, 0);

        return { buffer, memory };
    }

    void compute_gpu::destroy_storage_buffer(ScratchBuffer const& buffer) const
    {
        _device.freeMemory(buffer.memory);
        _device.destroyBuffer(buffer.buffer);
    }

    auto compute_gpu::create_radix_sort_pipeline() -> RadixSortPipeline
    {
        vk::DescriptorSetLayoutBinding binding(0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute);
        vk::DescriptorSetLayout setLayout = _device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlagBits::ePushDescriptor, binding));
        vk::PushConstantRange   pcRange(vk::ShaderStageFlagBits::eCompute, 0, sizeof(uint32_t));
        vk::PipelineLayout      pipelineLayout = _device.createPipelineLayout(vk::PipelineLayoutCreateInfo({}, setLayout, pcRange));
        vk::ShaderModule        shader = gpu_radix_sort_shader(_device);

        vk::PipelineShaderStageCreateInfo stage({}, vk::ShaderStageFlagBits::eCompute, shader, "computeMain");
        vk::Pipeline                      pipeline = _device.createComputePipeline({}, vk::ComputePipelineCreateInfo({}, stage, pipelineLayout)).value;

        return { setLayout, pipelineLayout, shader, pipeline };
    }

    void compute_gpu::destroy_radix_sort_pipeline(RadixSortPipeline const& pipeline) const
    {
        _device.destroyPipeline(pipeline.pipeline);
        _device.destroyShaderModule(pipeline.shader);
        _device.destroyPipelineLayout(pipeline.layout);
        _device.destroyDescriptorSetLayout(pipeline.setLayout);
    }

    void compute_gpu::record_radix_sort(RadixSortPipeline const& pipeline, vk::Buffer buffer, uint32_t elementCount)
    {
        auto const& cmd = command_buffer();
        cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        cmd.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline.pipeline);

        vk::DescriptorBufferInfo bufInfo(buffer, 0, vk::WholeSize);
        vk::WriteDescriptorSet   write({}, 0, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &bufInfo);
        cmd.pushDescriptorSet(vk::PipelineBindPoint::eCompute, pipeline.layout, 0, write);
        cmd.pushConstants<uint32_t>(pipeline.layout, vk::ShaderStageFlagBits::eCompute, 0, elementCount);
        cmd.dispatch((elementCount + 63) / 64, 1, 1);

        vk::MemoryBarrier2 barrier(vk::PipelineStageFlagBits2::eComputeShader, // Make shader writes visible to the host.
                                   vk::AccessFlagBits2::eShaderStorageWrite,
                                   vk::PipelineStageFlagBits2::eHost,
                                   vk::AccessFlagBits2::eHostRead);
        cmd.pipelineBarrier2(vk::DependencyInfo({}, barrier));
        cmd.end();
    }

    template <typename T>
    auto compute_gpu::upload(vk::DeviceMemory memory, std::span<T const> data) -> void*
    {
        void* mapped = _device.mapMemory(memory, 0, data.size_bytes());
        std::memcpy(mapped, data.data(), data.size_bytes());

        return mapped;
    }

    template <typename T>
    void compute_gpu::download(void* mapped, std::span<T> data) const
    {
        std::memcpy(data.data(), mapped, data.size_bytes());
    }

    auto compute_gpu::submit() const -> bool
    {
        vk::CommandBufferSubmitInfo cbInfo(_cmd);
        auto const&                 queue = _computeQueue;
        queue.submit2(vk::SubmitInfo2({}, {}, cbInfo, {}), _fence);

        if (_device.waitForFences(_fence, VK_TRUE, UINT64_MAX) != vk::Result::eSuccess)
        {
            return false;
        }

        return true;
    }
} // namespace al
