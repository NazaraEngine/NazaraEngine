// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKPIPELINELAYOUT_HPP
#define NAZARA_VULKAN_VKPIPELINELAYOUT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/VkDeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class PipelineLayout : public DeviceObject<PipelineLayout, VkPipelineLayout, VkPipelineLayoutCreateInfo>
		{
			friend DeviceObject;

			public:
				PipelineLayout() = default;
				PipelineLayout(const PipelineLayout&) = delete;
				PipelineLayout(PipelineLayout&&) = default;
				~PipelineLayout() = default;

				PipelineLayout& operator=(const PipelineLayout&) = delete;
				PipelineLayout& operator=(PipelineLayout&&) = delete;

			private:
				static inline VkResult CreateHelper(const DeviceHandle& device, const VkPipelineLayoutCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkPipelineLayout* handle);
				static inline void DestroyHelper(const DeviceHandle& device, VkPipelineLayout handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/Vulkan/VkPipelineLayout.inl>

#endif // NAZARA_VULKAN_VKPIPELINELAYOUT_HPP
