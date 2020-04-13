// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKINSTANCE_HPP
#define NAZARA_VULKANRENDERER_VKINSTANCE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <vulkan/vulkan.h>
#include <string>
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
				inline bool Create(const std::string& appName, UInt32 appVersion, const std::string& engineName, UInt32 engineVersion, const std::vector<const char*>& layers, const std::vector<const char*>& extensions, const VkAllocationCallbacks* allocator = nullptr);
				inline void Destroy();

				bool EnumeratePhysicalDevices(std::vector<VkPhysicalDevice>* physicalDevices);

				inline PFN_vkVoidFunction GetDeviceProcAddr(VkDevice device, const char* name);

				inline UInt32 GetApiVersion() const;
				inline VkResult GetLastErrorCode() const;

				bool GetPhysicalDeviceExtensions(VkPhysicalDevice device, std::vector<VkExtensionProperties>* extensionProperties);
				inline VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(VkPhysicalDevice device);
				inline VkFormatProperties GetPhysicalDeviceFormatProperties(VkPhysicalDevice device, VkFormat format);
				inline bool GetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* imageFormatProperties);
				inline VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties(VkPhysicalDevice device);
				inline VkPhysicalDeviceProperties GetPhysicalDeviceProperties(VkPhysicalDevice device);
				bool GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>* queueFamilyProperties);

				inline bool IsExtensionLoaded(const std::string& extensionName) const;
				inline bool IsLayerLoaded(const std::string& layerName) const;
				inline bool IsValid() const;

				Instance& operator=(const Instance&) = delete;
				Instance& operator=(Instance&&) = delete;

				inline operator VkInstance();

				// Vulkan functions
#define NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN(ext)
#define NAZARA_VULKANRENDERER_INSTANCE_EXT_END()
#define NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(func) PFN_##func func;
#define NAZARA_VULKANRENDERER_INSTANCE_CORE_EXT_FUNCTION(func, ...) NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(func)

#include <Nazara/VulkanRenderer/Wrapper/InstanceFunctions.hpp>

#undef NAZARA_VULKANRENDERER_INSTANCE_CORE_EXT_FUNCTION
#undef NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN
#undef NAZARA_VULKANRENDERER_INSTANCE_EXT_END
#undef NAZARA_VULKANRENDERER_INSTANCE_FUNCTION

			private:
				inline void DestroyInstance();
				void ResetPointers();

				inline PFN_vkVoidFunction GetProcAddr(const char* name);

				VkAllocationCallbacks m_allocator;
				VkInstance m_instance;
				VkResult m_lastErrorCode;
				UInt32 m_apiVersion;
				std::unordered_set<std::string> m_loadedExtensions;
				std::unordered_set<std::string> m_loadedLayers;
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/Instance.inl>

#endif // NAZARA_VULKANRENDERER_VKINSTANCE_HPP
