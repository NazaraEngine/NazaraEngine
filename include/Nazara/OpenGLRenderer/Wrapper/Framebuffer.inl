// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>

namespace Nz::GL
{
	inline GLenum Framebuffer::Check() const
	{
		assert(m_objectId);

		const Context& context = EnsureContext();
		GLenum target = context.BindFramebuffer(m_objectId);
		return context.glCheckFramebufferStatus(target);
	}

	inline void Framebuffer::DrawBuffers(GLsizei n, const GLenum* bufs)
	{
		assert(m_objectId);

		const Context& context = EnsureContext();
		context.BindFramebuffer(FramebufferTarget::Draw, m_objectId);
		context.glDrawBuffers(n, bufs);
	}

	inline void Framebuffer::Renderbuffer(GLenum attachment, GLenum renderbuffer)
	{
		assert(m_objectId);

		const Context& context = EnsureContext();
		GLenum target = context.BindFramebuffer(m_objectId);
		context.glFramebufferRenderbuffer(target, attachment, GL_RENDERBUFFER, renderbuffer);
	}

	inline void Framebuffer::Texture2D(GLenum attachment, GLenum textarget, GLuint texture, GLint level)
	{
		assert(m_objectId);

		const Context& context = EnsureContext();
		GLenum target = context.BindFramebuffer(m_objectId);
		context.glFramebufferTexture2D(target, attachment, textarget, texture, level);
	}

	inline void Framebuffer::TextureLayer(GLenum attachment, GLuint texture, GLint level, GLint layer)
	{
		assert(m_objectId);

		const Context& context = EnsureContext();
		GLenum target = context.BindFramebuffer(m_objectId);
		context.glFramebufferTextureLayer(target, attachment, texture, level, layer);
	}

	inline GLuint Framebuffer::CreateHelper(const Context& context)
	{
		GLuint fbo = 0;
		context.glGenFramebuffers(1U, &fbo);

		return fbo;
	}

	inline void Framebuffer::DestroyHelper(const Context& context, GLuint objectId)
	{
		context.glDeleteFramebuffers(1U, &objectId);

		context.NotifyFramebufferDestruction(objectId);
	}
}
