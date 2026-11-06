//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace al
{
    /// The queue families needed.
    struct QueueFamilies
    {
        uint32_t compute{}; // The compute queue family index.
    };

    /// Create a logical Vulkan device.
    /// \param physicalDevice The physical Vulkan device.
    /// \param queueFamilies The queue families.
    /// \return A Vulkan device.
    auto create_logical_device(vk::PhysicalDevice const& physicalDevice, QueueFamilies const& queueFamilies) -> vk::Device;

    /// Find a queue family.
    /// \param device The Vulkan device.
    /// \param bit The type of family to find.
    /// \return The index of the queue family if found; nothing otherwise.
    auto find_queue_family(vk::PhysicalDevice const& device, vk::QueueFlagBits const bit) -> std::optional<uint32_t>;

#if !defined(NDEBUG)
    /// Initialise the debug utils messenger for a Vulkan instance.
    /// \param instance The Vulkan instance.
    /// \return A Vulkan debug utils messenger.
    auto init_debug_utils_messenger(vk::Instance const& instance) -> vk::DebugUtilsMessengerEXT;
#endif // #if !defined(NDEBUG)

    /// Initialize a Vulkan instance.
    /// \return A Vulkan instance.
    auto init_instance() -> vk::Instance;

    /// Find a suitable physical device.
    /// \param instance The Vulkan instance.
    /// \param queueFamilies Will be completed on success;
    /// \return A Vulkan device if found; nothing otherwise.
    auto pick_physical_device(vk::Instance const& instance, QueueFamilies* queueFamilies) -> std::optional<vk::PhysicalDevice>;

#if !defined(NDEBUG)
    /// Vulkan debug utils messenger callback.
    /// \param severity The severity of the message.
    /// \param types The type of message.
    /// \param data Data relating to the message.
    /// \param userData User specified data.
    /// \return vk::True.
    [[nodiscard]] vk::Bool32 validation_callback(vk::DebugUtilsMessageSeverityFlagBitsEXT      severity,
                                                 vk::DebugUtilsMessageTypeFlagsEXT             types,
                                                 vk::DebugUtilsMessengerCallbackDataEXT const* data,
                                                 void*                                         userData);
#endif // #if !defined(NDEBUG)
} // namespace al
