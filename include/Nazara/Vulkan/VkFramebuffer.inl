// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkFramebuffer.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline VkResult Framebuffer::CreateHelper(const DeviceHandle& device, const VkFramebufferCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkFramebuffer* handle)
		{
			return device->vkCreateFramebuffer(*device, createInfo, allocator, handle);
		}

		inline void Framebuffer::DestroyHelper(const DeviceHandle& device, VkFramebuffer handle, const VkAllocationCallbacks* allocator)
		{
			return device->vkDestroyFramebuffer(*device, handle, allocator);
		}
	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
