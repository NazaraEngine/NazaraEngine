// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/Utils.hpp>

namespace Nz::Vk
{
	inline bool Image::BindImageMemory(VkDeviceMemory memory, VkDeviceSize offset)
	{
		m_lastErrorCode = m_device->vkBindImageMemory(*m_device, m_handle, memory, offset);
		if (m_lastErrorCode != VK_SUCCESS)
		{
			NazaraError("failed to bind image memory: {0}", TranslateVulkanError(m_lastErrorCode));
			return false;
		}

		return true;
	}

	inline VkMemoryRequirements Image::GetMemoryRequirements() const
	{
		NazaraAssertMsg(IsValid(), "invalid image");

		VkMemoryRequirements memoryRequirements;
		m_device->vkGetImageMemoryRequirements(*m_device, m_handle, &memoryRequirements);

		return memoryRequirements;
	}

	inline VkResult Image::CreateHelper(Device& device, const VkImageCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkImage* handle)
	{
		return device.vkCreateImage(device, createInfo, allocator, handle);
	}

	inline void Image::DestroyHelper(Device& device, VkImage handle, const VkAllocationCallbacks* allocator)
	{
		return device.vkDestroyImage(device, handle, allocator);
	}
}
