// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanCommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanCommandBuffer::VulkanCommandBuffer(Vk::AutoCommandBuffer commandBuffer)
	{
		m_commandBuffers.push_back(std::move(commandBuffer));
	}

	inline VulkanCommandBuffer::VulkanCommandBuffer(std::vector<Vk::AutoCommandBuffer> commandBuffers) :
	m_commandBuffers(std::move(commandBuffers))
	{
	}

	inline Vk::CommandBuffer& VulkanCommandBuffer::GetCommandBuffer(std::size_t imageIndex)
	{
		return m_commandBuffers[imageIndex].Get();
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
