// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_SWAPCHAIN_HPP
#define NAZARA_RENDERER_SWAPCHAIN_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
#include <NazaraUtils/Signal.hpp>
#include <vector>

namespace Nz
{
	class CommandPool;
	class Framebuffer;
	class RenderDevice;
	class RenderResources;

	class NAZARA_RENDERER_API Swapchain
	{
		public:
			Swapchain() = default;
			virtual ~Swapchain();

			virtual RenderFrame AcquireFrame() = 0;

			virtual std::shared_ptr<CommandPool> CreateCommandPool(QueueType queueType) = 0;

			virtual const Framebuffer& GetFramebuffer(std::size_t i) const = 0;
			virtual std::size_t GetFramebufferCount() const = 0;
			virtual PresentMode GetPresentMode() const = 0;
			virtual const RenderPass& GetRenderPass() const = 0;
			virtual const Vector2ui& GetSize() const = 0;
			virtual PresentModeFlags GetSupportedPresentModes() const = 0;
			virtual RenderResources& GetTransientResources() = 0;

			virtual void NotifyResize(const Vector2ui& newSize) = 0;

			virtual void SetPresentMode(PresentMode presentMode) = 0;

			NazaraSignal(OnSwapchainResize, Swapchain* /*swapchain*/, const Vector2ui& /*newSize*/);

		protected:
			static void BuildRenderPass(PixelFormat colorFormat, PixelFormat depthFormat, std::vector<RenderPass::Attachment>& attachments, std::vector<RenderPass::SubpassDescription>& subpassDescriptions, std::vector<RenderPass::SubpassDependency>& subpassDependencies);
	};
}

#endif // NAZARA_RENDERER_SWAPCHAIN_HPP
