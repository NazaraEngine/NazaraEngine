// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanCommandBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandPool.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

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
#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#endif
