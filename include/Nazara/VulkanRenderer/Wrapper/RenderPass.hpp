// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_RENDERPASS_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_RENDERPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class RenderPass : public DeviceObject<RenderPass, VkRenderPass, VkRenderPassCreateInfo, VK_OBJECT_TYPE_RENDER_PASS>
		{
			friend DeviceObject;

			public:
				RenderPass() = default;
				RenderPass(const RenderPass&) = delete;
				RenderPass(RenderPass&&) = default;
				~RenderPass() = default;

				RenderPass& operator=(const RenderPass&) = delete;
				RenderPass& operator=(RenderPass&&) = default;

			private:
				static inline VkResult CreateHelper(Device& device, const VkRenderPassCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkRenderPass* handle);
				static inline void DestroyHelper(Device& device, VkRenderPass handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/RenderPass.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_RENDERPASS_HPP
