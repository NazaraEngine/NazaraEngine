// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/VulkanComputePipeline.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/VulkanShaderModule.hpp>
#include <cassert>

namespace Nz
{
	VulkanComputePipeline::VulkanComputePipeline(VulkanDevice& device, ComputePipelineInfo pipelineInfo) :
	m_pipelineInfo(std::move(pipelineInfo))
	{
		if (!device.GetEnabledFeatures().computeShaders)
			throw std::runtime_error("compute shaders are not enabled on the device");

		VulkanShaderModule& vulkanModule = SafeCast<VulkanShaderModule&>(*m_pipelineInfo.shaderModule);
		const VulkanShaderModule::Stage* computeStage = nullptr;
		for (const auto& stage : vulkanModule.GetStages())
		{
			if (stage.stage != nzsl::ShaderStageType::Compute)
				continue;

			if (computeStage)
				throw std::runtime_error("multiple compute stages found in shader module");

			computeStage = &stage;
		}

		if (!computeStage)
			throw std::runtime_error("no compute stages found in shader module");

		VkComputePipelineCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		createInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		createInfo.stage.module = vulkanModule.GetHandle();
		createInfo.stage.pName = computeStage->name.data();
		createInfo.stage.stage = ToVulkan(computeStage->stage);

		VulkanRenderPipelineLayout& pipelineLayout = *SafeCast<VulkanRenderPipelineLayout*>(m_pipelineInfo.pipelineLayout.get());
		createInfo.layout = pipelineLayout.GetPipelineLayout();

		if (!m_pipeline.CreateCompute(device, createInfo))
			throw std::runtime_error("failed to create compute pipeline: " + TranslateVulkanError(m_pipeline.GetLastErrorCode()));
	}

	void VulkanComputePipeline::UpdateDebugName(std::string_view name)
	{
		m_pipeline.SetDebugName(name);
	}
}
