// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_GLTEXTURE_HPP
#define NAZARA_OPENGLRENDERER_GLTEXTURE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovableValue.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>

namespace Nz::GL
{
	class Texture
	{
		public:
			Texture() = default;
			Texture(const Texture&) = delete;
			Texture(Texture&&) noexcept = default;
			inline ~Texture();

			inline bool Create(OpenGLDevice& device);
			inline void Destroy();

			inline void TexImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border);
			inline void TexImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data);
			inline void TexSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* data);

			Texture& operator=(const Texture&) = delete;
			Texture& operator=(Texture&&) noexcept = default;

		private:
			const Context& EnsureDeviceContext();

			MovablePtr<OpenGLDevice> m_device;
			MovableValue<GLuint> m_texture;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Texture.inl>

#endif
