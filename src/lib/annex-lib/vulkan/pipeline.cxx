// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

#include "pipeline.hxx"

namespace al
{
    ComputePipeline::ComputePipeline(vk::Device const& device, std::vector<uint32_t> const& shaderCode, std::string const& entryPoint) : _device(device)
    {
        vk::DescriptorSetLayoutBinding binding(0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute);
        _setLayout = _device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlagBits::ePushDescriptor, binding));
        vk::PushConstantRange   pcRange(vk::ShaderStageFlagBits::eCompute, 0, sizeof(uint32_t));
        _layout = _device.createPipelineLayout(vk::PipelineLayoutCreateInfo({}, _setLayout, pcRange));
        _shader = _device.createShaderModule(vk::ShaderModuleCreateInfo({}, shaderCode));

        vk::PipelineShaderStageCreateInfo stage({}, vk::ShaderStageFlagBits::eCompute, _shader, entryPoint.data());
        _pipeline = _device.createComputePipeline({}, vk::ComputePipelineCreateInfo({}, stage, _layout)).value;
    }

    ComputePipeline::~ComputePipeline()
    {
        _device.destroyPipeline(_pipeline);
        _device.destroyShaderModule(_shader);
        _device.destroyPipelineLayout(_layout);
        _device.destroyDescriptorSetLayout(_setLayout);
    }
} // namespace al
