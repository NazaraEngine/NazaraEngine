// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_RENDERWINDOW_HPP
#define NAZARA_OPENGLRENDERER_RENDERWINDOW_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderImage.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPass.hpp>
#include <Nazara/OpenGLRenderer/OpenGLWindowFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Renderer/RenderWindowImpl.hpp>
#include <optional>
#include <vector>

namespace Nz
{
	class RenderWindow;

	class NAZARA_OPENGLRENDERER_API OpenGLRenderWindow : public RenderWindowImpl
	{
		public:
			OpenGLRenderWindow(RenderWindow& owner);
			~OpenGLRenderWindow() = default;

			RenderFrame Acquire() override;

			bool Create(RendererImpl* renderer, RenderSurface* surface, const RenderWindowParameters& parameters) override;
			std::unique_ptr<CommandPool> CreateCommandPool(QueueType queueType) override;

			inline GL::Context& GetContext();
			const OpenGLFramebuffer& GetFramebuffer() const override;
			const OpenGLRenderPass& GetRenderPass() const override;

			std::shared_ptr<RenderDevice> GetRenderDevice() override;

			void Present();

		private:
			std::size_t m_currentFrame;
			std::shared_ptr<OpenGLDevice> m_device;
			std::vector<OpenGLRenderImage> m_renderImage;
			std::unique_ptr<GL::Context> m_context;
			OpenGLRenderPass m_renderPass;
			OpenGLWindowFramebuffer m_framebuffer;
			RenderWindow& m_owner;
			Vector2ui m_size;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderWindow.inl>

#endif // NAZARA_OPENGLRENDERER_RENDERWINDOW_HPP
