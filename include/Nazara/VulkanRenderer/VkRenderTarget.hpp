// Copyright (C) 201 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_RENDERTARGET_HPP
#define NAZARA_VULKANRENDERER_RENDERTARGET_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Framebuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/RenderPass.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Semaphore.hpp>
#include <unordered_map>
#include <vulkan/vulkan_core.h>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VkRenderTarget
	{
		public:
			VkRenderTarget() = default;
			VkRenderTarget(const VkRenderTarget&) = delete;
			VkRenderTarget(VkRenderTarget&&) = delete; ///TOOD?
			virtual ~VkRenderTarget();

			VkRenderTarget& operator=(const VkRenderTarget&) = delete;
			VkRenderTarget& operator=(VkRenderTarget&&) = delete; ///TOOD?

			// Signals:
			NazaraSignal(OnRenderTargetRelease,	const VkRenderTarget* /*renderTarget*/);
			NazaraSignal(OnRenderTargetSizeChange, const VkRenderTarget* /*renderTarget*/);
	};
}

#include <Nazara/VulkanRenderer/VkRenderTarget.inl>

#endif // NAZARA_VULKANRENDERER_RENDERTARGET_HPP
