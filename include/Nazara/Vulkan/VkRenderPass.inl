// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkRenderPass.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline VkResult RenderPass::CreateHelper(const DeviceHandle& device, const VkRenderPassCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkRenderPass* handle)
		{
			return device->vkCreateRenderPass(*device, createInfo, allocator, handle);
		}

		inline void RenderPass::DestroyHelper(const DeviceHandle& device, VkRenderPass handle, const VkAllocationCallbacks* allocator)
		{
			return device->vkDestroyRenderPass(*device, handle, allocator);
		}
	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
