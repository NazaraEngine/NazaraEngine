// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandPool.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipeline.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/VulkanShaderStage.hpp>
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

	std::shared_ptr<ShaderStage> VulkanDevice::InstantiateShaderStage(ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize)
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
