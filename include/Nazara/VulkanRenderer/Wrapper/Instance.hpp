// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKINSTANCE_HPP
#define NAZARA_VULKANRENDERER_VKINSTANCE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <vulkan/vulkan_core.h>
#include <string>
#include <unordered_set>

// Don't include vulkan.h because it does includes X11 headers which has some defines like "None", breaking compilation
#ifdef VK_USE_PLATFORM_ANDROID_KHR
#include <vulkan/vulkan_android.h>
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
#include <wayland-client.h>
#include <vulkan/vulkan_wayland.h>
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
#include <xcb/xcb.h>
#include <vulkan/vulkan_xcb.h>
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
typedef struct _XDisplay Display;
typedef unsigned long XID;
typedef XID Window;
typedef unsigned long VisualID;

#include <vulkan/vulkan_xlib.h>
#endif

namespace Nz 
{
	namespace Vk
	{
		class NAZARA_VULKANRENDERER_API Instance
		{
			public:
				Instance();
				Instance(const Instance&) = delete;
				Instance(Instance&&) = delete;
				~Instance();

				bool Create(const VkInstanceCreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline bool Create(const std::string& appName, UInt32 appVersion, const std::string& engineName, UInt32 engineVersion, UInt32 apiVersion, const std::vector<const char*>& layers, const std::vector<const char*>& extensions, const VkAllocationCallbacks* allocator = nullptr);
				inline void Destroy();

				bool EnumeratePhysicalDevices(std::vector<VkPhysicalDevice>* physicalDevices) const;

				inline PFN_vkVoidFunction GetDeviceProcAddr(VkDevice device, const char* name) const;

				inline UInt32 GetApiVersion() const;
				inline VkResult GetLastErrorCode() const;

				bool GetPhysicalDeviceExtensions(VkPhysicalDevice device, std::vector<VkExtensionProperties>* extensionProperties) const;
				inline VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(VkPhysicalDevice device) const;
				inline VkFormatProperties GetPhysicalDeviceFormatProperties(VkPhysicalDevice device, VkFormat format) const;
				inline bool GetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* imageFormatProperties) const;
				inline VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties(VkPhysicalDevice device) const;
				inline VkPhysicalDeviceProperties GetPhysicalDeviceProperties(VkPhysicalDevice device) const;
				bool GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>* queueFamilyProperties) const;

				void InstallDebugMessageCallback();

				inline bool IsExtensionLoaded(const std::string& extensionName) const;
				inline bool IsLayerLoaded(const std::string& layerName) const;
				inline bool IsValid() const;

				Instance& operator=(const Instance&) = delete;
				Instance& operator=(Instance&&) = delete;

				inline operator VkInstance();

				// Vulkan functions
#define NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(func) PFN_##func func;
#define NAZARA_VULKANRENDERER_INSTANCE_CORE_EXT_FUNCTION(func, ...) NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(func)

#include <Nazara/VulkanRenderer/Wrapper/InstanceFunctions.hpp>

			private:
				void DestroyInstance();
				void ResetPointers();

				inline PFN_vkVoidFunction GetProcAddr(const char* name) const;

				struct InternalData;

				std::unique_ptr<InternalData> m_internalData;
				std::unordered_set<std::string> m_loadedExtensions;
				std::unordered_set<std::string> m_loadedLayers;
				VkAllocationCallbacks m_allocator;
				VkInstance m_instance;
				mutable VkResult m_lastErrorCode;
				UInt32 m_apiVersion;
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/Instance.inl>

#endif // NAZARA_VULKANRENDERER_VKINSTANCE_HPP
