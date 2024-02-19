// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


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

