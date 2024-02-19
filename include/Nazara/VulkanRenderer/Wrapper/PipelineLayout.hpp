// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_PIPELINELAYOUT_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_PIPELINELAYOUT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz
{
	namespace Vk
	{
		class PipelineLayout : public DeviceObject<PipelineLayout, VkPipelineLayout, VkPipelineLayoutCreateInfo, VK_OBJECT_TYPE_PIPELINE_LAYOUT>
		{
			friend DeviceObject;

			public:
				PipelineLayout() = default;
				PipelineLayout(const PipelineLayout&) = delete;
				PipelineLayout(PipelineLayout&&) = default;
				~PipelineLayout() = default;

				using DeviceObject::Create;
				bool Create(Device& device, VkDescriptorSetLayout layout, VkPipelineLayoutCreateFlags flags = 0);
				bool Create(Device& device, UInt32 layoutCount, const VkDescriptorSetLayout* layouts, VkPipelineLayoutCreateFlags flags = 0);

				PipelineLayout& operator=(const PipelineLayout&) = delete;
				PipelineLayout& operator=(PipelineLayout&&) = delete;

			private:
				static inline VkResult CreateHelper(Device& device, const VkPipelineLayoutCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkPipelineLayout* handle);
				static inline void DestroyHelper(Device& device, VkPipelineLayout handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/PipelineLayout.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_PIPELINELAYOUT_HPP
