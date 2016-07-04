// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKPIPELINE_HPP
#define NAZARA_VULKAN_VKPIPELINE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/VkDeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class Pipeline
		{
			public:
				inline Pipeline();
				Pipeline(const Pipeline&) = delete;
				Pipeline(Pipeline&&);
				inline ~Pipeline();

				inline bool CreateCompute(const DeviceHandle& device, const VkComputePipelineCreateInfo& createInfo, VkPipelineCache cache = VK_NULL_HANDLE, const VkAllocationCallbacks* allocator = nullptr);
				inline bool CreateGraphics(const DeviceHandle& device, const VkGraphicsPipelineCreateInfo& createInfo, VkPipelineCache cache = VK_NULL_HANDLE, const VkAllocationCallbacks* allocator = nullptr);
				inline void Destroy();

				inline const DeviceHandle& GetDevice() const;
				inline VkResult GetLastErrorCode() const;

				Pipeline& operator=(const Pipeline&) = delete;
				Pipeline& operator=(Pipeline&&) = delete;

				inline operator VkPipeline() const;

			protected:
				inline bool Create(const DeviceHandle& device, VkResult result, const VkAllocationCallbacks* allocator);

				DeviceHandle m_device;
				VkAllocationCallbacks m_allocator;
				VkPipeline m_handle;
				mutable VkResult m_lastErrorCode;
		};
	}
}

#include <Nazara/Vulkan/VkPipeline.inl>

#endif // NAZARA_VULKAN_VKPIPELINE_HPP
