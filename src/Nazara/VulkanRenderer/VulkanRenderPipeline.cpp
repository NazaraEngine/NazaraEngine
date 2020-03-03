// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderPipeline.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/VulkanRenderer/VulkanShaderStage.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <cassert>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanRenderPipeline::VulkanRenderPipeline(Vk::DeviceHandle device, RenderPipelineInfo pipelineInfo) :
	m_device(std::move(device)),
	m_pipelineInfo(std::move(pipelineInfo))
	{
	}

	std::vector<VkPipelineColorBlendAttachmentState> VulkanRenderPipeline::BuildColorBlendAttachmentStateList(const RenderPipelineInfo& pipelineInfo)
	{
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendStates;

		VkPipelineColorBlendAttachmentState colorBlendState = colorBlendStates.emplace_back();
		colorBlendState.blendEnable = pipelineInfo.blending;
		colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; //< TODO

		if (pipelineInfo.blending)
		{
			//TODO
			/*switch (pipelineInfo.dstBlend)
			{
				blendState.dstAlphaBlendFactor
			}*/
		}
		else
		{
			colorBlendState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendState.colorBlendOp        = VK_BLEND_OP_ADD;
			colorBlendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendState.alphaBlendOp        = VK_BLEND_OP_ADD;
		}

		return colorBlendStates;
	}

	VkPipelineColorBlendStateCreateInfo VulkanRenderPipeline::BuildColorBlendInfo(const RenderPipelineInfo& pipelineInfo, const std::vector<VkPipelineColorBlendAttachmentState>& attachmentState)
	{
		VkPipelineColorBlendStateCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		createInfo.attachmentCount = std::uint32_t(attachmentState.size());
		createInfo.pAttachments = attachmentState.data();

		return createInfo;
	}

	VkPipelineDepthStencilStateCreateInfo VulkanRenderPipeline::BuildDepthStencilInfo(const RenderPipelineInfo& pipelineInfo)
	{
		VkPipelineDepthStencilStateCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		createInfo.depthTestEnable = pipelineInfo.depthBuffer;
		createInfo.depthWriteEnable = pipelineInfo.depthWrite;
		createInfo.depthCompareOp = ToVulkan(pipelineInfo.depthCompare);
		createInfo.stencilTestEnable = pipelineInfo.stencilTest;
		createInfo.front = BuildStencilOp(pipelineInfo, true);
		createInfo.back  = BuildStencilOp(pipelineInfo, false);

		return createInfo;
	}

	VkPipelineDynamicStateCreateInfo VulkanRenderPipeline::BuildDynamicStateInfo(const RenderPipelineInfo& pipelineInfo, const std::vector<VkDynamicState>& dynamicStates)
	{
		VkPipelineDynamicStateCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		createInfo.dynamicStateCount = std::uint32_t(dynamicStates.size());
		createInfo.pDynamicStates = dynamicStates.data();

		return createInfo;
	}

	std::vector<VkDynamicState> VulkanRenderPipeline::BuildDynamicStateList(const RenderPipelineInfo& pipelineInfo)
	{
		return { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	}

	VkPipelineInputAssemblyStateCreateInfo VulkanRenderPipeline::BuildInputAssemblyInfo(const RenderPipelineInfo& pipelineInfo)
	{
		VkPipelineInputAssemblyStateCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		createInfo.topology = ToVulkan(pipelineInfo.primitiveMode);

		return createInfo;
	}

	VkPipelineRasterizationStateCreateInfo VulkanRenderPipeline::BuildRasterizationInfo(const RenderPipelineInfo& pipelineInfo)
	{
		VkPipelineRasterizationStateCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		createInfo.polygonMode = ToVulkan(pipelineInfo.faceFilling);
		createInfo.cullMode = ToVulkan(pipelineInfo.cullingSide);
		createInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; //< TODO
		createInfo.lineWidth = pipelineInfo.lineWidth;

		return createInfo;
	}

	VkPipelineViewportStateCreateInfo VulkanRenderPipeline::BuildViewportInfo(const RenderPipelineInfo& pipelineInfo)
	{
		VkPipelineViewportStateCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		createInfo.scissorCount = createInfo.viewportCount = 1; //< TODO

		return createInfo;
	}

	VkStencilOpState VulkanRenderPipeline::BuildStencilOp(const RenderPipelineInfo& pipelineInfo, bool front)
	{
		const auto& pipelineStencil = (front) ? pipelineInfo.stencilFront : pipelineInfo.stencilBack;

		VkStencilOpState stencilStates;
		stencilStates.compareMask = pipelineStencil.compareMask;
		stencilStates.compareOp = ToVulkan(pipelineStencil.compare);
		stencilStates.depthFailOp = ToVulkan(pipelineStencil.depthFail);
		stencilStates.failOp = ToVulkan(pipelineStencil.fail);
		stencilStates.passOp = ToVulkan(pipelineStencil.pass);
		stencilStates.reference = pipelineStencil.reference;
		stencilStates.writeMask = pipelineStencil.writeMask;

		return stencilStates;
	}

	std::vector<VkPipelineShaderStageCreateInfo> VulkanRenderPipeline::BuildShaderStageInfo(const RenderPipelineInfo& pipelineInfo)
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;

		for (auto&& stagePtr : pipelineInfo.shaderStages)
		{
			Nz::VulkanShaderStage& vulkanStage = *static_cast<Nz::VulkanShaderStage*>(stagePtr.get());

			VkPipelineShaderStageCreateInfo& createInfo = shaderStageCreateInfos.emplace_back();
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			createInfo.module = vulkanStage.GetHandle();
			createInfo.pName = "main";
			createInfo.stage = ToVulkan(vulkanStage.GetStageType());
		}

		return shaderStageCreateInfos;
	}

	auto VulkanRenderPipeline::BuildCreateInfo(const RenderPipelineInfo& pipelineInfo) -> CreateInfo
	{
		CreateInfo createInfo = {};
		createInfo.stateData = std::make_unique<CreateInfo::StateData>();

		createInfo.colorBlendAttachmentState = BuildColorBlendAttachmentStateList(pipelineInfo);
		createInfo.dynamicStates = BuildDynamicStateList(pipelineInfo);
		createInfo.shaderStages = BuildShaderStageInfo(pipelineInfo);

		createInfo.stateData->colorBlendState = BuildColorBlendInfo(pipelineInfo, createInfo.colorBlendAttachmentState);
		createInfo.stateData->depthStencilState = BuildDepthStencilInfo(pipelineInfo);
		createInfo.stateData->dynamicState = BuildDynamicStateInfo(pipelineInfo, createInfo.dynamicStates);
		createInfo.stateData->inputAssemblyState = BuildInputAssemblyInfo(pipelineInfo);
		createInfo.stateData->rasterizationState = BuildRasterizationInfo(pipelineInfo);
		createInfo.stateData->viewportState = BuildViewportInfo(pipelineInfo);

		createInfo.pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.pipelineInfo.stageCount = std::uint32_t(createInfo.shaderStages.size());
		createInfo.pipelineInfo.pStages = createInfo.shaderStages.data();

		createInfo.pipelineInfo.pColorBlendState = createInfo.colorBlendAttachmentState.data();
		createInfo.pipelineInfo.

		return createInfo;
	}
}
