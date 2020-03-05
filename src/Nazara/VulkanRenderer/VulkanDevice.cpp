// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipeline.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/VulkanShaderStage.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanDevice::~VulkanDevice() = default;

	std::unique_ptr<AbstractBuffer> VulkanDevice::InstantiateBuffer(Buffer* parent, BufferType type)
	{
		return std::make_unique<VulkanBuffer>(shared_from_this(), parent, type);
	}

	std::unique_ptr<RenderPipeline> VulkanDevice::InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo)
	{
		return std::make_unique<VulkanRenderPipeline>(shared_from_this(), std::move(pipelineInfo));
	}

	std::shared_ptr<RenderPipelineLayout> VulkanDevice::InstantiateRenderPipelineLayout(RenderPipelineLayoutInfo pipelineLayoutInfo)
	{
		auto pipelineLayout = std::make_shared<VulkanRenderPipelineLayout>();
		if (!pipelineLayout->Create(shared_from_this(), std::move(pipelineLayoutInfo)))
			return {};

		return pipelineLayout;
	}

	std::shared_ptr<ShaderStageImpl> VulkanDevice::InstantiateShaderStage(ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize)
	{
		auto stage = std::make_shared<VulkanShaderStage>();
		if (!stage->Create(shared_from_this(), type, lang, source, sourceSize))
			return {};

		return stage;
	}
}
