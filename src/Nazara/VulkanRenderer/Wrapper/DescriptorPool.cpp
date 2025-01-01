// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/Wrapper/DescriptorPool.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DescriptorSet.hpp>

namespace Nz
{
	namespace Vk
	{
		DescriptorSet DescriptorPool::AllocateDescriptorSet(const VkDescriptorSetLayout& setLayouts)
		{
			VkDescriptorSetAllocateInfo createInfo = {
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
				nullptr,
				m_handle,
				1U,
				&setLayouts
			};

			VkDescriptorSet handle = VK_NULL_HANDLE;
			m_lastErrorCode = m_device->vkAllocateDescriptorSets(*m_device, &createInfo, &handle);

			return DescriptorSet(*this, handle);
		}

		std::vector<DescriptorSet> DescriptorPool::AllocateDescriptorSets(UInt32 descriptorSetCount, const VkDescriptorSetLayout* setLayouts)
		{
			VkDescriptorSetAllocateInfo createInfo = {
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
				nullptr,
				m_handle,
				descriptorSetCount,
				setLayouts
			};

			std::vector<VkDescriptorSet> handles(descriptorSetCount, VK_NULL_HANDLE);
			m_lastErrorCode = m_device->vkAllocateDescriptorSets(*m_device, &createInfo, handles.data());
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
				return {};

			std::vector<DescriptorSet> descriptorSets;
			for (UInt32 i = 0; i < descriptorSetCount; ++i)
				descriptorSets.emplace_back(DescriptorSet(*this, handles[i]));

			return descriptorSets;
		}
	}
}
