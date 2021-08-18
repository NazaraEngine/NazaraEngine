// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/Framebuffer.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline VkResult Framebuffer::CreateHelper(Device& device, const VkFramebufferCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkFramebuffer* handle)
		{
			return device.vkCreateFramebuffer(device, createInfo, allocator, handle);
		}

		inline void Framebuffer::DestroyHelper(Device& device, VkFramebuffer handle, const VkAllocationCallbacks* allocator)
		{
			return device.vkDestroyFramebuffer(device, handle, allocator);
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
