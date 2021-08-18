// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/Semaphore.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
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
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
