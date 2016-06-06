// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkSemaphore.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline Semaphore::Semaphore(Device& device) :
		DeviceObject(device)
		{
		}

		inline bool Semaphore::Create(VkSemaphoreCreateFlags flags, const VkAllocationCallbacks* allocator)
		{
			VkSemaphoreCreateInfo createInfo =
			{
				VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				nullptr,
				flags
			};

			return Create(createInfo, allocator);
		}

		VkResult Semaphore::CreateHelper(Device& device, const VkSemaphoreCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkSemaphore* handle)
		{
			return device.vkCreateSemaphore(device, createInfo, allocator, handle);
		}

		void Semaphore::DestroyHelper(Device& device, VkSemaphore handle, const VkAllocationCallbacks* allocator)
		{
			return device.vkDestroySemaphore(device, handle, allocator);
		}
	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
