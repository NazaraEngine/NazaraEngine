// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanCommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanCommandBuffer::VulkanCommandBuffer(Vk::AutoCommandBuffer commandBuffer) :
	m_commandBuffer(std::move(commandBuffer))
	{
	}

	inline Vk::CommandBuffer& VulkanCommandBuffer::GetCommandBuffer()
	{
		return m_commandBuffer.Get();
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
