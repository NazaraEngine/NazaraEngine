// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Instance.hpp>
#include <NazaraUtils/StackArray.hpp>

namespace Nz::Vk
{
	inline UInt32 Device::GetDefaultFamilyIndex(QueueType queueType) const
	{
		return m_defaultQueues[queueType];
	}

	inline const std::vector<Device::QueueFamilyInfo>& Device::GetEnabledQueues() const
	{
		return m_enabledQueuesInfos;
	}

	inline const Device::QueueList& Device::GetEnabledQueues(UInt32 familyQueue) const
	{
		NazaraAssertMsg(familyQueue < m_enabledQueuesInfos.size(), "invalid family queue %u", familyQueue);

		return *m_queuesByFamily[familyQueue];
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

	inline VmaAllocator Device::GetMemoryAllocator() const
	{
		return m_memAllocator;
	}

	inline VkPhysicalDevice Device::GetPhysicalDevice() const
	{
		return m_physicalDevice->physDevice;
	}

	inline const Vk::PhysicalDevice& Device::GetPhysicalDeviceInfo() const
	{
		return *m_physicalDevice;
	}

	inline PFN_vkVoidFunction Device::GetProcAddr(const char* name, bool allowInstanceFallback)
	{
		PFN_vkVoidFunction func;
		{
			ErrorFlags errFlags({}, ErrorMode::ThrowException);
			func = m_instance.GetDeviceProcAddr(m_device, name);
		}

		if (!func)
		{
			if (allowInstanceFallback)
				return m_instance.GetProcAddr(name);

			NazaraError("failed to get {0} address", name);
		}

		return func;
	}

	inline bool Device::IsExtensionLoaded(std::string_view extensionName)
	{
		return m_loadedExtensions.count(extensionName) > 0;
	}

	inline bool Device::IsLayerLoaded(std::string_view layerName)
	{
		return m_loadedLayers.count(layerName) > 0;
	}

	inline void Device::SetDebugName(VkObjectType objectType, UInt64 objectHandle, const char* name)
	{
		if (vkSetDebugUtilsObjectNameEXT)
		{
			VkDebugUtilsObjectNameInfoEXT debugName = {
				VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
				nullptr,
				objectType,
				objectHandle,
				name
			};

			vkSetDebugUtilsObjectNameEXT(m_device, &debugName);
		}
	}

	inline void Device::SetDebugName(VkObjectType objectType, UInt64 objectHandle, std::string_view name)
	{
		if (vkSetDebugUtilsObjectNameEXT)
		{
			// Ensure \0 at the end of string
			StackArray<char> nullTerminatedName = NazaraStackArrayNoInit(char, name.size() + 1);
			std::memcpy(nullTerminatedName.data(), name.data(), name.size());
			nullTerminatedName[name.size()] = '\0';

			return SetDebugName(objectType, objectHandle, nullTerminatedName.data());
		}
	}

	inline void Device::SetDebugName(VkObjectType objectType, UInt64 objectHandle, const std::string& name)
	{
		return SetDebugName(objectType, objectHandle, name.data());
	}

	inline bool Device::WaitForIdle()
	{
		m_lastErrorCode = vkDeviceWaitIdle(m_device);
		if (m_lastErrorCode != VkResult::VK_SUCCESS)
		{
			NazaraError("failed to wait for device idle: {0}", TranslateVulkanError(m_lastErrorCode));
			return false;
		}

		return true;
	}

	inline Device::operator VkDevice()
	{
		return m_device;
	}
}

