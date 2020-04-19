// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Texture.hpp>
#include <cassert>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	inline Texture::~Texture()
	{
		Destroy();
	}

	inline bool Texture::Create(OpenGLDevice& device)
	{
		Destroy();

		m_device = &device;

		const Context& context = EnsureDeviceContext();
		context.glGenTextures(1U, &m_texture.Get());
		if (!m_texture)
			return false; //< TODO: Handle error messages

		return true;
	}

	inline void Texture::Destroy()
	{
		if (m_texture)
		{
			const Context& context = EnsureDeviceContext();
			context.glDeleteTextures(1U, &m_texture.Get());

			m_device->NotifyTextureDestruction(m_texture);

			m_texture = 0;
		}
	}

	inline void Texture::TexImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border)
	{
		return TexImage2D(level, internalFormat, width, height, border, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	}

	inline void Texture::TexImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data)
	{
		const Context& context = EnsureDeviceContext();
		context.BindTexture(TextureTarget::Target2D, m_texture);

		context.glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, format, type, data);
		//< TODO: Handle errors
	}

	inline void Texture::TexSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* data)
	{
		const Context& context = EnsureDeviceContext();
		context.BindTexture(TextureTarget::Target2D, m_texture);

		context.glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, data);
		//< TODO: Handle errors
	}

	inline const Context& Texture::EnsureDeviceContext()
	{
		assert(m_device);

		const Context* activeContext = Context::GetCurrentContext();
		if (!activeContext || activeContext->GetDevice() != m_device)
		{
			const Context& referenceContext = m_device->GetReferenceContext();
			if (!Context::SetCurrentContext(&referenceContext))
				throw std::runtime_error("failed to activate context");

			return referenceContext;
		}

		return *activeContext;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
