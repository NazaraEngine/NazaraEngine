// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanCommandBufferBuilder.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanCommandBufferBuilder::VulkanCommandBufferBuilder(Vk::CommandBuffer& commandBuffer, std::size_t imageIndex) :
	m_commandBuffer(commandBuffer),
	m_framebufferCount(0),
	m_imageIndex(imageIndex)
	{
	}

	inline Vk::CommandBuffer& VulkanCommandBufferBuilder::GetCommandBuffer()
	{
		return m_commandBuffer;
	}

	inline std::size_t VulkanCommandBufferBuilder::GetMaxFramebufferCount() const
	{
		return m_framebufferCount;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
