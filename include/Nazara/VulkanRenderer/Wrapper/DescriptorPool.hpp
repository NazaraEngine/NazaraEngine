// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKDESCRIPTORPOOL_HPP
#define NAZARA_VULKANRENDERER_VKDESCRIPTORPOOL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class DescriptorPool;
		class DescriptorSet;

		using DescriptorPoolHandle = ObjectHandle<DescriptorPool>;

		class NAZARA_VULKANRENDERER_API DescriptorPool : public DeviceObject<DescriptorPool, VkDescriptorPool, VkDescriptorPoolCreateInfo>, public HandledObject<DescriptorPool>
		{
			friend DeviceObject;

			public:
				DescriptorPool() = default;
				DescriptorPool(const DescriptorPool&) = delete;
				DescriptorPool(DescriptorPool&&) = default;
				~DescriptorPool() = default;

				DescriptorSet AllocateDescriptorSet(const VkDescriptorSetLayout& setLayouts);
				std::vector<DescriptorSet> AllocateDescriptorSets(UInt32 descriptorSetCount, const VkDescriptorSetLayout* setLayouts);

				using DeviceObject::Create;
				inline bool Create(const DeviceHandle& device, UInt32 maxSets, const VkDescriptorPoolSize& poolSize, VkDescriptorPoolCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);
				inline bool Create(const DeviceHandle& device, UInt32 maxSets, UInt32 poolSizeCount, const VkDescriptorPoolSize* poolSize, VkDescriptorPoolCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);

				DescriptorPool& operator=(const DescriptorPool&) = delete;
				DescriptorPool& operator=(DescriptorPool&&) = delete;

			private:
				static inline VkResult CreateHelper(const DeviceHandle& device, const VkDescriptorPoolCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkDescriptorPool* handle);
				static inline void DestroyHelper(const DeviceHandle& device, VkDescriptorPool handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/DescriptorPool.inl>

#endif // NAZARA_VULKANRENDERER_VKDESCRIPTORPOOL_HPP
