// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKDEVICE_HPP
#define NAZARA_VULKANRENDERER_VKDEVICE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <Nazara/VulkanRenderer/Wrapper/PhysicalDevice.hpp>
#include <vulkan/vulkan.h>
#include <memory>
#include <unordered_set>

namespace Nz 
{
	namespace Vk
	{
		class CommandBuffer;
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

				CommandBuffer AllocateTransferCommandBuffer();

				bool Create(const Vk::PhysicalDevice& deviceInfo, const VkDeviceCreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline void Destroy();

				inline const std::vector<QueueFamilyInfo>& GetEnabledQueues() const;
				inline const QueueList& GetEnabledQueues(UInt32 familyQueue) const;

				QueueHandle GetQueue(UInt32 queueFamilyIndex, UInt32 queueIndex);
				inline Instance& GetInstance();
				inline const Instance& GetInstance() const;
				inline VkResult GetLastErrorCode() const;
				inline VkPhysicalDevice GetPhysicalDevice() const;
				inline const Vk::PhysicalDevice& GetPhysicalDeviceInfo() const;

				inline UInt32 GetTransferQueueFamilyIndex() const;

				inline bool IsExtensionLoaded(const std::string& extensionName);
				inline bool IsLayerLoaded(const std::string& layerName);

				inline bool WaitForIdle();

				Device& operator=(const Device&) = delete;
				Device& operator=(Device&&) = delete;

				inline operator VkDevice();

				// Vulkan functions
#define NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN(ext)
#define NAZARA_VULKANRENDERER_DEVICE_EXT_END()
#define NAZARA_VULKANRENDERER_DEVICE_FUNCTION(func) PFN_##func func = nullptr;

#include <Nazara/VulkanRenderer/Wrapper/DeviceFunctions.hpp>

#undef NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN
#undef NAZARA_VULKANRENDERER_DEVICE_EXT_END
#undef NAZARA_VULKANRENDERER_DEVICE_FUNCTION

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

			private:
				void ResetPointers();
				void WaitAndDestroyDevice();

				inline PFN_vkVoidFunction GetProcAddr(const char* name);

				struct InternalData;

				std::unique_ptr<InternalData> m_internalData;
				Instance& m_instance;
				const Vk::PhysicalDevice* m_physicalDevice;
				VkAllocationCallbacks m_allocator;
				VkDevice m_device;
				VkResult m_lastErrorCode;
				UInt32 m_transferQueueFamilyIndex;
				std::unordered_set<std::string> m_loadedExtensions;
				std::unordered_set<std::string> m_loadedLayers;
				std::vector<QueueFamilyInfo> m_enabledQueuesInfos;
				std::vector<const QueueList*> m_queuesByFamily;
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/Device.inl>

#endif // NAZARA_VULKANRENDERER_VKDEVICE_HPP
