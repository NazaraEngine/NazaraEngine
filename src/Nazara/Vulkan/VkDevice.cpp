// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkDevice.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		bool Device::Create(VkPhysicalDevice device, const VkDeviceCreateInfo& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.vkCreateDevice(device, &createInfo, allocator, &m_device);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to create Vulkan device");
				return false;
			}

			// Store the allocator to access them when needed
			if (allocator)
				m_allocator = *allocator;
			else
				m_allocator.pfnAllocation = nullptr;

			#define NAZARA_VULKAN_LOAD_DEVICE(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func))

			try
			{
				ErrorFlags flags(ErrorFlag_ThrowException, true);

				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyDevice);
				NAZARA_VULKAN_LOAD_DEVICE(vkDeviceWaitIdle);
				NAZARA_VULKAN_LOAD_DEVICE(vkGetDeviceQueue);
			}
			catch (const std::exception& e)
			{
				NazaraError(String("Failed to query device function: ") + e.what());
				return false;
			}

			#undef NAZARA_VULKAN_LOAD_DEVICE

			return true;
		}
	}
}
