// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_DESCRIPTORPOOL_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_DESCRIPTORPOOL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz
{
	namespace Vk
	{
		class DescriptorSet;

		class NAZARA_VULKANRENDERER_API DescriptorPool : public DeviceObject<DescriptorPool, VkDescriptorPool, VkDescriptorPoolCreateInfo, VK_OBJECT_TYPE_DESCRIPTOR_POOL>
		{
			friend DeviceObject;

			public:
				DescriptorPool() = default;
				DescriptorPool(const DescriptorPool&) = delete;
				DescriptorPool(DescriptorPool&&) noexcept = default;
				~DescriptorPool() = default;

				DescriptorSet AllocateDescriptorSet(const VkDescriptorSetLayout& setLayouts);
				std::vector<DescriptorSet> AllocateDescriptorSets(UInt32 descriptorSetCount, const VkDescriptorSetLayout* setLayouts);

				using DeviceObject::Create;
				inline bool Create(Device& device, UInt32 maxSets, const VkDescriptorPoolSize& poolSize, VkDescriptorPoolCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);
				inline bool Create(Device& device, UInt32 maxSets, UInt32 poolSizeCount, const VkDescriptorPoolSize* poolSize, VkDescriptorPoolCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);

				DescriptorPool& operator=(const DescriptorPool&) = delete;
				DescriptorPool& operator=(DescriptorPool&&) = delete;

			private:
				static inline VkResult CreateHelper(Device& device, const VkDescriptorPoolCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkDescriptorPool* handle);
				static inline void DestroyHelper(Device& device, VkDescriptorPool handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/DescriptorPool.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_DESCRIPTORPOOL_HPP
