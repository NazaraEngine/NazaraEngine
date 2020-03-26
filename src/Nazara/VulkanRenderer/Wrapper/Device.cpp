// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandPool.hpp>
#include <Nazara/VulkanRenderer/Wrapper/QueueHandle.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		struct Device::InternalData
		{
			Vk::CommandPool transferCommandPool;
		};

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

		CommandBuffer Device::AllocateTransferCommandBuffer()
		{
			return m_internalData->transferCommandPool.AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		}

		bool Device::Create(const Vk::PhysicalDevice& deviceInfo, const VkDeviceCreateInfo& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.vkCreateDevice(deviceInfo.physDevice, &createInfo, allocator, &m_device);
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

				UInt32 deviceVersion = deviceInfo.properties.apiVersion;

#define NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN(ext) if (IsExtensionLoaded(#ext)) {
#define NAZARA_VULKANRENDERER_DEVICE_EXT_END() }
#define NAZARA_VULKANRENDERER_DEVICE_FUNCTION(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func));

#define NAZARA_VULKANRENDERER_DEVICE_CORE_EXT_FUNCTION(func, coreVersion, suffix, extName)   \
				if (deviceVersion >= coreVersion)                                            \
					func = reinterpret_cast<PFN_##func>(GetProcAddr(#func));                 \
				else if (IsExtensionLoaded("VK_" #suffix "_" #extName))                      \
					func = reinterpret_cast<PFN_##func##suffix>(GetProcAddr(#func #suffix));

#include <Nazara/VulkanRenderer/Wrapper/DeviceFunctions.hpp>

#undef NAZARA_VULKANRENDERER_DEVICE_CORE_EXT_FUNCTION
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
			m_transferQueueFamilyIndex = UINT32_MAX;

			UInt32 maxFamilyIndex = 0;
			m_enabledQueuesInfos.resize(createInfo.queueCreateInfoCount);
			for (UInt32 i = 0; i < createInfo.queueCreateInfoCount; ++i)
			{
				const VkDeviceQueueCreateInfo& queueCreateInfo = createInfo.pQueueCreateInfos[i];
				QueueFamilyInfo& info = m_enabledQueuesInfos[i];

				info.familyIndex = queueCreateInfo.queueFamilyIndex;
				if (info.familyIndex > maxFamilyIndex)
					maxFamilyIndex = info.familyIndex;

				const VkQueueFamilyProperties& queueProperties = deviceInfo.queueFamilies[info.familyIndex];
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

				if (info.flags & (VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT))
				{
					if (m_transferQueueFamilyIndex == UINT32_MAX)
						m_transferQueueFamilyIndex = info.familyIndex;
					else if ((info.flags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT)) == 0)
					{
						m_transferQueueFamilyIndex = info.familyIndex;
						break;
					}
				}
			}

			m_queuesByFamily.resize(maxFamilyIndex + 1);
			for (const QueueFamilyInfo& familyInfo : m_enabledQueuesInfos)
				m_queuesByFamily[familyInfo.familyIndex] = &familyInfo.queues;

			m_internalData = std::make_unique<InternalData>();
			if (!m_internalData->transferCommandPool.Create(*this, m_transferQueueFamilyIndex, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT))
			{
				NazaraError("Failed to create transfer command pool: " + TranslateVulkanError(m_internalData->transferCommandPool.GetLastErrorCode()));
				return false;
			}

			destroyOnFailure.Reset();

			return true;
		}

		void Device::Destroy()
		{
			if (m_device != VK_NULL_HANDLE)
			{
				WaitAndDestroyDevice();
				ResetPointers();
			}
		}

		QueueHandle Device::GetQueue(UInt32 queueFamilyIndex, UInt32 queueIndex)
		{
			const auto& queues = GetEnabledQueues(queueFamilyIndex);
			NazaraAssert(queueIndex < queues.size(), "Invalid queue index");

			return QueueHandle(*this, queues[queueIndex].queue);
		}

		void Device::ResetPointers()
		{
			m_device = VK_NULL_HANDLE;
			m_physicalDevice = nullptr;

			// Reset functions pointers
#define NAZARA_VULKANRENDERER_DEVICE_FUNCTION(func) func = nullptr;
#define NAZARA_VULKANRENDERER_DEVICE_CORE_EXT_FUNCTION(func, ...) NAZARA_VULKANRENDERER_DEVICE_FUNCTION(func)
#define NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN(ext)
#define NAZARA_VULKANRENDERER_DEVICE_EXT_END()

#include <Nazara/VulkanRenderer/Wrapper/DeviceFunctions.hpp>

#undef NAZARA_VULKANRENDERER_DEVICE_CORE_EXT_FUNCTION
#undef NAZARA_VULKANRENDERER_DEVICE_FUNCTION
#undef NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN
#undef NAZARA_VULKANRENDERER_DEVICE_EXT_END
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
	}
}
