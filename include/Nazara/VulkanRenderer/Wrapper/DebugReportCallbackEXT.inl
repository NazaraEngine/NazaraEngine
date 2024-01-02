// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz::Vk
{
	inline bool DebugReportCallbackEXT::IsSupported(Instance& instance)
	{
		return instance.vkCreateDebugReportCallbackEXT != nullptr;
	}

	inline VkResult DebugReportCallbackEXT::CreateHelper(Instance& instance, const VkDebugReportCallbackCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugReportCallbackEXT* handle)
	{
		return instance.vkCreateDebugReportCallbackEXT(instance, createInfo, allocator, handle);
	}

	inline void DebugReportCallbackEXT::DestroyHelper(Instance& instance, VkDebugReportCallbackEXT handle, const VkAllocationCallbacks* allocator)
	{
		return instance.vkDestroyDebugReportCallbackEXT(instance, handle, allocator);
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
