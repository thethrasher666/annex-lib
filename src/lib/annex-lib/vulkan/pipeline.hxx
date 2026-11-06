// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace al
{
    /// The pipeline (and supporting objects) used to run a compute shader.
    class ComputePipeline final
    {
    public:
        /// Constructor.
        /// \param device The Vulkan device.
        /// \param shaderCode The shader code.
        /// \param entryPoint The name of the entry point.
        ComputePipeline(vk::Device const& device, std::vector<uint32_t> const& shaderCode, std::string const& entryPoint);

        /// Destructor.
        ~ComputePipeline();

        /// Get the pipeline.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto pipeline() -> vk::Pipeline&
        {
            return _pipeline;
        }

        /// Get the pipeline layout.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto pipeline_layout() -> vk::PipelineLayout&
        {
            return _layout;
        }

    private:
        vk::Device const&       _device;    ///< The Vulkan device.
        vk::DescriptorSetLayout _setLayout; ///< The descriptor set layout.
        vk::PipelineLayout      _layout;    ///< The pipeline layout.
        vk::ShaderModule        _shader;    ///< The shader module.
        vk::Pipeline            _pipeline;  ///< The compute pipeline.
    };
} // namespace al
