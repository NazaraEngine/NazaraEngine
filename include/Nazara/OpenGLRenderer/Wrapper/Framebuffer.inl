// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Framebuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	inline GLenum Framebuffer::Check() const
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.BindFramebuffer(m_objectId);
		return context.glCheckFramebufferStatus(GL_FRAMEBUFFER);
	}

	inline void Framebuffer::Renderbuffer(GLenum attachment, GLenum renderbuffer)
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.BindFramebuffer(m_objectId);
		context.glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer);
	}

	inline void Framebuffer::Texture2D(GLenum attachment, GLenum textarget, GLuint texture, GLint level)
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.BindFramebuffer(m_objectId);
		context.glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textarget, texture, level);
	}

	inline GLuint Framebuffer::CreateHelper(OpenGLDevice& /*device*/, const Context& context)
	{
		GLuint fbo = 0;
		context.glGenFramebuffers(1U, &fbo);

		return fbo;
	}

	inline void Framebuffer::DestroyHelper(OpenGLDevice& device, const Context& context, GLuint objectId)
	{
		context.glDeleteFramebuffers(1U, &objectId);

		device.NotifyFramebufferDestruction(objectId);
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
