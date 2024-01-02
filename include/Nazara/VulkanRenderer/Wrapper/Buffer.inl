// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline bool Buffer::BindBufferMemory(VkDeviceMemory memory, VkDeviceSize offset)
		{
			m_lastErrorCode = m_device->vkBindBufferMemory(*m_device, m_handle, memory, offset);
			if (m_lastErrorCode != VK_SUCCESS)
			{
				NazaraError("failed to bind buffer memory");
				return false;
			}

			return true;
		}

		inline bool Buffer::Create(Device& device, VkBufferCreateFlags flags, VkDeviceSize size, VkBufferUsageFlags usage, const VkAllocationCallbacks* allocator)
		{
			VkBufferCreateInfo createInfo = {
				VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, // VkStructureType        sType;
				nullptr,                              // const void*            pNext;
				flags,                                // VkBufferCreateFlags    flags;
				size,                                 // VkDeviceSize           size;
				usage,                                // VkBufferUsageFlags     usage;
				VK_SHARING_MODE_EXCLUSIVE,            // VkSharingMode          sharingMode;
				0,                                    // uint32_t               queueFamilyIndexCount;
				nullptr                               // const uint32_t*        pQueueFamilyIndices;
			};

			return Create(device, createInfo, allocator);
		}

		inline VkMemoryRequirements Buffer::GetMemoryRequirements() const
		{
			NazaraAssert(IsValid(), "Invalid buffer");

			VkMemoryRequirements memoryRequirements;
			m_device->vkGetBufferMemoryRequirements(*m_device, m_handle, &memoryRequirements);

			return memoryRequirements;
		}

		inline VkResult Buffer::CreateHelper(Device& device, const VkBufferCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkBuffer* handle)
		{
			return device.vkCreateBuffer(device, createInfo, allocator, handle);
		}

		inline void Buffer::DestroyHelper(Device& device, VkBuffer handle, const VkAllocationCallbacks* allocator)
		{
			return device.vkDestroyBuffer(device, handle, allocator);
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
