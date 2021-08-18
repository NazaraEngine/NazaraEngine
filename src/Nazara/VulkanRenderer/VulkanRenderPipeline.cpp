// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderPipeline.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/VulkanShaderModule.hpp>
#include <cassert>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanRenderPipeline::VulkanRenderPipeline(VulkanDevice& device, RenderPipelineInfo pipelineInfo) :
	m_device(&device),
	m_pipelineInfo(std::move(pipelineInfo))
	{
		ValidatePipelineInfo(device, m_pipelineInfo);

		m_pipelineCreateInfo = BuildCreateInfo(m_pipelineInfo);
	}

	VkPipeline VulkanRenderPipeline::Get(const VulkanRenderPass& renderPass, std::size_t subpassIndex) const
	{
		const Vk::RenderPass& renderPassHandle = renderPass.GetRenderPass();

		// Use color attachment count as a key
		const auto& subpasses = renderPass.GetSubpassDescriptions();
		assert(subpassIndex < subpasses.size());

		std::size_t colorAttachmentCount = subpasses[subpassIndex].colorAttachment.size();

		std::pair<VkRenderPass, std::size_t> key = { renderPassHandle, colorAttachmentCount };

		if (auto it = m_pipelines.find(key); it != m_pipelines.end())
			return it->second;

		UpdateCreateInfo(colorAttachmentCount);

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = m_pipelineCreateInfo.pipelineInfo;
		pipelineCreateInfo.renderPass = renderPassHandle;

		Vk::Pipeline newPipeline;
		if (!newPipeline.CreateGraphics(*m_device, pipelineCreateInfo))
			return VK_NULL_HANDLE;

		auto it = m_pipelines.emplace(key, std::move(newPipeline)).first;
		return it->second;
	}

	std::vector<VkPipelineColorBlendAttachmentState> VulkanRenderPipeline::BuildColorBlendAttachmentStateList(const RenderPipelineInfo& pipelineInfo)
	{
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendStates;

		VkPipelineColorBlendAttachmentState& colorBlendState = colorBlendStates.emplace_back();
		colorBlendState.blendEnable = pipelineInfo.blending;
		if (pipelineInfo.colorWrite)
			colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; //< TODO
		else
			colorBlendState.colorWriteMask = 0;

		if (pipelineInfo.blending)
		{
			colorBlendState.srcColorBlendFactor = ToVulkan(pipelineInfo.blend.srcColor);
			colorBlendState.dstColorBlendFactor = ToVulkan(pipelineInfo.blend.dstColor);
			colorBlendState.colorBlendOp        = ToVulkan(pipelineInfo.blend.modeColor);
			colorBlendState.srcAlphaBlendFactor = ToVulkan(pipelineInfo.blend.srcAlpha);
			colorBlendState.dstAlphaBlendFactor = ToVulkan(pipelineInfo.blend.dstAlpha);
			colorBlendState.alphaBlendOp        = ToVulkan(pipelineInfo.blend.modeAlpha);
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

	VkPipelineColorBlendStateCreateInfo VulkanRenderPipeline::BuildColorBlendInfo(const RenderPipelineInfo& /*pipelineInfo*/, const std::vector<VkPipelineColorBlendAttachmentState>& attachmentState)
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

	VkPipelineDynamicStateCreateInfo VulkanRenderPipeline::BuildDynamicStateInfo(const RenderPipelineInfo& /*pipelineInfo*/, const std::vector<VkDynamicState>& dynamicStates)
	{
		VkPipelineDynamicStateCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		createInfo.dynamicStateCount = std::uint32_t(dynamicStates.size());
		createInfo.pDynamicStates = dynamicStates.data();

		return createInfo;
	}

	std::vector<VkDynamicState> VulkanRenderPipeline::BuildDynamicStateList(const RenderPipelineInfo& /*pipelineInfo*/)
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

	VkPipelineMultisampleStateCreateInfo VulkanRenderPipeline::BuildMultisampleInfo(const RenderPipelineInfo& /*pipelineInfo*/)
	{
		VkPipelineMultisampleStateCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.minSampleShading = 1.0f; //< TODO: Remove

		return createInfo;
	}

	VkPipelineRasterizationStateCreateInfo VulkanRenderPipeline::BuildRasterizationInfo(const RenderPipelineInfo& pipelineInfo)
	{
		VkPipelineRasterizationStateCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		createInfo.cullMode = (pipelineInfo.faceCulling) ? ToVulkan(pipelineInfo.cullingSide) : VK_CULL_MODE_NONE;
		createInfo.depthClampEnable = pipelineInfo.depthClamp;
		createInfo.frontFace = ToVulkan(pipelineInfo.frontFace);
		createInfo.lineWidth = pipelineInfo.lineWidth;
		createInfo.polygonMode = ToVulkan(pipelineInfo.faceFilling);

		return createInfo;
	}

	VkPipelineViewportStateCreateInfo VulkanRenderPipeline::BuildViewportInfo(const RenderPipelineInfo& /*pipelineInfo*/)
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

		for (auto&& stagePtr : pipelineInfo.shaderModules)
		{
			assert(stagePtr);

			Nz::VulkanShaderModule& vulkanModule = *static_cast<Nz::VulkanShaderModule*>(stagePtr.get());
			for (auto& stage : vulkanModule.GetStages())
			{
				VkPipelineShaderStageCreateInfo& createInfo = shaderStageCreateInfos.emplace_back();
				createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				createInfo.module = vulkanModule.GetHandle();
				createInfo.pName = stage.name.data();
				createInfo.stage = ToVulkan(stage.stage);
			}
		}

		return shaderStageCreateInfos;
	}

	std::vector<VkVertexInputAttributeDescription> VulkanRenderPipeline::BuildVertexAttributeDescription(const RenderPipelineInfo& pipelineInfo)
	{
		std::vector<VkVertexInputAttributeDescription> vertexAttributes;

		std::uint32_t locationIndex = 0;

		for (const auto& bufferData : pipelineInfo.vertexBuffers)
		{
			std::uint32_t binding = std::uint32_t(bufferData.binding);

			for (const auto& componentInfo : bufferData.declaration->GetComponents())
			{
				auto& bufferAttribute = vertexAttributes.emplace_back();
				bufferAttribute.binding = binding;
				bufferAttribute.location = locationIndex++;
				bufferAttribute.offset = std::uint32_t(componentInfo.offset);
				bufferAttribute.format = ToVulkan(componentInfo.type);
			}
		}

		return vertexAttributes;
	}

	std::vector<VkVertexInputBindingDescription> VulkanRenderPipeline::BuildVertexBindingDescription(const RenderPipelineInfo& pipelineInfo)
	{
		std::vector<VkVertexInputBindingDescription> vertexBindings;

		for (const auto& bufferData : pipelineInfo.vertexBuffers)
		{
			auto& bufferBinding = vertexBindings.emplace_back();
			bufferBinding.binding = std::uint32_t(bufferData.binding);
			bufferBinding.stride = std::uint32_t(bufferData.declaration->GetStride());
			bufferBinding.inputRate = ToVulkan(bufferData.declaration->GetInputRate());
		}

		return vertexBindings;
	}

	VkPipelineVertexInputStateCreateInfo VulkanRenderPipeline::BuildVertexInputInfo(const RenderPipelineInfo& /*pipelineInfo*/, const std::vector<VkVertexInputAttributeDescription>& vertexAttributes, const std::vector<VkVertexInputBindingDescription>& bindingDescriptions)
	{
		VkPipelineVertexInputStateCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		createInfo.vertexAttributeDescriptionCount = std::uint32_t(vertexAttributes.size());
		createInfo.pVertexAttributeDescriptions = vertexAttributes.data();

		createInfo.vertexBindingDescriptionCount = std::uint32_t(bindingDescriptions.size());
		createInfo.pVertexBindingDescriptions = bindingDescriptions.data();

		return createInfo;
	}

	auto VulkanRenderPipeline::BuildCreateInfo(const RenderPipelineInfo& pipelineInfo) -> CreateInfo
	{
		CreateInfo createInfo = {};
		createInfo.stateData = std::make_unique<CreateInfo::StateData>();

		createInfo.colorBlendAttachmentState = BuildColorBlendAttachmentStateList(pipelineInfo);
		createInfo.dynamicStates = BuildDynamicStateList(pipelineInfo);
		createInfo.shaderStages = BuildShaderStageInfo(pipelineInfo);
		createInfo.vertexAttributesDescription = BuildVertexAttributeDescription(pipelineInfo);
		createInfo.vertexBindingDescription = BuildVertexBindingDescription(pipelineInfo);

		createInfo.stateData->colorBlendState = BuildColorBlendInfo(pipelineInfo, createInfo.colorBlendAttachmentState);
		createInfo.stateData->depthStencilState = BuildDepthStencilInfo(pipelineInfo);
		createInfo.stateData->dynamicState = BuildDynamicStateInfo(pipelineInfo, createInfo.dynamicStates);
		createInfo.stateData->inputAssemblyState = BuildInputAssemblyInfo(pipelineInfo);
		createInfo.stateData->multiSampleState = BuildMultisampleInfo(pipelineInfo);
		createInfo.stateData->rasterizationState = BuildRasterizationInfo(pipelineInfo);
		createInfo.stateData->viewportState = BuildViewportInfo(pipelineInfo);
		createInfo.stateData->vertexInputState = BuildVertexInputInfo(pipelineInfo, createInfo.vertexAttributesDescription, createInfo.vertexBindingDescription);

		createInfo.pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.pipelineInfo.stageCount = std::uint32_t(createInfo.shaderStages.size());
		createInfo.pipelineInfo.pStages = createInfo.shaderStages.data();
		createInfo.pipelineInfo.pColorBlendState    = &createInfo.stateData->colorBlendState;
		createInfo.pipelineInfo.pDepthStencilState  = &createInfo.stateData->depthStencilState;
		createInfo.pipelineInfo.pDynamicState       = &createInfo.stateData->dynamicState;
		createInfo.pipelineInfo.pInputAssemblyState = &createInfo.stateData->inputAssemblyState;
		createInfo.pipelineInfo.pMultisampleState   = &createInfo.stateData->multiSampleState;
		createInfo.pipelineInfo.pRasterizationState = &createInfo.stateData->rasterizationState;
		createInfo.pipelineInfo.pVertexInputState   = &createInfo.stateData->vertexInputState;
		createInfo.pipelineInfo.pViewportState      = &createInfo.stateData->viewportState;

		VulkanRenderPipelineLayout& pipelineLayout = *static_cast<VulkanRenderPipelineLayout*>(pipelineInfo.pipelineLayout.get());
		createInfo.pipelineInfo.layout = pipelineLayout.GetPipelineLayout();

		return createInfo;
	}

	void VulkanRenderPipeline::UpdateCreateInfo(std::size_t colorBufferCount) const
	{
		// TODO: Add support for independent blend
		std::size_t previousSize = m_pipelineCreateInfo.colorBlendAttachmentState.size();
		if (previousSize < colorBufferCount)
		{
			assert(!m_pipelineCreateInfo.colorBlendAttachmentState.empty());

			m_pipelineCreateInfo.colorBlendAttachmentState.resize(colorBufferCount);
			for (std::size_t i = previousSize; i < colorBufferCount; ++i)
				m_pipelineCreateInfo.colorBlendAttachmentState[i] = m_pipelineCreateInfo.colorBlendAttachmentState.front();
		}

		m_pipelineCreateInfo.stateData->colorBlendState = BuildColorBlendInfo(m_pipelineInfo, m_pipelineCreateInfo.colorBlendAttachmentState);
	}
}
