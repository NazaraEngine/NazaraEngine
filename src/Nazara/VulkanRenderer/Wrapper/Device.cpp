// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/VulkanRenderer/VulkanDescriptorSetLayoutCache.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandPool.hpp>
#include <Nazara/VulkanRenderer/Wrapper/QueueHandle.hpp>
#include <NazaraUtils/CallOnExit.hpp>

#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#include <vma/vk_mem_alloc.h>

#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		struct Device::InternalData
		{
			InternalData(Device& device) :
			setLayoutCache(device)
			{
			}

			EnumArray<QueueType, Vk::CommandPool> commandPools;
			VulkanDescriptorSetLayoutCache setLayoutCache;
		};

		Device::Device(Instance& instance) :
		m_instance(instance),
		m_physicalDevice(nullptr),
		m_device(VK_NULL_HANDLE),
		m_lastErrorCode(VK_SUCCESS),
		m_memAllocator(VK_NULL_HANDLE)
		{
		}

		Device::~Device()
		{
			if (m_device != VK_NULL_HANDLE)
				WaitAndDestroyDevice();
		}

		AutoCommandBuffer Device::AllocateCommandBuffer(QueueType queueType)
		{
			return m_internalData->commandPools[queueType].AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		}

		bool Device::Create(const Vk::PhysicalDevice& deviceInfo, const VkDeviceCreateInfo& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_instance.vkCreateDevice(deviceInfo.physDevice, &createInfo, allocator, &m_device);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraErrorFmt("failed to create Vulkan device: {0}", TranslateVulkanError(m_lastErrorCode));
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
				ErrorFlags flags(ErrorMode::ThrowException);

				UInt32 deviceVersion = deviceInfo.properties.apiVersion;

#define NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN(ext) if (IsExtensionLoaded(#ext)) {
#define NAZARA_VULKANRENDERER_DEVICE_EXT_END() }
#define NAZARA_VULKANRENDERER_DEVICE_FUNCTION(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func, false));
#define NAZARA_VULKANRENDERER_DEVICE_OR_INSTANCE_FUNCTION(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func, true));

#define NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN(ext) if (m_instance.IsExtensionLoaded(#ext)) {
#define NAZARA_VULKANRENDERER_INSTANCE_EXT_END() }

#define NAZARA_VULKANRENDERER_DEVICE_CORE_EXT_FUNCTION(func, coreVersion, suffix, extName)   \
				if (deviceVersion >= coreVersion)                                            \
					func = reinterpret_cast<PFN_##func>(GetProcAddr(#func, false));                 \
				else if (IsExtensionLoaded("VK_" #suffix "_" #extName))                      \
					func = reinterpret_cast<PFN_##func##suffix>(GetProcAddr(#func #suffix, false));

#include <Nazara/VulkanRenderer/Wrapper/DeviceFunctions.hpp>
			}
			catch (const std::exception& e)
			{
				NazaraErrorFmt("Failed to query device function: {0}", e.what());
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
			}

			m_queuesByFamily.resize(maxFamilyIndex + 1);
			for (const QueueFamilyInfo& familyInfo : m_enabledQueuesInfos)
				m_queuesByFamily[familyInfo.familyIndex] = &familyInfo.queues;

			m_internalData = std::make_unique<InternalData>(*this);

			m_defaultQueues.fill(InvalidQueue);
			for (QueueType queueType : { QueueType::Graphics, QueueType::Compute, QueueType::Transfer })
			{
				auto QueueTypeToFlags = [](QueueType type) -> VkQueueFlags
				{
					switch (type)
					{
						case QueueType::Compute:  return VK_QUEUE_COMPUTE_BIT;
						case QueueType::Graphics: return VK_QUEUE_GRAPHICS_BIT;
						case QueueType::Transfer: return VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT; //< Compute/graphics imply transfer
					}

					return 0U;
				};

				for (const QueueFamilyInfo& familyInfo : m_enabledQueuesInfos)
				{
					if ((familyInfo.flags & QueueTypeToFlags(queueType)) == 0)
						continue;

					m_defaultQueues[queueType] = familyInfo.familyIndex;

					// Break only if queue has not been selected before
					auto queueBegin = m_defaultQueues.begin();
					auto queueEnd = queueBegin + static_cast<std::size_t>(queueType);

					if (std::find(queueBegin, queueEnd, familyInfo.familyIndex) == queueEnd)
						break;
				}

				Vk::CommandPool& commandPool = m_internalData->commandPools[queueType];
				if (!commandPool.Create(*this, m_defaultQueues[queueType], VK_COMMAND_POOL_CREATE_TRANSIENT_BIT))
				{
					NazaraErrorFmt("failed to create command pool: {0}", TranslateVulkanError(commandPool.GetLastErrorCode()));
					return false;
				}
			}

			assert(GetDefaultFamilyIndex(QueueType::Transfer) != InvalidQueue);

			// Initialize VMA
			VmaVulkanFunctions vulkanFunctions = {
				Loader::vkGetInstanceProcAddr,
				m_instance.vkGetDeviceProcAddr,
				m_instance.vkGetPhysicalDeviceProperties,
				m_instance.vkGetPhysicalDeviceMemoryProperties,
				vkAllocateMemory,
				vkFreeMemory,
				vkMapMemory,
				vkUnmapMemory,
				vkFlushMappedMemoryRanges,
				vkInvalidateMappedMemoryRanges,
				vkBindBufferMemory,
				vkBindImageMemory,
				vkGetBufferMemoryRequirements,
				vkGetImageMemoryRequirements,
				vkCreateBuffer,
				vkDestroyBuffer,
				vkCreateImage,
				vkDestroyImage,
				vkCmdCopyBuffer,
#if VMA_DEDICATED_ALLOCATION || VMA_VULKAN_VERSION >= 1001000
				vkGetBufferMemoryRequirements2,
				vkGetImageMemoryRequirements2,
#endif
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
				vkBindBufferMemory2,
				vkBindImageMemory2,
#endif
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000
				m_instance.vkGetPhysicalDeviceMemoryProperties2,
#endif
#if VMA_VULKAN_VERSION >= 1003000
				vkGetDeviceBufferMemoryRequirements,
				vkGetDeviceImageMemoryRequirements,
#endif
			};

			VmaAllocatorCreateInfo allocatorInfo = {};
			allocatorInfo.physicalDevice = deviceInfo.physDevice;
			allocatorInfo.device = m_device;
			allocatorInfo.instance = m_instance;
			allocatorInfo.vulkanApiVersion = std::min<UInt32>(VK_API_VERSION_1_1, m_instance.GetApiVersion());
			allocatorInfo.pVulkanFunctions = &vulkanFunctions;

			if (vkGetBufferMemoryRequirements2 && vkGetImageMemoryRequirements2)
				allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;

			if (vkBindBufferMemory2 && vkBindImageMemory2)
				allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT;

			if (IsExtensionLoaded(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME))
				allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;

			m_lastErrorCode = vmaCreateAllocator(&allocatorInfo, &m_memAllocator);
			if (m_lastErrorCode != VK_SUCCESS)
			{
				NazaraErrorFmt("failed to initialize Vulkan Memory Allocator (VMA): {0}", TranslateVulkanError(m_lastErrorCode));
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

		const VulkanDescriptorSetLayoutCache& Device::GetDescriptorSetLayoutCache() const
		{
			assert(m_internalData);
			return m_internalData->setLayoutCache;
		}

		QueueHandle Device::GetQueue(UInt32 queueFamilyIndex, UInt32 queueIndex)
		{
			const auto& queues = GetEnabledQueues(queueFamilyIndex);
			NazaraAssert(queueIndex < queues.size(), "Invalid queue index");

			return QueueHandle(*this, queues[queueIndex].queue, queueFamilyIndex);
		}

		void Device::ResetPointers()
		{
			m_device = VK_NULL_HANDLE;
			m_physicalDevice = nullptr;

			// Reset functions pointers
#define NAZARA_VULKANRENDERER_DEVICE_FUNCTION(func) func = nullptr;
#define NAZARA_VULKANRENDERER_DEVICE_CORE_EXT_FUNCTION(func, ...) NAZARA_VULKANRENDERER_DEVICE_FUNCTION(func)

#include <Nazara/VulkanRenderer/Wrapper/DeviceFunctions.hpp>
		}

		void Device::WaitAndDestroyDevice()
		{
			assert(m_device != VK_NULL_HANDLE);

			if (vkDeviceWaitIdle)
				vkDeviceWaitIdle(m_device);

			if (m_memAllocator != VK_NULL_HANDLE)
				vmaDestroyAllocator(m_memAllocator);

			m_internalData.reset();

			if (vkDestroyDevice)
				vkDestroyDevice(m_device, (m_allocator.pfnAllocation) ? &m_allocator : nullptr);
		}
	}
}

// vma includes vulkan.h which includes system headers
#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#elif defined(NAZARA_PLATFORM_LINUX)
#include <Nazara/Core/AntiX11.hpp>
#endif
