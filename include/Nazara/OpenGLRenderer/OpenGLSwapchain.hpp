// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLSWAPCHAIN_HPP
#define NAZARA_OPENGLRENDERER_OPENGLSWAPCHAIN_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderImage.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPass.hpp>
#include <Nazara/OpenGLRenderer/OpenGLWindowFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Renderer/Swapchain.hpp>
#include <Nazara/Renderer/SwapchainParameters.hpp>
#include <optional>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLSwapchain final : public Swapchain
	{
		public:
			OpenGLSwapchain(OpenGLDevice& device, WindowHandle windowHandle, const Vector2ui& windowSize, const SwapchainParameters& parameters);
			~OpenGLSwapchain() = default;

			RenderFrame AcquireFrame() override;

			std::shared_ptr<CommandPool> CreateCommandPool(QueueType queueType) override;

			inline GL::Context& GetContext();
			inline OpenGLDevice& GetDevice();
			const OpenGLFramebuffer& GetFramebuffer(std::size_t imageIndex) const override;
			std::size_t GetFramebufferCount() const override;
			PresentMode GetPresentMode() const override;
			const OpenGLRenderPass& GetRenderPass() const override;
			const Vector2ui& GetSize() const override;
			PresentModeFlags GetSupportedPresentModes() const override;

			void NotifyResize(const Vector2ui& newSize) override;

			void Present();

			void SetPresentMode(PresentMode presentMode) override;

			RenderResources& GetTransientResources() override;

		private:
			std::optional<OpenGLRenderPass> m_renderPass;
			std::shared_ptr<GL::Context> m_context;
			std::vector<std::unique_ptr<OpenGLRenderImage>> m_renderImage;
			OpenGLDevice& m_device;
			OpenGLWindowFramebuffer m_framebuffer;
			PresentMode m_presentMode;
			PresentModeFlags m_supportedPresentModes;
			UInt32 m_currentImageIndex;
			Vector2ui m_size;
			bool m_sizeInvalidated;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLSwapchain.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLSWAPCHAIN_HPP
