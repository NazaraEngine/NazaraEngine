// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_DESCRIPTORSET_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_DESCRIPTORSET_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/VulkanRenderer/Wrapper/AutoFree.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DescriptorPool.hpp>
#include <vulkan/vulkan_core.h>

namespace Nz
{
	namespace Vk
	{
		class DescriptorSet
		{
			friend DescriptorPool;

			public:
				inline DescriptorSet();
				DescriptorSet(const DescriptorSet&) = delete;
				inline DescriptorSet(DescriptorSet&& descriptorSet) noexcept;
				~DescriptorSet() = default;

				inline void Free();

				inline bool IsValid() const;

				inline void WriteCombinedImageSamplerDescriptor(UInt32 binding, VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout);
				inline void WriteCombinedImageSamplerDescriptor(UInt32 binding, const VkDescriptorImageInfo& imageInfo);
				inline void WriteCombinedImageSamplerDescriptor(UInt32 binding, UInt32 arrayElement, VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout);
				inline void WriteCombinedImageSamplerDescriptor(UInt32 binding, UInt32 arrayElement, const VkDescriptorImageInfo& imageInfo);
				inline void WriteCombinedImageSamplerDescriptors(UInt32 binding, UInt32 descriptorCount, const VkDescriptorImageInfo* imageInfo);
				inline void WriteCombinedImageSamplerDescriptors(UInt32 binding, UInt32 arrayElement, UInt32 descriptorCount, const VkDescriptorImageInfo* imageInfo);

				inline void WriteUniformDescriptor(UInt32 binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);
				inline void WriteUniformDescriptor(UInt32 binding, const VkDescriptorBufferInfo& bufferInfo);
				inline void WriteUniformDescriptor(UInt32 binding, UInt32 arrayElement, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);
				inline void WriteUniformDescriptor(UInt32 binding, UInt32 arrayElement, const VkDescriptorBufferInfo& bufferInfo);
				inline void WriteUniformDescriptors(UInt32 binding, UInt32 descriptorCount, const VkDescriptorBufferInfo* bufferInfo);
				inline void WriteUniformDescriptors(UInt32 binding, UInt32 arrayElement, UInt32 descriptorCount, const VkDescriptorBufferInfo* bufferInfo);

				DescriptorSet& operator=(const DescriptorSet&) = delete;
				inline DescriptorSet& operator=(DescriptorSet&& descriptorSet) noexcept;

				inline explicit operator bool() const;
				inline operator VkDescriptorSet() const;

			private:
				inline DescriptorSet(DescriptorPool& pool, VkDescriptorSet handle);

				DescriptorPool* m_pool;
				VkDescriptorSet m_handle;
		};

		class AutoDescriptorSet : public AutoFree<DescriptorSet>
		{
			public:
				using AutoFree::AutoFree;

				explicit operator bool() const { return Get(); }
				operator VkDescriptorSet() const { return Get(); }
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/DescriptorSet.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_DESCRIPTORSET_HPP
