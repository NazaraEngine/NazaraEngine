// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz::Vk
{
	inline VkResult RenderPass::CreateHelper(Device& device, const VkRenderPassCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkRenderPass* handle)
	{
		return device.vkCreateRenderPass(device, createInfo, allocator, handle);
	}

	inline void RenderPass::DestroyHelper(Device& device, VkRenderPass handle, const VkAllocationCallbacks* allocator)
	{
		return device.vkDestroyRenderPass(device, handle, allocator);
	}
}

