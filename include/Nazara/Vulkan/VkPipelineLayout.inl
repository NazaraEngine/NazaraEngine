// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkPipelineLayout.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline VkResult PipelineLayout::CreateHelper(const DeviceHandle& device, const VkPipelineLayoutCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkPipelineLayout* handle)
		{
			return device->vkCreatePipelineLayout(*device, createInfo, allocator, handle);
		}

		inline void PipelineLayout::DestroyHelper(const DeviceHandle& device, VkPipelineLayout handle, const VkAllocationCallbacks* allocator)
		{
			return device->vkDestroyPipelineLayout(*device, handle, allocator);
		}
	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
