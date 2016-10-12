// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VkDeviceMemory.hpp>
#include <Nazara/VulkanRenderer/VkPhysicalDevice.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/VulkanRenderer/Vulkan.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline DeviceMemory::DeviceMemory() :
		m_mappedPtr(nullptr)
		{
		}

		inline DeviceMemory::DeviceMemory(DeviceMemory&& memory) :
		DeviceObject(std::move(memory))
		{
			m_mappedPtr = memory.m_mappedPtr;
			memory.m_mappedPtr = nullptr;
		}

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

		inline void* DeviceMemory::GetMappedPointer()
		{
			return m_mappedPtr;
		}

		inline bool DeviceMemory::Map(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags)
		{
			m_lastErrorCode = m_device->vkMapMemory(*m_device, m_handle, offset, size, flags, &m_mappedPtr);
			if (m_lastErrorCode != VK_SUCCESS)
			{
				NazaraError("Failed to map device memory: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			return true;
		}

		inline void DeviceMemory::Unmap()
		{
			NazaraAssert(m_mappedPtr != nullptr, "Memory is not mapped");

			m_device->vkUnmapMemory(*m_device, m_handle);
			m_mappedPtr = nullptr;
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

#include <Nazara/VulkanRenderer/DebugOff.hpp>
