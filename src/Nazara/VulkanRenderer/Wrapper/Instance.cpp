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

#include <Nazara/VulkanRenderer/Wrapper/InstanceFunctions.hpp>

#undef NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN
#undef NAZARA_VULKANRENDERER_INSTANCE_EXT_END
#undef NAZARA_VULKANRENDERER_INSTANCE_FUNCTION
			}
			catch (const std::exception& e)
			{
				NazaraError(String("Failed to query instance function: ") + e.what());
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

		bool Instance::GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>* queueFamilyProperties)
		{
			NazaraAssert(queueFamilyProperties, "Invalid device vector");

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

#include <Nazara/VulkanRenderer/Wrapper/InstanceFunctions.hpp>

#undef NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN
#undef NAZARA_VULKANRENDERER_INSTANCE_EXT_END
#undef NAZARA_VULKANRENDERER_INSTANCE_FUNCTION
		}
	}
}
