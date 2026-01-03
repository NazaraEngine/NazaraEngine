// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLWINDOWFRAMEBUFFER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLWINDOWFRAMEBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/OpenGLFramebuffer.hpp>

namespace Nz
{
	class OpenGLSwapchain;

	class NAZARA_OPENGLRENDERER_API OpenGLWindowFramebuffer : public OpenGLFramebuffer
	{
		public:
			inline OpenGLWindowFramebuffer(OpenGLSwapchain& renderWindow);
			OpenGLWindowFramebuffer(const OpenGLWindowFramebuffer&) = delete;
			OpenGLWindowFramebuffer(OpenGLWindowFramebuffer&&) noexcept = default;
			~OpenGLWindowFramebuffer() = default;

			void Activate() const override;

			std::size_t GetColorBufferCount() const override;

			const Vector2ui& GetSize() const override;

			void UpdateDebugName(std::string_view name) override;

			OpenGLWindowFramebuffer& operator=(const OpenGLWindowFramebuffer&) = delete;
			OpenGLWindowFramebuffer& operator=(OpenGLWindowFramebuffer&&) = delete;

		private:
			OpenGLSwapchain& m_renderWindow;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLWindowFramebuffer.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLWINDOWFRAMEBUFFER_HPP
