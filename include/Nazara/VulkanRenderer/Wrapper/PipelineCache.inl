// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz::Vk
{
	inline VkResult PipelineCache::CreateHelper(Device& device, const VkPipelineCacheCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkPipelineCache* handle)
	{
		return device.vkCreatePipelineCache(device, createInfo, allocator, handle);
	}

	inline void PipelineCache::DestroyHelper(Device& device, VkPipelineCache handle, const VkAllocationCallbacks* allocator)
	{
		return device.vkDestroyPipelineCache(device, handle, allocator);
	}
}
