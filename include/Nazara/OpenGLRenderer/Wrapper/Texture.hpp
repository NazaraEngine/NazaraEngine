// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_TEXTURE_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_TEXTURE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/DeviceObject.hpp>
#include <NazaraUtils/MovableValue.hpp>

namespace Nz::GL
{
	class Texture : public DeviceObject<Texture, GL_TEXTURE>
	{
		friend DeviceObject;

		public:
			using DeviceObject::DeviceObject;
			Texture(const Texture&) = delete;
			Texture(Texture&&) noexcept = default;
			~Texture() = default;

			inline void GenerateMipmap();

			inline TextureTarget GetTarget() const;

			inline void SetParameterf(GLenum pname, GLfloat param);
			inline void SetParameteri(GLenum pname, GLint param);
			inline void SetParameterfv(GLenum pname, const GLfloat* param);
			inline void SetParameteriv(GLenum pname, const GLint* param);

			inline void TexImage2D(TextureTarget target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type);
			inline void TexImage2D(TextureTarget target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data);
			inline void TexImage3D(TextureTarget target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type);
			inline void TexImage3D(TextureTarget target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* data);
			inline void TexStorage2D(TextureTarget target, GLint levels, GLint internalFormat, GLsizei width, GLsizei height);
			inline void TexStorage3D(TextureTarget target, GLint levels, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth);
			inline void TexSubImage2D(TextureTarget target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* data);
			inline void TexSubImage3D(TextureTarget target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* data);
			inline void TextureView(TextureTarget target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers);

			Texture& operator=(const Texture&) = delete;
			Texture& operator=(Texture&&) noexcept = default;

		private:
			static inline GLuint CreateHelper(OpenGLDevice& device, const Context& context);
			static inline void DestroyHelper(OpenGLDevice& device, const Context& context, GLuint objectId);

			TextureTarget m_target;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Texture.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_TEXTURE_HPP
