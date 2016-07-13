// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkDeviceMemory.hpp>
#include <Nazara/Vulkan/VkPhysicalDevice.hpp>
#include <Nazara/Vulkan/Vulkan.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline bool DeviceMemory::Create(const DeviceHandle& device, VkDeviceSize size, UInt32 memoryType, const VkAllocationCallbacks* allocator)
		{
			VkMemoryAllocateInfo allocInfo =
			{
				VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, // VkStructureType    sType;
				nullptr,                                // const void*        pNext;
				size,                                   // VkDeviceSize       allocationSize;
				memoryType                              // uint32_t           memoryTypeIndex;
			};

			return Create(device, allocInfo, allocator);
		}

		inline bool DeviceMemory::Create(const DeviceHandle& device, VkDeviceSize size, UInt32 typeBits, VkFlags properties, const VkAllocationCallbacks* allocator)
		{
			const Vk::PhysicalDevice& deviceInfo = Vulkan::GetPhysicalDeviceInfo(device->GetPhysicalDevice());

			UInt32 typeMask = 1;
			for (UInt32 i = 0; i < VK_MAX_MEMORY_TYPES; ++i)
			{
				if (typeBits & typeMask)
				{
					if ((deviceInfo.memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
						return Create(device, size, i, allocator);
				}

				typeMask <<= 1;
			}

			NazaraError("Failed to find a memory type suitable for typeBits: " + String::Number(typeBits) + " and properties: 0x" + String::Number(properties, 16));
			return false;
		}

		inline VkResult DeviceMemory::CreateHelper(const DeviceHandle& device, const VkMemoryAllocateInfo* allocInfo, const VkAllocationCallbacks* allocator, VkDeviceMemory* handle)
		{
			return device->vkAllocateMemory(*device, allocInfo, allocator, handle);
		}

		inline void DeviceMemory::DestroyHelper(const DeviceHandle& device, VkDeviceMemory handle, const VkAllocationCallbacks* allocator)
		{
			return device->vkFreeMemory(*device, handle, allocator);
		}
	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
