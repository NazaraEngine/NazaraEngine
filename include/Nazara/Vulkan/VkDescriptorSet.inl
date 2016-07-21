// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkDescriptorSet.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Vulkan/VkInstance.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline DescriptorSet::DescriptorSet() :
		m_pool(),
		m_handle(VK_NULL_HANDLE)
		{
		}

		inline DescriptorSet::DescriptorSet(DescriptorPool& pool, VkDescriptorSet handle) :
		m_pool(&pool),
		m_handle(handle)
		{
		}

		inline DescriptorSet::DescriptorSet(DescriptorSet&& descriptorSet) :
		m_pool(std::move(descriptorSet.m_pool)),
		m_allocator(descriptorSet.m_allocator),
		m_handle(descriptorSet.m_handle),
		m_lastErrorCode(descriptorSet.m_lastErrorCode)
		{
			descriptorSet.m_handle = VK_NULL_HANDLE;
		}

		inline DescriptorSet::~DescriptorSet()
		{
			Free();
		}

		inline void DescriptorSet::Free()
		{
			if (m_handle)
				m_pool->GetDevice()->vkFreeDescriptorSets(*m_pool->GetDevice(), *m_pool, 1, &m_handle);
		}

		inline VkResult DescriptorSet::GetLastErrorCode() const
		{
			return m_lastErrorCode;
		}

		inline void DescriptorSet::WriteUniformDescriptor(UInt32 binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
		{
			return WriteUniformDescriptor(binding, 0U, buffer, offset, range);
		}

		inline void DescriptorSet::WriteUniformDescriptor(UInt32 binding, const VkDescriptorBufferInfo& bufferInfo)
		{
			return WriteUniformDescriptors(binding, 0U, 1U, &bufferInfo);
		}

		inline void DescriptorSet::WriteUniformDescriptor(UInt32 binding, UInt32 arrayElement, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
		{
			VkDescriptorBufferInfo bufferInfo =
			{
				buffer, // VkBuffer        buffer;
				offset, // VkDeviceSize    offset;
				range   // VkDeviceSize    range;
			};

			return WriteUniformDescriptor(binding, arrayElement, bufferInfo);
		}

		inline void DescriptorSet::WriteUniformDescriptor(UInt32 binding, UInt32 arrayElement, const VkDescriptorBufferInfo& bufferInfo)
		{
			return WriteUniformDescriptors(binding, arrayElement, 1U, &bufferInfo);
		}

		inline void DescriptorSet::WriteUniformDescriptors(UInt32 binding, UInt32 descriptorCount, const VkDescriptorBufferInfo* bufferInfo)
		{
			return WriteUniformDescriptors(binding, 0U, descriptorCount, bufferInfo);
		}

		inline void DescriptorSet::WriteUniformDescriptors(UInt32 binding, UInt32 arrayElement, UInt32 descriptorCount, const VkDescriptorBufferInfo* bufferInfo)
		{
			VkWriteDescriptorSet writeDescriptorSet =
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, // VkStructureType                  sType;
				nullptr,                                // const void*                      pNext;
				m_handle,                               // VkDescriptorSet                  dstSet;
				binding,                                // uint32_t                         dstBinding;
				arrayElement,                           // uint32_t                         dstArrayElement;
				descriptorCount,                        // uint32_t                         descriptorCount;
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,      // VkDescriptorType                 descriptorType;
				nullptr,                                // const VkDescriptorImageInfo*     pImageInfo;
				bufferInfo,                             // const VkDescriptorBufferInfo*    pBufferInfo;
				nullptr                                 // const VkBufferView*              pTexelBufferView;
			};

			return m_pool->GetDevice()->vkUpdateDescriptorSets(*m_pool->GetDevice(), 1U, &writeDescriptorSet, 0U, nullptr);
		}

		inline DescriptorSet& DescriptorSet::operator=(DescriptorSet&& descriptorSet)
		{
			m_allocator = descriptorSet.m_allocator;
			m_handle = descriptorSet.m_handle;
			m_lastErrorCode = descriptorSet.m_lastErrorCode;
			m_pool = std::move(descriptorSet.m_pool);
			m_handle = descriptorSet.m_handle;
			
			descriptorSet.m_handle = VK_NULL_HANDLE;

			return *this;
		}

		inline DescriptorSet::operator VkDescriptorSet() const
		{
			return m_handle;
		}
	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
