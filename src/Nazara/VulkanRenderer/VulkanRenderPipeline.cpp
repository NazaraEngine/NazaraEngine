// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderPipeline.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
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

	/*VkPipelineColorBlendAttachmentState VulkanRenderPipeline::BuildColorBlendAttachmentState(const RenderPipelineInfo& pipelineInfo)
	{
		VkPipelineColorBlendAttachmentState colorBlendStates;
		colorBlendStates.blendEnable = pipelineInfo.blending;
		colorBlendStates.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; //< TODO

		if (pipelineInfo.blending)
		{
			//TODO
			/*switch (pipelineInfo.dstBlend)
			{
				blendState.dstAlphaBlendFactor
			}*/
		/*}
		else
		{
			colorBlendStates.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendStates.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendStates.colorBlendOp        = VK_BLEND_OP_ADD;
			colorBlendStates.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendStates.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendStates.alphaBlendOp        = VK_BLEND_OP_ADD;
		}
		return colorBlendStates;
	}*/

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
			VkPipelineShaderStageCreateInfo& createInfo = shaderStageCreateInfos.emplace_back();
			//createInfo.
		}

		return shaderStageCreateInfos;
	}
}
