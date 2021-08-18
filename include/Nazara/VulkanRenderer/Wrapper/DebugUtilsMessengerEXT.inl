// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/DebugUtilsMessengerEXT.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline bool DebugUtilsMessengerEXT::IsSupported(Instance& instance)
		{
			return instance.vkCreateDebugUtilsMessengerEXT != nullptr;
		}

		inline VkResult DebugUtilsMessengerEXT::CreateHelper(Instance& instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* handle)
		{
			return instance.vkCreateDebugUtilsMessengerEXT(instance, createInfo, allocator, handle);
		}

		inline void DebugUtilsMessengerEXT::DestroyHelper(Instance& instance, VkDebugUtilsMessengerEXT handle, const VkAllocationCallbacks* allocator)
		{
			return instance.vkDestroyDebugUtilsMessengerEXT(instance, handle, allocator);
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
