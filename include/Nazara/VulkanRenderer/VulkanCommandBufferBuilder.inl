// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanCommandBufferBuilder.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanCommandBufferBuilder::VulkanCommandBufferBuilder(Vk::CommandBuffer& commandBuffer) :
	m_commandBuffer(commandBuffer)
	{
	}

	inline Vk::CommandBuffer& VulkanCommandBufferBuilder::GetCommandBuffer()
	{
		return m_commandBuffer;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
