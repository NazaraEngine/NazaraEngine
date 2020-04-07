// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanCommandPool.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandBufferBuilder.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	std::unique_ptr<CommandBuffer> VulkanCommandPool::BuildCommandBuffer(const std::function<void(CommandBufferBuilder& builder)>& callback)
	{
		Vk::AutoCommandBuffer commandBuffer = m_commandPool.AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		if (!commandBuffer->Begin())
			throw std::runtime_error("failed to begin command buffer: " + TranslateVulkanError(commandBuffer->GetLastErrorCode()));

		VulkanCommandBufferBuilder builder(commandBuffer.Get());
		callback(builder);

		if (!commandBuffer->End())
			throw std::runtime_error("failed to build command buffer: " + TranslateVulkanError(commandBuffer->GetLastErrorCode()));

		return std::make_unique<VulkanCommandBuffer>(std::move(commandBuffer));
	}
}
