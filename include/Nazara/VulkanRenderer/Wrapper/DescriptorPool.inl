// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/DescriptorPool.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline bool DescriptorPool::Create(const DeviceHandle& device, UInt32 maxSets, const VkDescriptorPoolSize& poolSize, VkDescriptorPoolCreateFlags flags, const VkAllocationCallbacks* allocator)
		{
			VkDescriptorPoolCreateInfo createInfo =
			{
				VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, // VkStructureType                sType;
				nullptr,                                       // const void*                    pNext;
				flags,                                         // VkDescriptorPoolCreateFlags    flags;
				maxSets,                                       // uint32_t                       maxSets;
				1U,                                            // uint32_t                       poolSizeCount;
				&poolSize                                      // const VkDescriptorPoolSize*    pPoolSizes;
			};

			return Create(device, createInfo, allocator);
		}

		inline bool DescriptorPool::Create(const DeviceHandle& device, UInt32 maxSets, UInt32 poolSizeCount, const VkDescriptorPoolSize* poolSize, VkDescriptorPoolCreateFlags flags, const VkAllocationCallbacks* allocator)
		{
			VkDescriptorPoolCreateInfo createInfo =
			{
				VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, // VkStructureType                sType;
				nullptr,                                       // const void*                    pNext;
				flags,                                         // VkDescriptorPoolCreateFlags    flags;
				maxSets,                                       // uint32_t                       maxSets;
				poolSizeCount,                                 // uint32_t                       poolSizeCount;
				poolSize                                       // const VkDescriptorPoolSize*    pPoolSizes;
			};

			return Create(device, createInfo, allocator);
		}

		inline VkResult DescriptorPool::CreateHelper(const DeviceHandle& device, const VkDescriptorPoolCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkDescriptorPool* handle)
		{
			return device->vkCreateDescriptorPool(*device, createInfo, allocator, handle);
		}

		inline void DescriptorPool::DestroyHelper(const DeviceHandle& device, VkDescriptorPool handle, const VkAllocationCallbacks* allocator)
		{
			return device->vkDestroyDescriptorPool(*device, handle, allocator);
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
