// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VkImageView.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline VkResult ImageView::CreateHelper(const DeviceHandle& device, const VkImageViewCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkImageView* handle)
		{
			return device->vkCreateImageView(*device, createInfo, allocator, handle);
		}

		inline void ImageView::DestroyHelper(const DeviceHandle& device, VkImageView handle, const VkAllocationCallbacks* allocator)
		{
			return device->vkDestroyImageView(*device, handle, allocator);
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
