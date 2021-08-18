// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKPIPELINECACHE_HPP
#define NAZARA_VULKANRENDERER_VKPIPELINECACHE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class PipelineCache : public DeviceObject<PipelineCache, VkPipelineCache, VkPipelineCacheCreateInfo, VK_OBJECT_TYPE_PIPELINE_CACHE>
		{
			friend DeviceObject;

			public:
				PipelineCache() = default;
				PipelineCache(const PipelineCache&) = delete;
				PipelineCache(PipelineCache&&) = default;
				~PipelineCache() = default;

				PipelineCache& operator=(const PipelineCache&) = delete;
				PipelineCache& operator=(PipelineCache&&) = delete;

			private:
				static inline VkResult CreateHelper(Device& device, const VkPipelineCacheCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkPipelineCache* handle);
				static inline void DestroyHelper(Device& device, VkPipelineCache handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/PipelineCache.inl>

#endif // NAZARA_VULKANRENDERER_VKPIPELINECACHE_HPP
