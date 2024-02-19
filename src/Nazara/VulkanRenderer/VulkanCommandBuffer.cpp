// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/VulkanCommandBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandPool.hpp>

namespace Nz
{
	void VulkanCommandBuffer::UpdateDebugName(std::string_view name)
	{
		return m_owner.m_device->SetDebugName(VK_OBJECT_TYPE_COMMAND_BUFFER, VulkanHandleToInteger(static_cast<VkCommandBuffer>(m_commandBuffer)), name);
	}

	void VulkanCommandBuffer::Release()
	{
		m_owner.Release(*this);
	}
}
