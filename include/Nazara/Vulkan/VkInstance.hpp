// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKINSTANCE_HPP
#define NAZARA_VULKAN_VKINSTANCE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/Config.hpp>
#include <Nazara/Vulkan/VkLoader.hpp>
#include <vulkan/vulkan.h>

namespace Nz 
{
	namespace Vk
	{
		class NAZARA_VULKAN_API Instance
		{
			public:
				inline Instance();
				Instance(const Instance&) = delete;
				Instance(Instance&&) = delete;
				inline ~Instance();

				bool Create(const VkInstanceCreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline void Destroy();

				bool EnumeratePhysicalDevices(std::vector<VkPhysicalDevice>* physicalDevices);

				inline PFN_vkVoidFunction GetDeviceProcAddr(VkDevice device, const char* name);

				inline void GetPhysicalDeviceFeatures(VkPhysicalDevice device, VkPhysicalDeviceFeatures* features);
				inline void GetPhysicalDeviceFormatProperties(VkPhysicalDevice device, VkFormat format, VkFormatProperties* formatProperties);
				inline bool GetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* imageFormatProperties);
				inline void GetPhysicalDeviceMemoryProperties(VkPhysicalDevice device, VkPhysicalDeviceMemoryProperties* properties);
				inline void GetPhysicalDeviceProperties(VkPhysicalDevice device, VkPhysicalDeviceProperties* properties);
				bool GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>* queueFamilyProperties);

				inline VkResult GetLastErrorCode() const;

				Instance& operator=(const Instance&) = delete;
				Instance& operator=(Instance&&) = delete;

				// Vulkan functions
				#define NAZARA_VULKAN_INSTANCE_FUNCTION(func) PFN_##func func

				NAZARA_VULKAN_INSTANCE_FUNCTION(vkCreateDevice);
				NAZARA_VULKAN_INSTANCE_FUNCTION(vkDestroyInstance);
				NAZARA_VULKAN_INSTANCE_FUNCTION(vkEnumeratePhysicalDevices);
				NAZARA_VULKAN_INSTANCE_FUNCTION(vkGetDeviceProcAddr);
				NAZARA_VULKAN_INSTANCE_FUNCTION(vkGetPhysicalDeviceFeatures);
				NAZARA_VULKAN_INSTANCE_FUNCTION(vkGetPhysicalDeviceFormatProperties);
				NAZARA_VULKAN_INSTANCE_FUNCTION(vkGetPhysicalDeviceImageFormatProperties);
				NAZARA_VULKAN_INSTANCE_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
				NAZARA_VULKAN_INSTANCE_FUNCTION(vkGetPhysicalDeviceProperties);
				NAZARA_VULKAN_INSTANCE_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);

				#undef NAZARA_VULKAN_INSTANCE_FUNCTION

			private:
				inline PFN_vkVoidFunction GetProcAddr(const char* name);

				VkAllocationCallbacks m_allocator;
				VkInstance m_instance;
				VkResult m_lastErrorCode;
		};
	}
}

#include <Nazara/Vulkan/VkInstance.inl>

#endif // NAZARA_VULKAN_VKINSTANCE_HPP
