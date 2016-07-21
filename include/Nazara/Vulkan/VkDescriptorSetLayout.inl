// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkDescriptorSetLayout.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline bool DescriptorSetLayout::Create(const DeviceHandle& device, const VkDescriptorSetLayoutBinding& binding, VkDescriptorSetLayoutCreateFlags flags, const VkAllocationCallbacks* allocator)
		{
			return Create(device, 1U, &binding, flags, allocator);
		}

		inline bool DescriptorSetLayout::Create(const DeviceHandle& device, UInt32 bindingCount, const VkDescriptorSetLayoutBinding* binding, VkDescriptorSetLayoutCreateFlags flags, const VkAllocationCallbacks* allocator)
		{
			VkDescriptorSetLayoutCreateInfo createInfo = 
			{
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, // VkStructureType                        sType;
				nullptr,                                             // const void*                            pNext;
				flags,                                               // VkDescriptorSetLayoutCreateFlags       flags;
				bindingCount,                                        // uint32_t                               bindingCount;
				binding                                              // const VkDescriptorSetLayoutBinding*    pBindings;
			};

			return Create(device, createInfo, allocator);
		}

		inline VkResult DescriptorSetLayout::CreateHelper(const DeviceHandle& device, const VkDescriptorSetLayoutCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkDescriptorSetLayout* handle)
		{
			return device->vkCreateDescriptorSetLayout(*device, createInfo, allocator, handle);
		}

		inline void DescriptorSetLayout::DestroyHelper(const DeviceHandle& device, VkDescriptorSetLayout handle, const VkAllocationCallbacks* allocator)
		{
			return device->vkDestroyDescriptorSetLayout(*device, handle, allocator);
		}
	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
