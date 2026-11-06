// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <span>
#include <vulkan/vulkan.hpp>

#include "pipeline.hxx"

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

        /// Doubles the value of each element in a container.
        /// \param dataset The data set to sort.
        /// \return true on success; false otherwise.
        [[nodiscard]] auto op_double_up(std::span<uint32_t> const& dataset) -> bool;

        /// Submit and wait for the job.
        /// \return true on success; false otherwise.
        [[nodiscard]] auto submit() const -> bool;

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
