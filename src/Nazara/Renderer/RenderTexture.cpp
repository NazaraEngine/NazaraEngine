// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <vector>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	struct Attachment
	{
		union
		{
			GLuint buffer;
			NzTexture* texture;
		};

		bool isBuffer;
		bool isUsed = false;
	};

	unsigned int attachmentIndex[nzAttachmentPoint_Max+1] =
	{
		2, // nzAttachmentPoint_Color
		0, // nzAttachmentPoint_Depth
		0, // nzAttachmentPoint_DepthStencil
		1  // nzAttachmentPoint_Stencil
	};

	nzAttachmentPoint formatTypeToAttachment[nzPixelFormatType_Max+1] =
	{
		nzAttachmentPoint_Color,        // nzPixelFormatType_Color
		nzAttachmentPoint_Depth,        // nzPixelFormatType_Depth
		nzAttachmentPoint_DepthStencil, // nzPixelFormatType_DepthStencil
		nzAttachmentPoint_Stencil       // nzPixelFormatType_Stencil
	};

	GLuint lockedPrevious = 0;
	nzUInt8 lockedLevel = 0;
}

struct NzRenderTextureImpl
{
	GLuint fbo;
	std::vector<Attachment> attachements;
	mutable std::vector<GLenum> drawBuffers;
	const NzContext* context;
	bool checked = false;
	bool complete = false;
	mutable bool drawBuffersUpdated = true;
	unsigned int height;
	unsigned int width;
};

NzRenderTexture::~NzRenderTexture()
{
	Destroy();
}

bool NzRenderTexture::AttachBuffer(nzAttachmentPoint attachmentPoint, nzUInt8 index, nzPixelFormat format)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Render texture not created");
		return false;
	}

	if (attachmentPoint != nzAttachmentPoint_Color && index > 0)
	{
		NazaraError("Index must be 0 for non-color attachments");
		return false;
	}

	unsigned int depthStencilIndex = attachmentIndex[nzAttachmentPoint_DepthStencil];
	if (attachmentPoint == nzAttachmentPoint_Stencil && m_impl->attachements.size() > depthStencilIndex &&
		m_impl->attachements[depthStencilIndex].isUsed)
	{
		NazaraError("Stencil target already attached by DepthStencil attachment");
		return false;
	}

	if (formatTypeToAttachment[NzPixelFormat::GetType(format)] != attachmentPoint)
	{
		NazaraError("Pixel format type does not match attachment point type");
		return false;
	}
	#endif

	NzOpenGL::Format openglFormat;
	if (!NzOpenGL::TranslateFormat(format, &openglFormat, NzOpenGL::FormatType_RenderBuffer))
	{
		NazaraError("Failed to translate pixel format into OpenGL format");
		return false;
	}

	if (!Lock())
	{
		NazaraError("Failed to lock render texture");
		return false;
	}

	// Détachement de l'attache précédente (Si il y a)
	Detach(attachmentPoint, index);

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
	glRenderbufferStorage(GL_RENDERBUFFER, openglFormat.internalFormat, m_impl->width, m_impl->height);

	if (previous != 0)
		glBindRenderbuffer(GL_RENDERBUFFER, previous);

	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, NzOpenGL::Attachment[attachmentPoint]+index, GL_RENDERBUFFER, renderBuffer);
	Unlock();

	unsigned int minSize = attachmentIndex[attachmentPoint]+index+1;
	if (m_impl->attachements.size() < minSize)
		m_impl->attachements.resize(minSize);

	Attachment& attachment = m_impl->attachements[minSize-1];
	attachment.isBuffer = true;
	attachment.isUsed = true;
	attachment.buffer = renderBuffer;

	m_impl->checked = false;
	m_impl->drawBuffersUpdated = false;

	return true;
}

bool NzRenderTexture::AttachTexture(nzAttachmentPoint attachmentPoint, nzUInt8 index, NzTexture* texture, unsigned int z)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Render texture not created");
		return false;
	}

	if (attachmentPoint != nzAttachmentPoint_Color && index > 0)
	{
		NazaraError("Index must be 0 for non-color attachments");
		return false;
	}

	if (attachmentPoint == nzAttachmentPoint_Stencil)
	{
		NazaraError("Targeting stencil-only textures is not supported");
		return false;
	}

	unsigned int depthStencilIndex = attachmentIndex[nzAttachmentPoint_DepthStencil];
	if (attachmentPoint == nzAttachmentPoint_Stencil && m_impl->attachements.size() > depthStencilIndex &&
		m_impl->attachements[depthStencilIndex].isUsed)
	{
		NazaraError("Stencil target already attached by DepthStencil attachment");
		return false;
	}

	if (!texture || !texture->IsValid())
	{
		NazaraError("Invalid texture");
		return false;
	}

	if (texture->GetWidth() < m_impl->width || texture->GetHeight() < m_impl->height)
	{
		NazaraError("Texture cannot be smaller than render texture");
		return false;
	}

	unsigned int depth = (texture->GetType() == nzImageType_Cubemap) ? 6 : texture->GetDepth();
	if (z >= depth)
	{
		NazaraError("Z value exceeds depth (" + NzString::Number(z) + " >= (" + NzString::Number(depth) + ')');
		return false;
	}

	if (texture->GetRenderTexture() != nullptr)
	{
		NazaraError("Texture already used by another render texture");
		return false;
	}

	if (formatTypeToAttachment[NzPixelFormat::GetType(texture->GetFormat())] != attachmentPoint)
	{
		NazaraError("Pixel format type does not match attachment point type");
		return false;
	}
	#endif

	if (!Lock())
	{
		NazaraError("Failed to lock render texture");
		return false;
	}

	// Détachement de l'attache précédente (Si il y a)
	Detach(attachmentPoint, index);

	switch (texture->GetType())
	{
		case nzImageType_1D:
			glFramebufferTexture1D(GL_DRAW_FRAMEBUFFER, NzOpenGL::Attachment[attachmentPoint]+index, GL_TEXTURE_1D, texture->GetOpenGLID(), 0);
			break;

		case nzImageType_1D_Array:
		case nzImageType_2D_Array:
			glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, NzOpenGL::Attachment[attachmentPoint]+index, texture->GetOpenGLID(), 0, z);
			break;

		case nzImageType_2D:
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, NzOpenGL::Attachment[attachmentPoint]+index, NzOpenGL::TextureTarget[texture->GetType()], texture->GetOpenGLID(), 0);
			break;

		case nzImageType_3D:
			glFramebufferTexture3D(GL_DRAW_FRAMEBUFFER, NzOpenGL::Attachment[attachmentPoint]+index, GL_TEXTURE_3D, texture->GetOpenGLID(), 0, z);
			break;

		case nzImageType_Cubemap:
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, NzOpenGL::Attachment[attachmentPoint]+index, NzOpenGL::CubemapFace[z], texture->GetOpenGLID(), 0);
			break;
	}

	Unlock();

	unsigned int minSize = attachmentIndex[attachmentPoint]+index+1;
	if (m_impl->attachements.size() < minSize)
		m_impl->attachements.resize(minSize);

	Attachment& attachment = m_impl->attachements[minSize-1];
	attachment.isBuffer = false;
	attachment.isUsed = true;
	attachment.texture = texture;

	texture->AddResourceListener(this);
	texture->SetRenderTexture(this);

	m_impl->checked = false;
	m_impl->drawBuffersUpdated = false;

	return true;
}

bool NzRenderTexture::Create(unsigned int width, unsigned int height, bool lock)
{
	if (!IsSupported())
	{
		NazaraError("Render textures not supported");
		return false;
	}

	Destroy();

	#if NAZARA_RENDERER_SAFE
	if (width == 0)
	{
		NazaraError("Width must be at least 1 (0)");
		return false;
	}

	if (height == 0)
	{
		NazaraError("Height must be at least 1 (0)");
		return false;
	}

	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return false;
	}
	#endif

	NzRenderTextureImpl* impl = new NzRenderTextureImpl;
	impl->width = width;
	impl->height = height;

	impl->context = NzContext::GetCurrent();
	impl->context->AddResourceListener(this);

	impl->fbo = 0;
	glGenFramebuffers(1, &impl->fbo);

	if (!impl->fbo)
	{
		NazaraError("Failed to create framebuffer");
		delete impl;

		return false;
	}

	m_impl = impl;

	if (lock && !Lock())
	{
		NazaraError("Failed to lock render texture");
		delete impl;
		m_impl = nullptr;

		return false;
	}

	NotifyParametersChange();
	NotifySizeChange();

	return true;
}

void NzRenderTexture::Destroy()
{
	if (m_impl)
	{
		bool canFreeFBO = true;
		#if NAZARA_RENDERER_SAFE
		if (NzContext::GetCurrent() != m_impl->context)
		{
			NazaraWarning("RenderTexture should be destroyed by it's creation context, this will cause leaks");
			canFreeFBO = false;
		}
		#endif

		m_impl->context->RemoveResourceListener(this);

		for (const Attachment& attachment : m_impl->attachements)
		{
			if (attachment.isUsed)
			{
				if (attachment.isBuffer)
					glDeleteRenderbuffers(1, &attachment.buffer); // Les Renderbuffers sont partagés entre les contextes: Ne posera pas de problème
				else
				{
					attachment.texture->SetRenderTexture(nullptr);
					attachment.texture->RemoveResourceListener(this);
				}
			}
		}

		if (canFreeFBO)
			glDeleteFramebuffers(1, &m_impl->fbo);

		delete m_impl;
		m_impl = nullptr;
	}
}

void NzRenderTexture::Detach(nzAttachmentPoint attachmentPoint, nzUInt8 index)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Render texture not created");
		return;
	}

	if (attachmentPoint != nzAttachmentPoint_Color && index > 0)
	{
		NazaraError("Index must be 0 for non-color attachments");
		return;
	}
	#endif

	unsigned int attachIndex = attachmentIndex[attachmentPoint]+index;
	if (attachIndex >= m_impl->attachements.size())
		return;

	Attachment& attachement = m_impl->attachements[attachIndex];
	if (!attachement.isUsed)
		return;

	if (!Lock())
	{
		NazaraError("Failed to lock render texture");
		return;
	}

	attachement.isUsed = false;

	if (attachement.isBuffer)
	{
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, NzOpenGL::Attachment[attachmentPoint]+index, GL_RENDERBUFFER, 0);
		glDeleteRenderbuffers(1, &attachement.buffer);
	}
	else
	{
		if (glFramebufferTexture)
			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, NzOpenGL::Attachment[attachmentPoint]+index, 0, 0);
		else
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, NzOpenGL::Attachment[attachmentPoint]+index, 0, 0, 0);

		attachement.texture->RemoveResourceListener(this);
		attachement.texture->SetRenderTexture(nullptr);
	}

	Unlock();

	m_impl->checked = false;
	m_impl->drawBuffersUpdated = false;

}

unsigned int NzRenderTexture::GetHeight() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Render texture not created");
		return 0;
	}
	#endif

	return m_impl->height;
}

NzRenderTargetParameters NzRenderTexture::GetParameters() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Render texture not created");
		return NzRenderTargetParameters();
	}
	#endif

	///TODO
	return NzRenderTargetParameters();
}

unsigned int NzRenderTexture::GetWidth() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Render texture not created");
		return 0;
	}
	#endif

	return m_impl->width;
}

bool NzRenderTexture::IsComplete() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Render texture not created");
		return false;
	}
	#endif

	if (!m_impl->checked)
	{
		if (!Lock())
		{
			NazaraError("Failed to lock render texture");
			return false;
		}

		GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		Unlock();

		m_impl->complete = false;

		switch (status)
		{
			case GL_FRAMEBUFFER_COMPLETE:
				m_impl->complete = true;
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				NazaraError("Incomplete attachment");
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				NazaraInternalError("Incomplete draw buffer");
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				NazaraInternalError("Incomplete read buffer");
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				NazaraError("Incomplete missing attachment");
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				NazaraError("Incomplete multisample");
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				NazaraError("Incomplete layer targets");
				break;

			case GL_FRAMEBUFFER_UNSUPPORTED:
				NazaraError("Render texture has unsupported attachments");
				break;

			default:
				NazaraInternalError("Unknown error");
		}

		m_impl->checked = true;
	}

	return m_impl->complete;
}

bool NzRenderTexture::IsRenderable() const
{
	return IsComplete();
}

bool NzRenderTexture::IsValid() const
{
	return m_impl != nullptr;
}

bool NzRenderTexture::Lock() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Render texture not created");
		return false;
	}

	if (NzContext::GetCurrent() != m_impl->context)
	{
		NazaraError("RenderTexture cannot be used with this context");
		return false;
	}
	#endif

	if (lockedLevel++ == 0)
	{
		GLint previous;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previous);

		lockedPrevious = previous;

		if (lockedPrevious != m_impl->fbo)
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_impl->fbo);
	}

	return true;
}

void NzRenderTexture::Unlock() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Render texture not created");
		return;
	}

	if (NzContext::GetCurrent() != m_impl->context)
	{
		NazaraError("RenderTexture cannot be used with this context");
		return;
	}

	if (lockedLevel == 0)
	{
		NazaraWarning("Unlock called on non-locked texture");
		return;
	}
	#endif

	if (--lockedLevel == 0 && lockedPrevious != m_impl->fbo) // Ici, il est important qu'un FBO soit débindé si l'ancien était 0
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lockedPrevious);
}

bool NzRenderTexture::HasContext() const
{
	return false;
}

bool NzRenderTexture::IsSupported()
{
	return NzRenderer::HasCapability(nzRendererCap_RenderTexture);
}

bool NzRenderTexture::Activate() const
{
	#if NAZARA_RENDERER_SAFE
	if (NzContext::GetCurrent() != m_impl->context)
	{
		NazaraError("RenderTexture cannot be used with this context");
		return false;
	}
	#endif

	if (!m_impl->drawBuffersUpdated)
	{
		m_impl->drawBuffers.clear();
		m_impl->drawBuffers.reserve(m_impl->attachements.size());
		for (unsigned int i = attachmentIndex[nzAttachmentPoint_Color]; i < m_impl->attachements.size(); ++i)
		{
			if (m_impl->attachements[i].isUsed)
				m_impl->drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i - attachmentIndex[nzAttachmentPoint_Color]);
		}

		m_impl->drawBuffersUpdated = true;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_impl->fbo);
	glDrawBuffers(m_impl->drawBuffers.size(), &m_impl->drawBuffers[0]);

	return true;
}

void NzRenderTexture::Desactivate() const
{
	#if NAZARA_RENDERER_SAFE
	if (NzContext::GetCurrent() != m_impl->context)
	{
		NazaraError("RenderTexture cannot be used with this context");
		return;
	}
	#endif

	glFlush();

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

bool NzRenderTexture::OnResourceDestroy(const NzResource* resource, int index)
{
	if (resource == m_impl->context)
	{
		// Notre contexte va être détruit, libérons la RenderTexture pour éviter un leak
		Destroy();

		return false;
	}
	else
	{
		// Sinon, c'est une texture
		resource->RemoveResourceListener(this);

		// La ressource n'est plus, du coup nous mettons à jour
		Attachment& attachement = m_impl->attachements[index];
		attachement.isUsed = false;

		m_impl->checked = false;
		m_impl->drawBuffersUpdated = false;

		return true;
	}
}
