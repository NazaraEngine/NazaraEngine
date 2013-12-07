// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Renderer/Debug.hpp>

NzRenderBuffer::NzRenderBuffer() :
m_id(0)
{
}

NzRenderBuffer::~NzRenderBuffer()
{
	Destroy();
}

bool NzRenderBuffer::Create(nzPixelFormat format, unsigned int width, unsigned int height)
{
	Destroy();

	#if NAZARA_RENDERER_SAFE
	if (width == 0 || height == 0)
	{
		NazaraError("Invalid size");
		return false;
	}

	if (!NzPixelFormat::IsValid(format))
	{
		NazaraError("Invalid pixel format");
		return false;
	}
	#endif

	NzOpenGL::Format openglFormat;
	if (!NzOpenGL::TranslateFormat(format, &openglFormat, NzOpenGL::FormatType_RenderBuffer))
	{
		NazaraError("Failed to translate pixel format \"" + NzPixelFormat::ToString(format) + "\" into OpenGL format");
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

	NotifyCreated();
	return true;
}

void NzRenderBuffer::Destroy()
{
	if (m_id)
	{
		NotifyDestroy();

		NzContext::EnsureContext();

		GLuint renderBuffer = m_id;
		glDeleteRenderbuffers(1, &renderBuffer); // Les Renderbuffers sont partagés entre les contextes: Ne posera pas de problème
		m_id = 0;
	}
}

unsigned int NzRenderBuffer::GetHeight() const
{
	return m_height;
}

nzPixelFormat NzRenderBuffer::GetFormat() const
{
	return m_pixelFormat;
}

unsigned int NzRenderBuffer::GetWidth() const
{
	return m_width;
}

unsigned int NzRenderBuffer::GetOpenGLID() const
{
	return m_id;
}

bool NzRenderBuffer::IsValid() const
{
	return m_id != 0;
}

bool NzRenderBuffer::IsSupported()
{
	return NzOpenGL::IsSupported(nzOpenGLExtension_FrameBufferObject);
}
