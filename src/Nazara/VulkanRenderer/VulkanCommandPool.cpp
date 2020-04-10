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
		std::vector<Vk::AutoCommandBuffer> commandBuffers;
		auto BuildCommandBuffer = [&](std::size_t imageIndex)
		{
			Vk::AutoCommandBuffer& commandBuffer = commandBuffers.emplace_back(m_commandPool.AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY));

			if (!commandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT))
				throw std::runtime_error("failed to begin command buffer: " + TranslateVulkanError(commandBuffer->GetLastErrorCode()));

			VulkanCommandBufferBuilder builder(commandBuffer.Get(), imageIndex);
			callback(builder);

			if (!commandBuffer->End())
				throw std::runtime_error("failed to build command buffer: " + TranslateVulkanError(commandBuffer->GetLastErrorCode()));

			return builder.GetMaxFramebufferCount();
		};

		std::size_t maxFramebufferCount = BuildCommandBuffer(0);
		for (std::size_t i = 1; i < maxFramebufferCount; ++i)
			BuildCommandBuffer(i);

		return std::make_unique<VulkanCommandBuffer>(std::move(commandBuffers));
	}
}
