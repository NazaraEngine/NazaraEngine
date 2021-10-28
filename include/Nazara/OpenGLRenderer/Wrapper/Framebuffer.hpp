// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_FRAMEBUFFER_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_FRAMEBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/ContextObject.hpp>

namespace Nz::GL
{
	class Framebuffer : public ContextObject<Framebuffer, GL_FRAMEBUFFER>
	{
		friend ContextObject;

		public:
			using ContextObject::ContextObject;
			Framebuffer(const Framebuffer&) = delete;
			Framebuffer(Framebuffer&&) noexcept = default;
			~Framebuffer() = default;

			inline GLenum Check() const;

			inline void Renderbuffer(GLenum attachment, GLenum renderbuffer);
			inline void Texture2D(GLenum attachment, GLenum textarget, GLuint texture, GLint level = 0);

			Framebuffer& operator=(const Framebuffer&) = delete;
			Framebuffer& operator=(Framebuffer&&) noexcept = default;

		private:
			static inline GLuint CreateHelper(const Context& context);
			static inline void DestroyHelper(const Context& context, GLuint objectId);
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Framebuffer.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_FRAMEBUFFER_HPP
