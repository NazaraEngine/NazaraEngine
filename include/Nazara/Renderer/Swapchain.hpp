// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_SWAPCHAIN_HPP
#define NAZARA_RENDERER_SWAPCHAIN_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <vector>

namespace Nz
{
	class CommandPool;
	class RenderDevice;
	class TransientResources;

	class NAZARA_RENDERER_API Swapchain : public RenderTarget
	{
		public:
			Swapchain() = default;
			virtual ~Swapchain();

			virtual RenderFrame AcquireFrame() = 0;

			virtual std::shared_ptr<CommandPool> CreateCommandPool(QueueType queueType) = 0;

			virtual PresentMode GetPresentMode() const = 0;
			virtual PresentModeFlags GetSupportedPresentModes() const = 0;

			virtual void NotifyResize(const Vector2ui& newSize) = 0;

			virtual void SetPresentMode(PresentMode presentMode) = 0;

			virtual TransientResources& Transient() = 0;

		protected:
			static void BuildRenderPass(PixelFormat colorFormat, PixelFormat depthFormat, std::vector<RenderPass::Attachment>& attachments, std::vector<RenderPass::SubpassDescription>& subpassDescriptions, std::vector<RenderPass::SubpassDependency>& subpassDependencies);
	};
}

#endif // NAZARA_RENDERER_SWAPCHAIN_HPP
