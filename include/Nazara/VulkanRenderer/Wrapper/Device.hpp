// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKDEVICE_HPP
#define NAZARA_VULKANRENDERER_VKDEVICE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <Nazara/VulkanRenderer/Wrapper/PhysicalDevice.hpp>
#include <vulkan/vulkan_core.h>
#include <array>
#include <memory>
#include <unordered_set>

VK_DEFINE_HANDLE(VmaAllocator)
VK_DEFINE_HANDLE(VmaAllocation)

namespace Nz 
{
	namespace Vk
	{
		class AutoCommandBuffer;
		class Instance;
		class QueueHandle;

		class NAZARA_VULKANRENDERER_API Device : public std::enable_shared_from_this<Device>
		{
			public:
				struct QueueFamilyInfo;
				struct QueueInfo;
				using QueueList = std::vector<QueueInfo>;

				Device(Instance& instance);
				Device(const Device&) = delete;
				Device(Device&&) = delete;
				~Device();

				AutoCommandBuffer AllocateCommandBuffer(QueueType queueType);

				bool Create(const Vk::PhysicalDevice& deviceInfo, const VkDeviceCreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline void Destroy();

				inline const std::vector<QueueFamilyInfo>& GetEnabledQueues() const;
				inline const QueueList& GetEnabledQueues(UInt32 familyQueue) const;

				QueueHandle GetQueue(UInt32 queueFamilyIndex, UInt32 queueIndex);
				inline Instance& GetInstance();
				inline const Instance& GetInstance() const;
				inline VkResult GetLastErrorCode() const;
				inline VmaAllocator GetMemoryAllocator() const;
				inline VkPhysicalDevice GetPhysicalDevice() const;
				inline const Vk::PhysicalDevice& GetPhysicalDeviceInfo() const;

				inline UInt32 GetDefaultFamilyIndex(QueueType queueType) const;

				inline bool IsExtensionLoaded(const std::string& extensionName);
				inline bool IsLayerLoaded(const std::string& layerName);

				inline bool WaitForIdle();

				Device& operator=(const Device&) = delete;
				Device& operator=(Device&&) = delete;

				inline operator VkDevice();

				// Vulkan functions
#define NAZARA_VULKANRENDERER_DEVICE_FUNCTION(func) PFN_##func func = nullptr;
#define NAZARA_VULKANRENDERER_DEVICE_CORE_EXT_FUNCTION(func, ...) NAZARA_VULKANRENDERER_DEVICE_FUNCTION(func)

#include <Nazara/VulkanRenderer/Wrapper/DeviceFunctions.hpp>

				struct QueueInfo
				{
					QueueFamilyInfo* familyInfo;
					VkQueue queue;
					float priority;
				};

				struct QueueFamilyInfo
				{
					QueueList queues;
					VkExtent3D minImageTransferGranularity;
					VkQueueFlags flags;
					UInt32 familyIndex;
					UInt32 timestampValidBits;
				};

				static constexpr UInt32 InvalidQueue = std::numeric_limits<UInt32>::max();

			private:
				void ResetPointers();
				void WaitAndDestroyDevice();

				inline PFN_vkVoidFunction GetProcAddr(const char* name);

				struct InternalData;

				static constexpr std::size_t QueueCount = static_cast<std::size_t>(QueueType::Max) + 1;

				std::unique_ptr<InternalData> m_internalData;
				std::array<UInt32, QueueCount> m_defaultQueues;
				std::unordered_set<std::string> m_loadedExtensions;
				std::unordered_set<std::string> m_loadedLayers;
				std::vector<QueueFamilyInfo> m_enabledQueuesInfos;
				std::vector<const QueueList*> m_queuesByFamily;
				Instance& m_instance;
				const Vk::PhysicalDevice* m_physicalDevice;
				VkAllocationCallbacks m_allocator;
				VkDevice m_device;
				VkResult m_lastErrorCode;
				VmaAllocator m_memAllocator;
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/Device.inl>

#endif // NAZARA_VULKANRENDERER_VKDEVICE_HPP
