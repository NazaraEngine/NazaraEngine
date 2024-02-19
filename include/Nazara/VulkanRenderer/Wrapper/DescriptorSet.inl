// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Instance.hpp>
#include <cassert>

namespace Nz::Vk
{
	inline DescriptorSet::DescriptorSet() :
	m_pool(nullptr),
	m_handle(VK_NULL_HANDLE)
	{
	}

	inline DescriptorSet::DescriptorSet(DescriptorPool& pool, VkDescriptorSet handle) :
	m_pool(&pool),
	m_handle(handle)
	{
	}

	inline DescriptorSet::DescriptorSet(DescriptorSet&& descriptorSet) noexcept :
	m_pool(descriptorSet.m_pool),
	m_handle(descriptorSet.m_handle)
	{
		descriptorSet.m_handle = VK_NULL_HANDLE;
	}

	inline void DescriptorSet::Free()
	{
		if (m_handle)
		{
			assert(m_pool);
			m_pool->GetDevice()->vkFreeDescriptorSets(*m_pool->GetDevice(), *m_pool, 1, &m_handle);
		}
	}

	inline bool DescriptorSet::IsValid() const
	{
		return m_handle != VK_NULL_HANDLE;
	}

	inline void DescriptorSet::WriteCombinedImageSamplerDescriptor(UInt32 binding, VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout)
	{
		return WriteCombinedImageSamplerDescriptor(binding, 0, sampler, imageView, imageLayout);
	}

	inline void DescriptorSet::WriteCombinedImageSamplerDescriptor(UInt32 binding, const VkDescriptorImageInfo& imageInfo)
	{
		return WriteCombinedImageSamplerDescriptor(binding, 0, imageInfo);
	}

	inline void DescriptorSet::WriteCombinedImageSamplerDescriptor(UInt32 binding, UInt32 arrayElement, VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout)
	{
		VkDescriptorImageInfo imageInfo = {
			sampler,
			imageView,
			imageLayout
		};

		return WriteCombinedImageSamplerDescriptors(binding, arrayElement, 1U, &imageInfo);
	}

	inline void DescriptorSet::WriteCombinedImageSamplerDescriptor(UInt32 binding, UInt32 arrayElement, const VkDescriptorImageInfo& imageInfo)
	{
		return WriteCombinedImageSamplerDescriptors(binding, arrayElement, 1U, &imageInfo);
	}

	inline void DescriptorSet::WriteCombinedImageSamplerDescriptors(UInt32 binding, UInt32 descriptorCount, const VkDescriptorImageInfo* imageInfo)
	{
		return WriteCombinedImageSamplerDescriptors(binding, 0U, descriptorCount, imageInfo);
	}

	inline void DescriptorSet::WriteCombinedImageSamplerDescriptors(UInt32 binding, UInt32 arrayElement, UInt32 descriptorCount, const VkDescriptorImageInfo* imageInfo)
	{
		VkWriteDescriptorSet writeDescriptorSet = {
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			m_handle,
			binding,
			arrayElement,
			descriptorCount,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			imageInfo,
			nullptr,
			nullptr
		};

		return m_pool->GetDevice()->vkUpdateDescriptorSets(*m_pool->GetDevice(), 1U, &writeDescriptorSet, 0U, nullptr);
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
		VkDescriptorBufferInfo bufferInfo = {
			buffer,
			offset,
			range
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
		VkWriteDescriptorSet writeDescriptorSet = {
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			m_handle,
			binding,
			arrayElement,
			descriptorCount,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			nullptr,
			bufferInfo,
			nullptr
		};

		return m_pool->GetDevice()->vkUpdateDescriptorSets(*m_pool->GetDevice(), 1U, &writeDescriptorSet, 0U, nullptr);
	}

	inline DescriptorSet& DescriptorSet::operator=(DescriptorSet&& descriptorSet) noexcept
	{
		std::swap(m_handle, descriptorSet.m_handle);
		std::swap(m_pool, descriptorSet.m_pool);

		return *this;
	}

	inline DescriptorSet::operator bool() const
	{
		return IsValid();
	}

	inline DescriptorSet::operator VkDescriptorSet() const
	{
		return m_handle;
	}
}

