// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKINSTANCE_HPP
#define NAZARA_VULKANRENDERER_VKINSTANCE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <vulkan/vulkan.h>
#include <unordered_set>

namespace Nz 
{
	namespace Vk
	{
		class NAZARA_VULKANRENDERER_API Instance
		{
			public:
				inline Instance();
				Instance(const Instance&) = delete;
				Instance(Instance&&) = delete;
				inline ~Instance();

				bool Create(const VkInstanceCreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline bool Create(const String& appName, UInt32 appVersion, const String& engineName, UInt32 engineVersion, const std::vector<const char*>& layers, const std::vector<const char*>& extensions, const VkAllocationCallbacks* allocator = nullptr);
				inline void Destroy();

				bool EnumeratePhysicalDevices(std::vector<VkPhysicalDevice>* physicalDevices);

				inline PFN_vkVoidFunction GetDeviceProcAddr(VkDevice device, const char* name);

				inline VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(VkPhysicalDevice device);
				inline VkFormatProperties GetPhysicalDeviceFormatProperties(VkPhysicalDevice device, VkFormat format);
				inline bool GetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* imageFormatProperties);
				inline VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties(VkPhysicalDevice device);
				inline VkPhysicalDeviceProperties GetPhysicalDeviceProperties(VkPhysicalDevice device);
				bool GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>* queueFamilyProperties);

				inline VkResult GetLastErrorCode() const;

				inline bool IsExtensionLoaded(const String& extensionName);
				inline bool IsLayerLoaded(const String& layerName);
				inline bool IsValid() const;

				Instance& operator=(const Instance&) = delete;
				Instance& operator=(Instance&&) = delete;

				inline operator VkInstance();

				// Vulkan functions
				#define NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(func) PFN_##func func

				// Vulkan core
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkCreateDevice);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkDestroyInstance);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkEnumeratePhysicalDevices);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetDeviceProcAddr);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceFeatures);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceFormatProperties);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceImageFormatProperties);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceProperties);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);

				// VK_KHR_display
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkCreateDisplayModeKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkCreateDisplayPlaneSurfaceKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetDisplayModePropertiesKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetDisplayPlaneCapabilitiesKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetDisplayPlaneSupportedDisplaysKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceDisplayPlanePropertiesKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceDisplayPropertiesKHR);

				// VK_KHR_surface
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkDestroySurfaceKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);

				// VK_EXT_debug_report
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkCreateDebugReportCallbackEXT);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkDestroyDebugReportCallbackEXT);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkDebugReportMessageEXT);

				#ifdef VK_USE_PLATFORM_ANDROID_KHR
				// VK_KHR_android_surface
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkCreateAndroidSurfaceKHR);
				#endif

				#ifdef VK_USE_PLATFORM_MIR_KHR
				// VK_KHR_mir_surface
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkCreateMirSurfaceKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceMirPresentationSupportKHR);
				#endif

				#ifdef VK_USE_PLATFORM_XCB_KHR
				// VK_KHR_xcb_surface
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkCreateXcbSurfaceKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceXcbPresentationSupportKHR);
				#endif

				#ifdef VK_USE_PLATFORM_XLIB_KHR
				// VK_KHR_xlib_surface
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkCreateXlibSurfaceKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceXlibPresentationSupportKHR);
				#endif

				#ifdef VK_USE_PLATFORM_WAYLAND_KHR
				// VK_KHR_wayland_surface
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkCreateWaylandSurfaceKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceWaylandPresentationSupportKHR);
				#endif

				#ifdef VK_USE_PLATFORM_WIN32_KHR
				// VK_KHR_win32_surface
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkCreateWin32SurfaceKHR);
				NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(vkGetPhysicalDeviceWin32PresentationSupportKHR);
				#endif

				#undef NAZARA_VULKANRENDERER_INSTANCE_FUNCTION

			private:
				inline PFN_vkVoidFunction GetProcAddr(const char* name);

				VkAllocationCallbacks m_allocator;
				VkInstance m_instance;
				VkResult m_lastErrorCode;
				std::unordered_set<String> m_loadedExtensions;
				std::unordered_set<String> m_loadedLayers;
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/Instance.inl>

#endif // NAZARA_VULKANRENDERER_VKINSTANCE_HPP
