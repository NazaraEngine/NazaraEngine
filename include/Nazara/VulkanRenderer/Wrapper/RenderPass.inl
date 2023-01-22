// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/RenderPass.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
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
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
