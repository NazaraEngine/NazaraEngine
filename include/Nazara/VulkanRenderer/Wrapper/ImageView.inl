// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz::Vk
{
	inline VkResult ImageView::CreateHelper(Device& device, const VkImageViewCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkImageView* handle)
	{
		return device.vkCreateImageView(device, createInfo, allocator, handle);
	}

	inline void ImageView::DestroyHelper(Device& device, VkImageView handle, const VkAllocationCallbacks* allocator)
	{
		return device.vkDestroyImageView(device, handle, allocator);
	}
}
