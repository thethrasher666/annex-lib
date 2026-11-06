//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "bootstrap.hxx"
#include "version.hxx"

#include <print>
#include <set>
#include <sstream>

namespace al
{
    auto create_logical_device(vk::PhysicalDevice const& physicalDevice, QueueFamilies const& queueFamilies) -> vk::Device
    {
        std::set<uint32_t>                     uniqueFamilies{ queueFamilies.compute };
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;
        const std::vector<float>               priority = { 1.0f };

        for (uint32_t family : uniqueFamilies)
        {
            queueCreateInfo.push_back(vk::DeviceQueueCreateInfo{ {}, family, priority });
        }

        vk::PhysicalDeviceVulkan13Features vk13Enable{};
        vk13Enable.setSynchronization2(true);

        vk::PhysicalDeviceVulkan14Features vk14Enable{};
        vk14Enable.setPushDescriptor(true);

        vk::StructureChain<vk::DeviceCreateInfo, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceVulkan14Features> createInfoChain{
            vk::DeviceCreateInfo{ {}, queueCreateInfo }, vk13Enable, vk14Enable
        };

        return physicalDevice.createDevice(createInfoChain.get<vk::DeviceCreateInfo>());
    }

    auto find_queue_family(vk::PhysicalDevice const& device, vk::QueueFlagBits const bit) -> std::optional<uint32_t>
    {
        auto const queueFamilies = device.getQueueFamilyProperties();

        for (uint32_t i = 0; i < queueFamilies.size(); ++i)
        {
            if (queueFamilies[i].queueFlags & bit)
            {
                return i;
            }
        }

        return std::nullopt;
    }

#if !defined(NDEBUG)
    auto init_debug_utils_messenger(vk::Instance const& instance) -> vk::DebugUtilsMessengerEXT
    {
        vk::DebugUtilsMessengerCreateInfoEXT const info({},
                                                        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
                                                        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
                                                        validation_callback);
        return instance.createDebugUtilsMessengerEXT(info, nullptr, vk::detail::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr));
    }
#endif // #if !defined(NDEBUG)

    auto init_instance() -> vk::Instance
    {
        std::vector<char const*> layers
        {
#if !defined(NDEBUG)
            "VK_LAYER_KHRONOS_validation"
#endif // #if !defined(NDEBUG)
        };

        std::vector<char const*> extensions
        {
#if !defined(NDEBUG)
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif // #if !defined(NDEBUG)
        };

        vk::InstanceCreateFlags flags{};
        vk::ApplicationInfo     appCreateInfo(version::name().data(), version::ordinal(), version::name().data(), version::ordinal(), VK_API_VERSION_1_4);
        vk::InstanceCreateInfo  instanceCreateInfo(flags, &appCreateInfo, layers, extensions);
        return vk::createInstance(instanceCreateInfo);
    }

    auto pick_physical_device(vk::Instance const& instance, QueueFamilies* queueFamilies) -> std::optional<vk::PhysicalDevice>
    {
        auto const devices = instance.enumeratePhysicalDevices();

        if (devices.empty())
        {
            return std::nullopt;
        }

        for (auto const& device : devices)
        {
            if (auto const family = find_queue_family(device, vk::QueueFlagBits::eCompute); family)
            {
                queueFamilies->compute = *family;
                return device;
            }
        }

        return std::nullopt;
    }

#if !defined(NDEBUG)
    vk::Bool32 validation_callback(vk::DebugUtilsMessageSeverityFlagBitsEXT      severity,
                                   vk::DebugUtilsMessageTypeFlagsEXT             types,
                                   vk::DebugUtilsMessengerCallbackDataEXT const* data,
                                   void* /*userData*/)
    {
        std::stringstream ss;

        ss << vk::to_string(severity) << ": " << vk::to_string(types) << ":\n";

        ss << "\tmessageIDName   = <" << data->pMessageIdName << ">\n";
        ss << "\tmessageIdNumber = " << data->messageIdNumber << "\n";
        ss << "\tmessage         = <" << data->pMessage << ">\n";

        if (0 < data->queueLabelCount)
        {
            ss << "\tQueue Labels:\n";

            std::span arr{ data->pQueueLabels, data->queueLabelCount };
            for (auto const& label : arr)
            {
                ss << "\t\tlabelName = <" << label.pLabelName << ">\n";
            }
        }

        if (0 < data->cmdBufLabelCount)
        {
            ss << "\tCommandBuffer Labels:\n";

            std::span arr{ data->pCmdBufLabels, data->cmdBufLabelCount };
            for (auto const& label : arr)
            {
                ss << "\t\tlabelName = <" << label.pLabelName << ">\n";
            }
        }

        if (0 < data->objectCount)
        {
            ss << "\tObjects:\n";

            std::span arr{ data->pObjects, data->objectCount };

            for (auto const& object : arr)
            {
                ss << "\t\tobjectType   = " << vk::to_string(object.objectType) << "\n";
                ss << "\t\tobjectHandle = " << object.objectHandle << "\n";

                if (object.pObjectName)
                {
                    ss << "\t\tobjectName   = <" << object.pObjectName << ">\n";
                }
            }
        }

        auto const output = ss.str();

        if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
        {
            std::print("Critical: {}\n", output);
        }

        if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
        {
            std::print("Warning: {}\n", output);
        }

        if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
        {
            std::print("Information: {}\n", output);
        }

        if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose)
        {
            std::print("Debug: {}\n", output);
        }

        return vk::True;
    }
#endif // #if !defined(NDEBUG)

} // namespace al
