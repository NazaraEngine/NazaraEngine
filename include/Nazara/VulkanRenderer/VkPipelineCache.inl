// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VkPipelineCache.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline VkResult PipelineCache::CreateHelper(const DeviceHandle& device, const VkPipelineCacheCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkPipelineCache* handle)
		{
			return device->vkCreatePipelineCache(*device, createInfo, allocator, handle);
		}

		inline void PipelineCache::DestroyHelper(const DeviceHandle& device, VkPipelineCache handle, const VkAllocationCallbacks* allocator)
		{
			return device->vkDestroyPipelineCache(*device, handle, allocator);
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
