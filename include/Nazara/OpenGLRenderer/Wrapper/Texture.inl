// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Texture.hpp>
#include <cassert>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	inline void Texture::SetParameterf(GLenum pname, GLfloat param)
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.BindTexture(m_target, m_objectId);
		context.glTexParameterf(ToOpenGL(m_target), pname, param);
	}

	inline void Texture::SetParameteri(GLenum pname, GLint param)
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.BindTexture(m_target, m_objectId);
		context.glTexParameteri(ToOpenGL(m_target), pname, param);
	}

	inline void Texture::SetParameterfv(GLenum pname, const GLfloat* param)
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.BindTexture(m_target, m_objectId);
		context.glTexParameterfv(ToOpenGL(m_target), pname, param);
	}

	inline void Texture::SetParameteriv(GLenum pname, const GLint* param)
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.BindTexture(m_target, m_objectId);
		context.glTexParameteriv(ToOpenGL(m_target), pname, param);
	}

	inline void Texture::TexImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type)
	{
		return TexImage2D(level, internalFormat, width, height, border, format, type, nullptr);
	}

	inline void Texture::TexImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data)
	{
		m_target = TextureTarget::Target2D;

		const Context& context = EnsureDeviceContext();
		context.BindTexture(m_target, m_objectId);
		context.glTexImage2D(ToOpenGL(m_target), level, internalFormat, width, height, border, format, type, data);
		//< TODO: Handle errors
	}

	inline void Texture::TexSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* data)
	{
		const Context& context = EnsureDeviceContext();
		context.BindTexture(m_target, m_objectId);
		context.glTexSubImage2D(ToOpenGL(m_target), level, xoffset, yoffset, width, height, format, type, data);
		//< TODO: Handle errors
	}

	inline GLuint Texture::CreateHelper(OpenGLDevice& device, const Context& context)
	{
		GLuint texture = 0;
		context.glGenTextures(1U, &texture);

		return texture;
	}

	inline void Texture::DestroyHelper(OpenGLDevice& device, const Context& context, GLuint objectId)
	{
		context.glDeleteTextures(1U, &objectId);

		device.NotifyTextureDestruction(objectId);
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
