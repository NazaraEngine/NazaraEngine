// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/PipelineLayout.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline bool PipelineLayout::Create(DeviceHandle device, VkDescriptorSetLayout layout, VkPipelineLayoutCreateFlags flags)
		{
			return Create(std::move(device), 1U, &layout, flags);
		}

		inline bool PipelineLayout::Create(DeviceHandle device, UInt32 layoutCount, const VkDescriptorSetLayout* layouts, VkPipelineLayoutCreateFlags flags)
		{
			VkPipelineLayoutCreateInfo createInfo = {
				VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
				nullptr,
				flags,
				layoutCount,
				layouts,
				0U,
				nullptr
			};

			return Create(std::move(device), createInfo);
		}

		inline VkResult PipelineLayout::CreateHelper(const DeviceHandle& device, const VkPipelineLayoutCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkPipelineLayout* handle)
		{
			return device->vkCreatePipelineLayout(*device, createInfo, allocator, handle);
		}

		inline void PipelineLayout::DestroyHelper(const DeviceHandle& device, VkPipelineLayout handle, const VkAllocationCallbacks* allocator)
		{
			return device->vkDestroyPipelineLayout(*device, handle, allocator);
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
