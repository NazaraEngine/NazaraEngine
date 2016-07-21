// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKDESCRIPTORSET_HPP
#define NAZARA_VULKAN_VKDESCRIPTORSET_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Vulkan/VkDescriptorPool.hpp>
#include <vulkan/vulkan.h>

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
				inline DescriptorSet(DescriptorSet&& descriptorSet);
				inline ~DescriptorSet();

				inline void Free();

				inline VkResult GetLastErrorCode() const;

				inline void WriteUniformDescriptor(UInt32 binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);
				inline void WriteUniformDescriptor(UInt32 binding, const VkDescriptorBufferInfo& bufferInfo);
				inline void WriteUniformDescriptor(UInt32 binding, UInt32 arrayElement, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);
				inline void WriteUniformDescriptor(UInt32 binding, UInt32 arrayElement, const VkDescriptorBufferInfo& bufferInfo);
				inline void WriteUniformDescriptors(UInt32 binding, UInt32 descriptorCount, const VkDescriptorBufferInfo* bufferInfo);
				inline void WriteUniformDescriptors(UInt32 binding, UInt32 arrayElement, UInt32 descriptorCount, const VkDescriptorBufferInfo* bufferInfo);

				DescriptorSet& operator=(const DescriptorSet&) = delete;
				DescriptorSet& operator=(DescriptorSet&& descriptorSet);

				inline operator VkDescriptorSet() const;

			private:
				inline DescriptorSet(DescriptorPool& pool, VkDescriptorSet handle);

				DescriptorPoolHandle m_pool;
				VkAllocationCallbacks m_allocator;
				VkDescriptorSet m_handle;
				VkResult m_lastErrorCode;

		};
	}
}

#include <Nazara/Vulkan/VkDescriptorSet.inl>

#endif // NAZARA_VULKAN_VKDESCRIPTORSET_HPP
