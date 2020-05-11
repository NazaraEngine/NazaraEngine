// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_GLTEXTURE_HPP
#define NAZARA_OPENGLRENDERER_GLTEXTURE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovableValue.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/DeviceObject.hpp>

namespace Nz::GL
{
	class Texture : public DeviceObject<Texture, GL_TEXTURE>
	{
		friend DeviceObject;

		public:
			Texture() = default;
			Texture(const Texture&) = delete;
			Texture(Texture&&) noexcept = default;
			~Texture() = default;

			inline void SetParameterf(GLenum pname, GLfloat param);
			inline void SetParameteri(GLenum pname, GLint param);
			inline void SetParameterfv(GLenum pname, const GLfloat* param);
			inline void SetParameteriv(GLenum pname, const GLint* param);

			inline void TexImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border);
			inline void TexImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data);
			inline void TexSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* data);

			Texture& operator=(const Texture&) = delete;
			Texture& operator=(Texture&&) noexcept = default;

		private:
			static inline GLuint CreateHelper(OpenGLDevice& device, const Context& context);
			static inline void DestroyHelper(OpenGLDevice& device, const Context& context, GLuint objectId);
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Texture.inl>

#endif
