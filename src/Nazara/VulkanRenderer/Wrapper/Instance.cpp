// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/Instance.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		bool Instance::Create(const VkInstanceCreateInfo& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = Loader::vkCreateInstance(&createInfo, allocator, &m_instance);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to create Vulkan instance");
				return false;
			}

			m_apiVersion = createInfo.pApplicationInfo->apiVersion;

			// Store the allocator to access them when needed
			if (allocator)
				m_allocator = *allocator;
			else
				m_allocator.pfnAllocation = nullptr;

			// Parse extensions and layers
			for (UInt32 i = 0; i < createInfo.enabledExtensionCount; ++i)
				m_loadedExtensions.insert(createInfo.ppEnabledExtensionNames[i]);

			for (UInt32 i = 0; i < createInfo.enabledLayerCount; ++i)
				m_loadedLayers.insert(createInfo.ppEnabledLayerNames[i]);

			// And now load everything
			try
			{
				ErrorFlags flags(ErrorFlag_ThrowException, true);

#define NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN(ext) if (IsExtensionLoaded(#ext)) {
#define NAZARA_VULKANRENDERER_INSTANCE_EXT_END() }
#define NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func));

#define NAZARA_VULKANRENDERER_INSTANCE_CORE_EXT_FUNCTION(func, coreVersion, suffix, extName) \
				if (m_apiVersion >= coreVersion)                                            \
					func = reinterpret_cast<PFN_##func>(GetProcAddr(#func));                 \
				else if (IsExtensionLoaded("VK_" #suffix "_" #extName))                      \
					func = reinterpret_cast<PFN_##func##suffix>(GetProcAddr(#func #suffix));

#include <Nazara/VulkanRenderer/Wrapper/InstanceFunctions.hpp>

#undef NAZARA_VULKANRENDERER_INSTANCE_CORE_EXT_FUNCTION
#undef NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN
#undef NAZARA_VULKANRENDERER_INSTANCE_EXT_END
#undef NAZARA_VULKANRENDERER_INSTANCE_FUNCTION
			}
			catch (const std::exception& e)
			{
				NazaraError(std::string("Failed to query instance function: ") + e.what());
				return false;
			}

			return true;
		}

		bool Instance::EnumeratePhysicalDevices(std::vector<VkPhysicalDevice>* devices)
		{
			NazaraAssert(devices, "Invalid device vector");

			// First, query physical device count
			UInt32 deviceCount = 0; // Remember, Nz::UInt32 is a typedef on uint32_t
			m_lastErrorCode = vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
			if (m_lastErrorCode != VkResult::VK_SUCCESS || deviceCount == 0)
			{
				NazaraError("Failed to query physical device count: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			// Now we can get the list of the available physical device
			devices->resize(deviceCount);
			m_lastErrorCode = vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices->data());
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to query physical devices: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			return true;
		}

		bool Instance::GetPhysicalDeviceExtensions(VkPhysicalDevice device, std::vector<VkExtensionProperties>* extensionProperties)
		{
			NazaraAssert(extensionProperties, "Invalid extension properties vector");

			// First, query extension count
			UInt32 extensionPropertyCount = 0;
			m_lastErrorCode = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionPropertyCount, nullptr);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to query extension properties count: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			if (extensionPropertyCount == 0)
				return true; //< No extension available

			// Now we can get the list of the available extensions
			extensionProperties->resize(extensionPropertyCount);
			m_lastErrorCode = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionPropertyCount, extensionProperties->data());
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to query extension properties count: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			return true;
		}

		bool Instance::GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>* queueFamilyProperties)
		{
			NazaraAssert(queueFamilyProperties, "Invalid family properties vector");

			// First, query physical device count
			UInt32 queueFamiliesCount = 0; // Remember, Nz::UInt32 is a typedef on uint32_t
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, nullptr);
			if (queueFamiliesCount == 0)
			{
				NazaraError("Failed to query physical device count");
				return false;
			}

			// Now we can get the list of the available physical device
			queueFamilyProperties->resize(queueFamiliesCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, queueFamilyProperties->data());

			return true;
		}

		void Instance::ResetPointers()
		{
			assert(m_instance != VK_NULL_HANDLE);
			m_instance = VK_NULL_HANDLE;

#define NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN(ext)
#define NAZARA_VULKANRENDERER_INSTANCE_EXT_END()
#define NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(func) func = nullptr;
#define NAZARA_VULKANRENDERER_INSTANCE_CORE_EXT_FUNCTION(func, ...) NAZARA_VULKANRENDERER_INSTANCE_FUNCTION(func)

#include <Nazara/VulkanRenderer/Wrapper/InstanceFunctions.hpp>

#undef NAZARA_VULKANRENDERER_INSTANCE_CORE_EXT_FUNCTION
#undef NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN
#undef NAZARA_VULKANRENDERER_INSTANCE_EXT_END
#undef NAZARA_VULKANRENDERER_INSTANCE_FUNCTION
		}
	}
}
