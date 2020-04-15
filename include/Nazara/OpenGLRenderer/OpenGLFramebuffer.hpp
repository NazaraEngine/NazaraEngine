// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLFRAMEBUFFER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLFRAMEBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Framebuffer.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Framebuffer.hpp>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLFramebuffer : public Framebuffer
	{
		public:
			enum class Type
			{
				Multiple,
				Single
			};

			inline OpenGLFramebuffer(Type type);
			OpenGLFramebuffer(const OpenGLFramebuffer&) = delete;
			OpenGLFramebuffer(OpenGLFramebuffer&&) noexcept = default;
			~OpenGLFramebuffer() = default;

			inline Type GetType() const;

			OpenGLFramebuffer& operator=(const OpenGLFramebuffer&) = delete;
			OpenGLFramebuffer& operator=(OpenGLFramebuffer&&) noexcept = default;

		private:
			Type m_type;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLFramebuffer.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLFRAMEBUFFER_HPP
