// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLRENDERWINDOW_HPP
#define NAZARA_OPENGLRENDERER_OPENGLRENDERWINDOW_HPP

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

	class NAZARA_OPENGLRENDERER_API OpenGLRenderWindow final : public RenderWindowImpl
	{
		public:
			OpenGLRenderWindow(RenderWindow& owner);
			~OpenGLRenderWindow() = default;

			RenderFrame Acquire() override;

			bool Create(RendererImpl* renderer, RenderSurface* surface, const RenderWindowParameters& parameters) override;
			std::shared_ptr<CommandPool> CreateCommandPool(QueueType queueType) override;

			inline GL::Context& GetContext();
			const OpenGLFramebuffer& GetFramebuffer(std::size_t i) const override;
			std::size_t GetFramebufferCount() const override;
			const OpenGLRenderPass& GetRenderPass() const override;
			const Vector2ui& GetSize() const override;

			void Present();

		private:
			std::optional<OpenGLRenderPass> m_renderPass;
			std::size_t m_currentFrame;
			std::vector<std::unique_ptr<OpenGLRenderImage>> m_renderImage;
			std::unique_ptr<GL::Context> m_context;
			OpenGLWindowFramebuffer m_framebuffer;
			RenderWindow& m_owner;
			Vector2ui m_size;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderWindow.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLRENDERWINDOW_HPP
