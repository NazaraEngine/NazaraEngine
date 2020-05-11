// Copyright (C) 2015 Jérôme Leclercq
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
	class NAZARA_OPENGLRENDERER_API OpenGLRenderWindow : public RenderWindowImpl
	{
		public:
			OpenGLRenderWindow();
			~OpenGLRenderWindow() = default;

			OpenGLRenderImage& Acquire() override;

			bool Create(RendererImpl* renderer, RenderSurface* surface, const Vector2ui& size, const RenderWindowParameters& parameters) override;
			std::unique_ptr<CommandPool> CreateCommandPool(QueueType queueType) override;

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
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderWindow.inl>

#endif // NAZARA_OPENGLRENDERER_RENDERWINDOW_HPP
