// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkDescriptorPool.hpp>
#include <Nazara/Vulkan/VkDescriptorSet.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		DescriptorSet DescriptorPool::AllocateDescriptorSet(const VkDescriptorSetLayout& setLayouts)
		{
			VkDescriptorSetAllocateInfo createInfo =
			{
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, // VkStructureType                 sType;
				nullptr,                                        // const void*                     pNext;
				m_handle,                                       // VkDescriptorPool                descriptorPool;
				1U,                                             // uint32_t                        descriptorSetCount;
				&setLayouts                                     // const VkDescriptorSetLayout*    pSetLayouts;
			};

			VkDescriptorSet handle = VK_NULL_HANDLE;
			m_lastErrorCode = m_device->vkAllocateDescriptorSets(*m_device, &createInfo, &handle);

			return DescriptorSet(*this, handle);
		}

		std::vector<DescriptorSet> DescriptorPool::AllocateDescriptorSets(UInt32 descriptorSetCount, const VkDescriptorSetLayout* setLayouts)
		{
			VkDescriptorSetAllocateInfo createInfo =
			{
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, // VkStructureType                 sType;
				nullptr,                                        // const void*                     pNext;
				m_handle,                                       // VkDescriptorPool                descriptorPool;
				descriptorSetCount,                             // uint32_t                        descriptorSetCount;
				setLayouts                                      // const VkDescriptorSetLayout*    pSetLayouts;
			};

			std::vector<VkDescriptorSet> handles(descriptorSetCount, VK_NULL_HANDLE);
			m_lastErrorCode = m_device->vkAllocateDescriptorSets(*m_device, &createInfo, handles.data());
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
				return std::vector<DescriptorSet>();

			std::vector<DescriptorSet> descriptorSets;
			for (UInt32 i = 0; i < descriptorSetCount; ++i)
				descriptorSets.emplace_back(DescriptorSet(*this, handles[i]));

			return descriptorSets;
		}
	}
}
