// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkBuffer.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline VkMemoryRequirements Buffer::GetMemoryRequirements() const
		{
			NazaraAssert(IsValid(), "Invalid buffer");

			VkMemoryRequirements memoryRequirements;
			m_device->vkGetBufferMemoryRequirements(*m_device, m_handle, &memoryRequirements);

			return memoryRequirements;
		}

		inline VkResult Buffer::CreateHelper(const DeviceHandle& device, const VkBufferCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkBuffer* handle)
		{
			return device->vkCreateBuffer(*device, createInfo, allocator, handle);
		}

		inline void Buffer::DestroyHelper(const DeviceHandle& device, VkBuffer handle, const VkAllocationCallbacks* allocator)
		{
			return device->vkDestroyBuffer(*device, handle, allocator);
		}
	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
