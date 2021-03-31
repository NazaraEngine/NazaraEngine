// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandPool.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPass.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipeline.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/VulkanShaderStage.hpp>
#include <Nazara/VulkanRenderer/VulkanSingleFramebuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanTexture.hpp>
#include <Nazara/VulkanRenderer/VulkanTextureSampler.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanDevice::~VulkanDevice() = default;

	std::shared_ptr<AbstractBuffer> VulkanDevice::InstantiateBuffer(BufferType type)
	{
		return std::make_shared<VulkanBuffer>(*this, type);
	}

	std::shared_ptr<CommandPool> VulkanDevice::InstantiateCommandPool(QueueType queueType)
	{
		return std::make_shared<VulkanCommandPool>(*this, queueType);
	}

	std::shared_ptr<Framebuffer> VulkanDevice::InstantiateFramebuffer(unsigned int width, unsigned int height, const std::shared_ptr<RenderPass>& renderPass, const std::vector<std::shared_ptr<Texture>>& attachments)
	{
		return std::make_shared<VulkanSingleFramebuffer>(*this, width, height, renderPass, attachments);
	}

	std::shared_ptr<RenderPass> VulkanDevice::InstantiateRenderPass(std::vector<RenderPass::Attachment> attachments, std::vector<RenderPass::SubpassDescription> subpassDescriptions, std::vector<RenderPass::SubpassDependency> subpassDependencies)
	{
		return std::make_shared<VulkanRenderPass>(*this, std::move(attachments), std::move(subpassDescriptions), std::move(subpassDependencies));
	}

	std::shared_ptr<RenderPipeline> VulkanDevice::InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo)
	{
		return std::make_shared<VulkanRenderPipeline>(*this, std::move(pipelineInfo));
	}

	std::shared_ptr<RenderPipelineLayout> VulkanDevice::InstantiateRenderPipelineLayout(RenderPipelineLayoutInfo pipelineLayoutInfo)
	{
		auto pipelineLayout = std::make_shared<VulkanRenderPipelineLayout>();
		if (!pipelineLayout->Create(*this, std::move(pipelineLayoutInfo)))
			return {};

		return pipelineLayout;
	}

	std::shared_ptr<ShaderModule> VulkanDevice::InstantiateShaderModule(const ShaderAst& shaderAst, const ShaderWriter::States& states)
	{
		auto stage = std::make_shared<VulkanShaderStage>();
		if (!stage->Create(*this, shaderAst, states))
			return {};

		return stage;
	}

	std::shared_ptr<ShaderModule> VulkanDevice::InstantiateShaderModule(ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize)
	{
		auto stage = std::make_shared<VulkanShaderStage>();
		if (!stage->Create(*this, type, lang, source, sourceSize))
			return {};

		return stage;
	}

	std::shared_ptr<Texture> VulkanDevice::InstantiateTexture(const TextureInfo& params)
	{
		return std::make_shared<VulkanTexture>(*this, params);
	}

	std::shared_ptr<TextureSampler> VulkanDevice::InstantiateTextureSampler(const TextureSamplerInfo& params)
	{
		return std::make_shared<VulkanTextureSampler>(*this, params);
	}
}
