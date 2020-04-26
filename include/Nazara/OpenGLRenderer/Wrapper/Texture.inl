// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Texture.hpp>
#include <cassert>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	inline void Texture::TexImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border)
	{
		return TexImage2D(level, internalFormat, width, height, border, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	}

	inline void Texture::TexImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data)
	{
		const Context& context = EnsureDeviceContext();
		context.BindTexture(TextureTarget::Target2D, m_objectId);

		context.glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, format, type, data);
		//< TODO: Handle errors
	}

	inline void Texture::TexSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* data)
	{
		const Context& context = EnsureDeviceContext();
		context.BindTexture(TextureTarget::Target2D, m_objectId);

		context.glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, data);
		//< TODO: Handle errors
	}

	inline GLuint Texture::CreateHelper(OpenGLDevice& device, const Context& context)
	{
		GLuint sampler = 0;
		context.glGenTextures(1U, &sampler);

		return sampler;
	}

	inline void Texture::DestroyHelper(OpenGLDevice& device, const Context& context, GLuint objectId)
	{
		context.glDeleteTextures(1U, &objectId);

		device.NotifyTextureDestruction(objectId);
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
