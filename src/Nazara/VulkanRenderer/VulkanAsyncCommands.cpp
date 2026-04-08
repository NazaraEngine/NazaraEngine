// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/VulkanAsyncCommands.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandBufferBuilder.hpp>
#include <Nazara/VulkanRenderer/VulkanDevice.hpp>

namespace Nz
{
	VulkanAsyncCommands::VulkanAsyncCommands(VulkanDevice& device, QueueType queueType) :
	m_queueType(queueType)
	{
		m_commandBuffer = device.AllocateCommandBuffer(m_queueType);
		m_commandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	}

	void VulkanAsyncCommands::AddCommands(Nz::FunctionRef<void(CommandBufferBuilder& builder)> callback)
	{
		VulkanCommandBufferBuilder commandBuilder(m_commandBuffer);
		callback(commandBuilder);
	}

	VkCommandBuffer VulkanAsyncCommands::PrepareForSubmit()
	{
		m_commandBuffer->End();
		return m_commandBuffer;
	}
}
