// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKDEVICEMEMORY_HPP
#define NAZARA_VULKANRENDERER_VKDEVICEMEMORY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/VulkanRenderer/VkDeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class DeviceMemory : public DeviceObject<DeviceMemory, VkDeviceMemory, VkMemoryAllocateInfo>
		{
			friend DeviceObject;

			public:
				DeviceMemory();
				DeviceMemory(const DeviceMemory&) = delete;
				DeviceMemory(DeviceMemory&& memory);
				~DeviceMemory() = default;

				using DeviceObject::Create;
				inline bool Create(const DeviceHandle& device, VkDeviceSize size, UInt32 memoryType, const VkAllocationCallbacks* allocator = nullptr);
				inline bool Create(const DeviceHandle& device, VkDeviceSize size, UInt32 typeBits, VkFlags properties, const VkAllocationCallbacks* allocator = nullptr);

				inline void* GetMappedPointer();

				inline bool Map(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags = 0);

				inline void Unmap();

				DeviceMemory& operator=(const DeviceMemory&) = delete;
				DeviceMemory& operator=(DeviceMemory&&) = delete;

			private:
				static inline VkResult CreateHelper(const DeviceHandle& device, const VkMemoryAllocateInfo* allocInfo, const VkAllocationCallbacks* allocator, VkDeviceMemory* handle);
				static inline void DestroyHelper(const DeviceHandle& device, VkDeviceMemory handle, const VkAllocationCallbacks* allocator);

				void* m_mappedPtr;
		};
	}
}

#include <Nazara/VulkanRenderer/VkDeviceMemory.inl>

#endif // NAZARA_VULKANRENDERER_VKDEVICEMEMORY_HPP
