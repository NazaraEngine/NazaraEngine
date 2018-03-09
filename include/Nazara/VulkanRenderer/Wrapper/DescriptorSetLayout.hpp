// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKDESCRIPTORSETLAYOUT_HPP
#define NAZARA_VULKANRENDERER_VKDESCRIPTORSETLAYOUT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class DescriptorSetLayout : public DeviceObject<DescriptorSetLayout, VkDescriptorSetLayout, VkDescriptorSetLayoutCreateInfo>
		{
			friend DeviceObject;

			public:
				DescriptorSetLayout() = default;
				DescriptorSetLayout(const DescriptorSetLayout&) = delete;
				DescriptorSetLayout(DescriptorSetLayout&&) = default;
				~DescriptorSetLayout() = default;

				using DeviceObject::Create;
				inline bool Create(const DeviceHandle& device, const VkDescriptorSetLayoutBinding& binding, VkDescriptorSetLayoutCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);
				inline bool Create(const DeviceHandle& device, UInt32 bindingCount, const VkDescriptorSetLayoutBinding* binding, VkDescriptorSetLayoutCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);

				DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
				DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

			private:
				static inline VkResult CreateHelper(const DeviceHandle& device, const VkDescriptorSetLayoutCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkDescriptorSetLayout* handle);
				static inline void DestroyHelper(const DeviceHandle& device, VkDescriptorSetLayout handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/DescriptorSetLayout.inl>

#endif // NAZARA_VULKANRENDERER_VKDESCRIPTORSETLAYOUT_HPP
