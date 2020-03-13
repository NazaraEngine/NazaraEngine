// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Queue.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		Device::Device(Instance& instance) :
		m_instance(instance),
		m_physicalDevice(nullptr),
		m_device(VK_NULL_HANDLE)
		{
		}

		Device::~Device()
		{
			if (m_device != VK_NULL_HANDLE)
				WaitAndDestroyDevice();
		}

		void Device::Destroy()
		{
			if (m_device != VK_NULL_HANDLE)
			{
				WaitAndDestroyDevice();
				ResetPointers();
			}
		}

		bool Device::Create(const Vk::PhysicalDevice& deviceInfo, const VkDeviceCreateInfo& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.vkCreateDevice(deviceInfo.device, &createInfo, allocator, &m_device);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to create Vulkan device");
				return false;
			}

			CallOnExit destroyOnFailure([this] { Destroy(); });

			m_physicalDevice = &deviceInfo;

			// Store the allocator to access them when needed
			if (allocator)
				m_allocator = *allocator;
			else
				m_allocator.pfnAllocation = nullptr;

			// Parse extensions and layers
			for (UInt32 i = 0; i < createInfo.enabledExtensionCount; ++i)
				m_loadedExtensions.emplace(createInfo.ppEnabledExtensionNames[i]);

			for (UInt32 i = 0; i < createInfo.enabledLayerCount; ++i)
				m_loadedLayers.emplace(createInfo.ppEnabledLayerNames[i]);

			// Load all device-related functions
			try
			{
				ErrorFlags flags(ErrorFlag_ThrowException, true);

#define NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN(ext) if (IsExtensionLoaded(#ext)) {
#define NAZARA_VULKANRENDERER_DEVICE_EXT_END() }
#define NAZARA_VULKANRENDERER_DEVICE_FUNCTION(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func));

#include <Nazara/VulkanRenderer/Wrapper/DeviceFunctions.hpp>

#undef NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN
#undef NAZARA_VULKANRENDERER_DEVICE_EXT_END
#undef NAZARA_VULKANRENDERER_DEVICE_FUNCTION
			}
			catch (const std::exception& e)
			{
				NazaraError(std::string("Failed to query device function: ") + e.what());
				return false;
			}

			// And retains informations about queues
			UInt32 maxFamilyIndex = 0;
			m_enabledQueuesInfos.resize(createInfo.queueCreateInfoCount);
			for (UInt32 i = 0; i < createInfo.queueCreateInfoCount; ++i)
			{
				const VkDeviceQueueCreateInfo& queueCreateInfo = createInfo.pQueueCreateInfos[i];
				QueueFamilyInfo& info = m_enabledQueuesInfos[i];

				info.familyIndex = queueCreateInfo.queueFamilyIndex;
				if (info.familyIndex > maxFamilyIndex)
					maxFamilyIndex = info.familyIndex;

				const VkQueueFamilyProperties& queueProperties = deviceInfo.queues[info.familyIndex];
				info.flags = queueProperties.queueFlags;
				info.minImageTransferGranularity = queueProperties.minImageTransferGranularity;
				info.timestampValidBits = queueProperties.timestampValidBits;

				info.queues.resize(queueCreateInfo.queueCount);
				for (UInt32 queueIndex = 0; queueIndex < queueCreateInfo.queueCount; ++queueIndex)
				{
					QueueInfo& queueInfo = info.queues[queueIndex];
					queueInfo.familyInfo = &info;
					queueInfo.priority = queueCreateInfo.pQueuePriorities[queueIndex];
					vkGetDeviceQueue(m_device, info.familyIndex, queueIndex, &queueInfo.queue);
				}
			}

			m_queuesByFamily.resize(maxFamilyIndex + 1);
			for (const QueueFamilyInfo& familyInfo : m_enabledQueuesInfos)
				m_queuesByFamily[familyInfo.familyIndex] = &familyInfo.queues;

			destroyOnFailure.Reset();

			return true;
		}

		Queue Device::GetQueue(UInt32 queueFamilyIndex, UInt32 queueIndex)
		{
			VkQueue queue;
			vkGetDeviceQueue(m_device, queueFamilyIndex, queueIndex, &queue);

			return Queue(*this, queue);
		}

		void Device::WaitAndDestroyDevice()
		{
			assert(m_device != VK_NULL_HANDLE);

			if (vkDeviceWaitIdle)
				vkDeviceWaitIdle(m_device);

			m_internalData.reset();

			if (vkDestroyDevice)
				vkDestroyDevice(m_device, (m_allocator.pfnAllocation) ? &m_allocator : nullptr);
		}

		void Device::ResetPointers()
		{
			m_device = VK_NULL_HANDLE;
			m_physicalDevice = nullptr;

			// Reset functions pointers
#define NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN(ext)
#define NAZARA_VULKANRENDERER_DEVICE_EXT_END()
#define NAZARA_VULKANRENDERER_DEVICE_FUNCTION(func) func = nullptr;

#include <Nazara/VulkanRenderer/Wrapper/DeviceFunctions.hpp>

#undef NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN
#undef NAZARA_VULKANRENDERER_DEVICE_EXT_END
#undef NAZARA_VULKANRENDERER_DEVICE_FUNCTION
		}
	}
}
