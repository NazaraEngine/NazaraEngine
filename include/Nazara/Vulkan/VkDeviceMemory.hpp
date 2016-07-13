// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKDEVICEMEMORY_HPP
#define NAZARA_VULKAN_VKDEVICEMEMORY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/VkDeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class DeviceMemory : public DeviceObject<DeviceMemory, VkDeviceMemory, VkMemoryAllocateInfo>
		{
			friend DeviceObject;

			public:
				DeviceMemory() = default;
				DeviceMemory(const DeviceMemory&) = delete;
				DeviceMemory(DeviceMemory&&) = default;
				~DeviceMemory() = default;

				using DeviceObject::Create;
				inline bool Create(const DeviceHandle& device, VkDeviceSize size, UInt32 memoryType, const VkAllocationCallbacks* allocator = nullptr);
				inline bool Create(const DeviceHandle& device, VkDeviceSize size, UInt32 typeBits, VkFlags properties, const VkAllocationCallbacks* allocator = nullptr);

				DeviceMemory& operator=(const DeviceMemory&) = delete;
				DeviceMemory& operator=(DeviceMemory&&) = delete;

			private:
				static inline VkResult CreateHelper(const DeviceHandle& device, const VkMemoryAllocateInfo* allocInfo, const VkAllocationCallbacks* allocator, VkDeviceMemory* handle);
				static inline void DestroyHelper(const DeviceHandle& device, VkDeviceMemory handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/Vulkan/VkDeviceMemory.inl>

#endif // NAZARA_VULKAN_VKDEVICEMEMORY_HPP
