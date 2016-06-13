// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkDevice.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Vulkan/VkInstance.hpp>
#include <Nazara/Vulkan/VkQueue.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline Device::Device(Instance& instance) :
		m_instance(instance),
		m_device(VK_NULL_HANDLE),
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
			}
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

#include <Nazara/Vulkan/DebugOff.hpp>
