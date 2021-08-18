// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/Instance.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <cassert>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline bool Instance::Create(const std::string& appName, UInt32 appVersion, const std::string& engineName, UInt32 engineVersion, UInt32 apiVersion, const std::vector<const char*>& layers, const std::vector<const char*>& extensions, const VkAllocationCallbacks* allocator)
		{
			VkApplicationInfo appInfo = 
			{
				VK_STRUCTURE_TYPE_APPLICATION_INFO,
				nullptr,
				appName.data(),
				appVersion,
				engineName.data(),
				engineVersion,
				apiVersion
			};

			VkInstanceCreateInfo instanceInfo = 
			{
				VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				nullptr,
				0,
				&appInfo,
				static_cast<UInt32>(layers.size()),
				(!layers.empty()) ? layers.data() : nullptr,
				static_cast<UInt32>(extensions.size()),
				(!extensions.empty()) ? extensions.data() : nullptr
			};

			return Create(instanceInfo, allocator);
		}

		inline void Instance::Destroy()
		{
			if (m_instance)
			{
				DestroyInstance();
				ResetPointers();
			}
		}

		inline PFN_vkVoidFunction Instance::GetDeviceProcAddr(VkDevice device, const char* name) const
		{
			PFN_vkVoidFunction func = vkGetDeviceProcAddr(device, name);
			if (!func)
				NazaraError("Failed to get " + std::string(name) + " address");

			return func;
		}

		inline UInt32 Instance::GetApiVersion() const
		{
			return m_apiVersion;
		}

		inline VkResult Instance::GetLastErrorCode() const
		{
			return m_lastErrorCode;
		}

		inline bool Instance::IsExtensionLoaded(const std::string& extensionName) const
		{
			return m_loadedExtensions.count(extensionName) > 0;
		}

		inline bool Instance::IsLayerLoaded(const std::string& layerName) const
		{
			return m_loadedLayers.count(layerName) > 0;
		}

		inline bool Instance::IsValid() const
		{
			return m_instance != nullptr;
		}

		inline Instance::operator VkInstance()
		{
			return m_instance;
		}

		inline VkPhysicalDeviceFeatures Instance::GetPhysicalDeviceFeatures(VkPhysicalDevice device) const
		{
			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures(device, &features);

			return features;
		}

		inline VkFormatProperties Instance::GetPhysicalDeviceFormatProperties(VkPhysicalDevice device, VkFormat format) const
		{
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties(device, format, &formatProperties);

			return formatProperties;
		}

		inline bool Instance::GetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* imageFormatProperties) const
		{
			m_lastErrorCode = vkGetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, imageFormatProperties);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to get physical device image format properties: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			return true;
		}

		inline VkPhysicalDeviceMemoryProperties Instance::GetPhysicalDeviceMemoryProperties(VkPhysicalDevice device) const
		{
			VkPhysicalDeviceMemoryProperties memoryProperties;
			vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);

			return memoryProperties;
		}

		inline VkPhysicalDeviceProperties Instance::GetPhysicalDeviceProperties(VkPhysicalDevice device) const
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(device, &properties);

			return properties;
		}

		inline PFN_vkVoidFunction Instance::GetProcAddr(const char* name) const
		{
			PFN_vkVoidFunction func = Loader::GetInstanceProcAddr(m_instance, name);
			if (!func)
				NazaraError("Failed to get " + std::string(name) + " address");
			
			return func;
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
