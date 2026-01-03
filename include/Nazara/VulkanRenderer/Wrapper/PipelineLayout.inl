// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz::Vk
{
	inline bool PipelineLayout::Create(Device& device, VkDescriptorSetLayout layout, VkPipelineLayoutCreateFlags flags)
	{
		return Create(device, std::span<const VkDescriptorSetLayout>{ &layout, 1 }, flags);
	}

	inline bool PipelineLayout::Create(Device& device, std::span<const VkDescriptorSetLayout> layouts, VkPipelineLayoutCreateFlags flags)
	{
		return Create(device, layouts, {}, flags);
	}

	inline bool PipelineLayout::Create(Device& device, std::span<const VkDescriptorSetLayout> layouts, std::span<const VkPushConstantRange> pushConstantRanges, VkPipelineLayoutCreateFlags flags)
	{
		VkPipelineLayoutCreateInfo createInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			nullptr,
			flags,
			UInt32(layouts.size()),
			layouts.data(),
			UInt32(pushConstantRanges.size()),
			pushConstantRanges.data()
		};

		return Create(device, createInfo);
	}

	inline VkResult PipelineLayout::CreateHelper(Device& device, const VkPipelineLayoutCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkPipelineLayout* handle)
	{
		return device.vkCreatePipelineLayout(device, createInfo, allocator, handle);
	}

	inline void PipelineLayout::DestroyHelper(Device& device, VkPipelineLayout handle, const VkAllocationCallbacks* allocator)
	{
		return device.vkDestroyPipelineLayout(device, handle, allocator);
	}
}
