// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_INSTANCE_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_INSTANCE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <NazaraUtils/StringHash.hpp>
#include <vulkan/vulkan_core.h>
#include <string>
#include <unordered_set>

// Don't include vulkan.h because it does includes X11 headers which has some defines like "None", breaking compilation
#ifdef VK_USE_PLATFORM_ANDROID_KHR
#include <vulkan/vulkan_android.h>
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan_wayland.h>
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
typedef struct HINSTANCE__* HINSTANCE;
typedef struct HWND__* HWND;
typedef struct HMONITOR__* HMONITOR;
typedef void* HANDLE;
typedef /*_Null_terminated_*/ const wchar_t* LPCWSTR;
typedef unsigned long DWORD;
typedef struct _SECURITY_ATTRIBUTES SECURITY_ATTRIBUTES;
#include <vulkan/vulkan_win32.h>
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
struct xcb_connection_t;
typedef uint32_t xcb_window_t;
typedef uint32_t xcb_visualid_t;
#include <vulkan/vulkan_xcb.h>
#endif

#ifdef VK_USE_PLATFORM_METAL_EXT
#include <vulkan/vulkan_metal.h>
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
typedef struct _XDisplay Display;
typedef unsigned long XID;
typedef XID Window;
typedef unsigned long VisualID;

#include <vulkan/vulkan_xlib.h>
#endif

namespace Nz::Vk
{
	class NAZARA_VULKANRENDERER_API Instance
	{
		public:
			Instance();
			Instance(const Instance&) = delete;
			Instance(Instance&&) = delete;
			~Instance();

			bool Create(RenderAPIValidationLevel validationLevel, const VkInstanceCreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
			inline bool Create(RenderAPIValidationLevel validationLevel, const std::string& appName, UInt32 appVersion, const std::string& engineName, UInt32 engineVersion, UInt32 apiVersion, const std::vector<const char*>& layers, const std::vector<const char*>& extensions, const VkAllocationCallbacks* allocator = nullptr);
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
			inline PFN_vkVoidFunction GetProcAddr(const char* name) const;
			inline RenderAPIValidationLevel GetValidationLevel() const;

			void InstallDebugMessageCallback(RenderAPIValidationLevel validationLevel);

			inline bool IsExtensionLoaded(std::string_view extensionName) const;
			inline bool IsLayerLoaded(std::string_view layerName) const;
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

			struct InternalData;

			std::unique_ptr<InternalData> m_internalData;
			std::unordered_set<std::string, StringHash<>, std::equal_to<>> m_loadedExtensions;
			std::unordered_set<std::string, StringHash<>, std::equal_to<>> m_loadedLayers;
			VkAllocationCallbacks m_allocator;
			VkInstance m_instance;
			mutable VkResult m_lastErrorCode;
			RenderAPIValidationLevel m_validationLevel;
			UInt32 m_apiVersion;
	};
}

#include <Nazara/VulkanRenderer/Wrapper/Instance.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_INSTANCE_HPP
