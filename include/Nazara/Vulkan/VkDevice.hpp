// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKDEVICE_HPP
#define NAZARA_VULKAN_VKDEVICE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/Config.hpp>
#include <Nazara/Vulkan/VkLoader.hpp>
#include <vulkan/vulkan.h>

namespace Nz 
{
	namespace Vk
	{
		class Instance;

		class NAZARA_VULKAN_API Device
		{
			public:
				inline Device(Instance& instance);
				Device(const Device&) = delete;
				Device(Device&&) = delete;
				inline ~Device();

				bool Create(VkPhysicalDevice device, const VkDeviceCreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline void Destroy();

				inline void GetDeviceQueue(UInt32 queueFamilyIndex, UInt32 queueIndex, VkQueue* queue);
				inline Instance& GetInstance();
				inline const Instance& GetInstance() const;
				inline VkResult GetLastErrorCode() const;

				inline bool WaitForIdle();

				Device& operator=(const Device&) = delete;
				Device& operator=(Device&&) = delete;

				inline operator VkDevice();

				// Vulkan functions
				#define NAZARA_VULKAN_DEVICE_FUNCTION(func) PFN_##func func

				// Vulkan core
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyDevice);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDeviceWaitIdle);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetDeviceQueue);

				// VK_KHR_display_swapchain
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateSharedSwapchainsKHR);

				// VK_KHR_surface
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroySurfaceKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);

				// VK_KHR_swapchain
				NAZARA_VULKAN_DEVICE_FUNCTION(vkAcquireNextImageKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateSwapchainKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroySwapchainKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetSwapchainImagesKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkQueuePresentKHR);

				#undef NAZARA_VULKAN_DEVICE_FUNCTION

			private:
				inline PFN_vkVoidFunction GetProcAddr(const char* name);

				Instance& m_instance;
				VkAllocationCallbacks m_allocator;
				VkDevice m_device;
				VkResult m_lastErrorCode;
		};
	}
}

#include <Nazara/Vulkan/VkDevice.inl>

#endif // NAZARA_VULKAN_VKDEVICE_HPP
