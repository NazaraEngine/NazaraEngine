// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkInstance.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Vulkan/Debug.hpp>

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

			#define NAZARA_VULKAN_LOAD_INSTANCE(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func))

			try
			{
				ErrorFlags flags(ErrorFlag_ThrowException, true);

				NAZARA_VULKAN_LOAD_INSTANCE(vkCreateDevice);
				NAZARA_VULKAN_LOAD_INSTANCE(vkDestroyInstance);
				NAZARA_VULKAN_LOAD_INSTANCE(vkEnumeratePhysicalDevices);
				NAZARA_VULKAN_LOAD_INSTANCE(vkGetDeviceProcAddr);
				NAZARA_VULKAN_LOAD_INSTANCE(vkGetPhysicalDeviceFeatures);
				NAZARA_VULKAN_LOAD_INSTANCE(vkGetPhysicalDeviceFormatProperties);
				NAZARA_VULKAN_LOAD_INSTANCE(vkGetPhysicalDeviceImageFormatProperties);
				NAZARA_VULKAN_LOAD_INSTANCE(vkGetPhysicalDeviceMemoryProperties);
				NAZARA_VULKAN_LOAD_INSTANCE(vkGetPhysicalDeviceProperties);
				NAZARA_VULKAN_LOAD_INSTANCE(vkGetPhysicalDeviceQueueFamilyProperties);
			}
			catch (const std::exception& e)
			{
				NazaraError(String("Failed to query instance function: ") + e.what());
				return false;
			}

			#undef NAZARA_VULKAN_LOAD_INSTANCE

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
				NazaraError("Failed to query physical device count");
				return false;
			}

			// Now we can get the list of the available physical device
			devices->resize(deviceCount);
			m_lastErrorCode = vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices->data());
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to query physical devices");
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

	}
}
