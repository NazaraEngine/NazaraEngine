// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	RenderBuffer::RenderBuffer() :
	m_id(0)
	{
	}

	RenderBuffer::~RenderBuffer()
	{
		OnRenderBufferRelease(this);

		Destroy();
	}

	bool RenderBuffer::Create(PixelFormatType format, unsigned int width, unsigned int height)
	{
		Destroy();

		#if NAZARA_RENDERER_SAFE
		if (width == 0 || height == 0)
		{
			NazaraError("Invalid size");
			return false;
		}

		if (!PixelFormat::IsValid(format))
		{
			NazaraError("Invalid pixel format");
			return false;
		}
		#endif

		OpenGL::Format openglFormat;
		if (!OpenGL::TranslateFormat(format, &openglFormat, OpenGL::FormatType_RenderBuffer))
		{
			NazaraError("Failed to translate pixel format \"" + PixelFormat::GetName(format) + "\" into OpenGL format");
			return false;
		}

		GLuint renderBuffer = 0;

		glGenRenderbuffers(1, &renderBuffer);
		if (!renderBuffer)
		{
			NazaraError("Failed to create renderbuffer");
			return false;
		}

		GLint previous;
		glGetIntegerv(GL_RENDERBUFFER_BINDING, &previous);

		glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, openglFormat.internalFormat, width, height);

		if (previous != 0)
			glBindRenderbuffer(GL_RENDERBUFFER, previous);

		m_pixelFormat = format;
		m_height = height;
		m_id = renderBuffer;
		m_width = width;

		return true;
	}

	void RenderBuffer::Destroy()
	{
		if (m_id)
		{
			OnRenderBufferDestroy(this);

			Context::EnsureContext();

			GLuint renderBuffer = m_id;
			glDeleteRenderbuffers(1, &renderBuffer); // Les Renderbuffers sont partagés entre les contextes: Ne posera pas de problème
			m_id = 0;
		}
	}

	unsigned int RenderBuffer::GetHeight() const
	{
		return m_height;
	}

	PixelFormatType RenderBuffer::GetFormat() const
	{
		return m_pixelFormat;
	}

	unsigned int RenderBuffer::GetWidth() const
	{
		return m_width;
	}

	unsigned int RenderBuffer::GetOpenGLID() const
	{
		return m_id;
	}

	bool RenderBuffer::IsValid() const
	{
		return m_id != 0;
	}

	bool RenderBuffer::Initialize()
	{
		if (!RenderBufferLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	void RenderBuffer::Uninitialize()
	{
		RenderBufferLibrary::Uninitialize();
	}

	RenderBufferLibrary::LibraryMap RenderBuffer::s_library;
}
