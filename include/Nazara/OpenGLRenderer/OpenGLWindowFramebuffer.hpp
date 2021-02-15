// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLWINDOWFRAMEBUFFER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLWINDOWFRAMEBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/OpenGLFramebuffer.hpp>

namespace Nz
{
	class OpenGLRenderWindow;

	class NAZARA_OPENGLRENDERER_API OpenGLWindowFramebuffer : public OpenGLFramebuffer
	{
		public:
			inline OpenGLWindowFramebuffer(OpenGLRenderWindow& renderWindow);
			OpenGLWindowFramebuffer(const OpenGLWindowFramebuffer&) = delete;
			OpenGLWindowFramebuffer(OpenGLWindowFramebuffer&&) noexcept = default;
			~OpenGLWindowFramebuffer() = default;

			void Activate() const override;

			std::size_t GetColorBufferCount() const override;

			OpenGLWindowFramebuffer& operator=(const OpenGLWindowFramebuffer&) = delete;
			OpenGLWindowFramebuffer& operator=(OpenGLWindowFramebuffer&&) = delete;

		private:
			OpenGLRenderWindow& m_renderWindow;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLWindowFramebuffer.inl>

#endif // NAZARA_OPENGLRENDERER_OpenGLWindowFramebuffer_HPP
