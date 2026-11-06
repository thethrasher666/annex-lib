// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "compute.hxx"
#include "bootstrap.hxx"
#include "util.hxx"

#include "shaders/double_up.spv.hxx" // Generated.

#include <memory>

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

    auto compute_gpu::op_double_up(std::span<uint32_t> const& dataset) -> bool
    {
        auto const  scratch = std::make_unique<ScratchBuffer>(_physicalDevice, _device, dataset.size_bytes());
        void* const mapped = scratch->upload<uint32_t>(dataset);

        auto pipeline = std::make_unique<ComputePipeline>(_device, double_up_spv, double_up_spv_entry_point);
        auto const elementCount = static_cast<uint32_t>(dataset.size());

        auto const& cmd = command_buffer();
        cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        cmd.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline->pipeline());

        vk::DescriptorBufferInfo bufInfo(scratch->buffer(), 0, vk::WholeSize);
        vk::WriteDescriptorSet   write({}, 0, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &bufInfo);
        cmd.pushDescriptorSet(vk::PipelineBindPoint::eCompute, pipeline->pipeline_layout(), 0, write);
        cmd.pushConstants<uint32_t>(pipeline->pipeline_layout(), vk::ShaderStageFlagBits::eCompute, 0, elementCount);
        cmd.dispatch((elementCount + 63) / 64, 1, 1);

        vk::MemoryBarrier2 barrier(vk::PipelineStageFlagBits2::eComputeShader, // Make shader writes visible to the host.
                                   vk::AccessFlagBits2::eShaderStorageWrite,
                                   vk::PipelineStageFlagBits2::eHost,
                                   vk::AccessFlagBits2::eHostRead);
        cmd.pipelineBarrier2(vk::DependencyInfo({}, barrier));
        cmd.end();

        auto const result = submit();
        if (result)
        {
            scratch->download<uint32_t>(mapped, dataset);
        }

        return result;
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
