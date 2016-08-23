// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VkDevice.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/VulkanRenderer/VkInstance.hpp>
#include <Nazara/VulkanRenderer/VkQueue.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline Device::Device(Instance& instance) :
		m_instance(instance),
		m_device(VK_NULL_HANDLE),
		m_physicalDevice(VK_NULL_HANDLE)
		{
		}

		inline Device::~Device()
		{
			Destroy();
		}

		inline void Device::Destroy()
		{
			if (m_device != VK_NULL_HANDLE)
			{
				vkDeviceWaitIdle(m_device);
				vkDestroyDevice(m_device, (m_allocator.pfnAllocation) ? &m_allocator : nullptr);

				m_device = VK_NULL_HANDLE;
				m_physicalDevice = VK_NULL_HANDLE;
			}
		}

		inline const std::vector<Device::QueueFamilyInfo>& Device::GetEnabledQueues() const
		{
			return m_enabledQueuesInfos;
		}

		inline const Device::QueueList& Device::GetEnabledQueues(UInt32 familyQueue) const
		{
			NazaraAssert(familyQueue < m_enabledQueuesInfos.size(), "Invalid family queue");

			return *m_queuesByFamily[familyQueue];
		}

		inline Queue Device::GetQueue(UInt32 queueFamilyIndex, UInt32 queueIndex)
		{
			VkQueue queue;
			vkGetDeviceQueue(m_device, queueFamilyIndex, queueIndex, &queue);
			
			return Queue(CreateHandle(), queue);
		}

		inline Instance& Device::GetInstance()
		{
			return m_instance;
		}

		inline const Instance& Device::GetInstance() const
		{
			return m_instance;
		}

		inline VkResult Device::GetLastErrorCode() const
		{
			return m_lastErrorCode;
		}

		inline VkPhysicalDevice Device::GetPhysicalDevice() const
		{
			return m_physicalDevice;
		}

		inline bool Device::IsExtensionLoaded(const String& extensionName)
		{
			return m_loadedExtensions.count(extensionName) > 0;
		}

		inline bool Device::IsLayerLoaded(const String& layerName)
		{
			return m_loadedLayers.count(layerName) > 0;
		}

		inline bool Device::WaitForIdle()
		{
			m_lastErrorCode = vkDeviceWaitIdle(m_device);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to wait for device idle");
				return false;
			}

			return true;
		}

		inline Device::operator VkDevice()
		{
			return m_device;
		}

		inline PFN_vkVoidFunction Device::GetProcAddr(const char* name)
		{
			PFN_vkVoidFunction func = m_instance.GetDeviceProcAddr(m_device, name);
			if (!func)
				NazaraError("Failed to get " + String(name) + " address");
			
			return func;
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
