// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz::Vk
{
	inline bool Semaphore::Create(Device& device, VkSemaphoreCreateFlags flags, const VkAllocationCallbacks* allocator)
	{
		VkSemaphoreCreateInfo createInfo =
		{
			VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			nullptr,
			flags
		};

		return Create(device, createInfo, allocator);
	}

	inline VkResult Semaphore::CreateHelper(Device& device, const VkSemaphoreCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkSemaphore* handle)
	{
		return device.vkCreateSemaphore(device, createInfo, allocator, handle);
	}

	inline void Semaphore::DestroyHelper(Device& device, VkSemaphore handle, const VkAllocationCallbacks* allocator)
	{
		return device.vkDestroySemaphore(device, handle, allocator);
	}
}

