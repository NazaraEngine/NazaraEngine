// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderBuffer.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <functional>
#include <limits>
#include <memory>
#include <vector>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	namespace
	{
		struct Attachment
		{
			NazaraSlot(RenderBuffer, OnRenderBufferDestroy, renderBufferDestroySlot);
			NazaraSlot(Texture, OnTextureDestroy, textureDestroySlot);

			RenderBufferRef buffer;
			TextureRef texture;

			AttachmentPoint attachmentPoint;
			bool isBuffer;
			bool isUsed = false;
			unsigned int height;
			unsigned int width;
		};

		unsigned int s_attachmentIndex[AttachmentPoint_Max+1] =
		{
			3, // AttachmentPoint_Color
			0, // AttachmentPoint_Depth
			1, // AttachmentPoint_DepthStencil
			2  // AttachmentPoint_Stencil
		};

		AttachmentPoint FormatTypeToAttachment(PixelFormatType format)
		{
			const PixelFormatInfo& info = PixelFormat::GetInfo(format);
			switch (info.content)
			{
				case PixelFormatContent_ColorRGBA:
					return AttachmentPoint_Color;

				case PixelFormatContent_DepthStencil:
					return (!info.greenMask.TestAny()) ? AttachmentPoint_Depth : AttachmentPoint_DepthStencil;

				case PixelFormatContent_Stencil:
					return AttachmentPoint_Stencil;

				case PixelFormatContent_Undefined:
					break;
			}

			NazaraInternalError("Unexpected pixel format content: 0x" + String::Number(info.content, 16));
			return AttachmentPoint_Max;
		}

		GLuint lockedPrevious = 0;
		UInt8 lockedLevel = 0;
	}

	struct RenderTextureImpl
	{
		NazaraSlot(Context, OnContextDestroy, contextDestroySlot);

		GLuint fbo;
		std::vector<Attachment> attachments;
		std::vector<UInt8> colorTargets;
		mutable std::vector<GLenum> drawBuffers;
		const Context* context;
		bool complete = false;
		bool userDefinedTargets = false;
		unsigned int height;
		unsigned int width;
	};

	bool RenderTexture::AttachBuffer(AttachmentPoint attachmentPoint, UInt8 index, RenderBuffer* buffer)
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_impl)
		{
			NazaraError("Render texture not created");
			return false;
		}

		if (attachmentPoint != AttachmentPoint_Color)
		{
			if (index > 0)
			{
				NazaraError("Index must be 0 for non-color attachments");
				return false;
			}
		}
		else
		{
			if (index >= Renderer::GetMaxColorAttachments())
			{
				NazaraError("Color index is over max color attachments (" + String::Number(index) + " >= " + String::Number(Renderer::GetMaxColorAttachments()) + ")");
				return false;
			}
		}

		if (!buffer || !buffer->IsValid())
		{
			NazaraError("Invalid render buffer");
			return false;
		}

		unsigned int depthStencilIndex = s_attachmentIndex[AttachmentPoint_DepthStencil];
		if (m_impl->attachments.size() > depthStencilIndex && m_impl->attachments[depthStencilIndex].isUsed)
		{
			if (attachmentPoint == AttachmentPoint_Depth)
			{
				NazaraError("Depth target already attached by DepthStencil attachment");
				return false;
			}
			else if (attachmentPoint == AttachmentPoint_Stencil)
			{
				NazaraError("Stencil target already attached by DepthStencil attachment");
				return false;
			}
		}

		AttachmentPoint targetAttachmentPoint = FormatTypeToAttachment(buffer->GetFormat());
		if (targetAttachmentPoint != attachmentPoint && targetAttachmentPoint != AttachmentPoint_DepthStencil &&
		    attachmentPoint != AttachmentPoint_Depth && attachmentPoint != AttachmentPoint_Stencil)
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

		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, OpenGL::Attachment[attachmentPoint]+index, GL_RENDERBUFFER, buffer->GetOpenGLID());

		Unlock();

		unsigned int attachIndex = s_attachmentIndex[attachmentPoint] + index;
		if (attachIndex >= m_impl->attachments.size())
			m_impl->attachments.resize(attachIndex+1);

		Attachment& attachment = m_impl->attachments[attachIndex];
		attachment.attachmentPoint = attachmentPoint;
		attachment.buffer = buffer;
		attachment.renderBufferDestroySlot.Connect(buffer->OnRenderBufferDestroy, std::bind(&RenderTexture::OnRenderBufferDestroy, this, std::placeholders::_1, attachIndex));
		attachment.isBuffer = true;
		attachment.isUsed = true;
		attachment.height = buffer->GetHeight();
		attachment.width = buffer->GetWidth();

		InvalidateSize();
		InvalidateTargets();

		return true;
	}

	bool RenderTexture::AttachBuffer(AttachmentPoint attachmentPoint, UInt8 index, PixelFormatType format, unsigned int width, unsigned int height)
	{
		RenderBufferRef renderBuffer = RenderBuffer::New();
		if (!renderBuffer->Create(format, width, height))
		{
			NazaraError("Failed to create RenderBuffer");
			return false;
		}

		if (!AttachBuffer(attachmentPoint, index, renderBuffer))
		{
			NazaraError("Failed to attach buffer");
			return false;
		}

		return true;
	}

	bool RenderTexture::AttachTexture(AttachmentPoint attachmentPoint, UInt8 index, Texture* texture, unsigned int z)
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_impl)
		{
			NazaraError("Render texture not created");
			return false;
		}

		if (attachmentPoint != AttachmentPoint_Color)
		{
			if (index > 0)
			{
				NazaraError("Index must be 0 for non-color attachments");
				return false;
			}
		}
		else
		{
			if (index >= Renderer::GetMaxColorAttachments())
			{
				NazaraError("Color index is over max color attachments (" + String::Number(index) + " >= " + String::Number(Renderer::GetMaxColorAttachments()) + ")");
				return false;
			}
		}

		if (attachmentPoint == AttachmentPoint_Stencil)
		{
			NazaraError("Targeting stencil-only textures is not supported");
			return false;
		}

		unsigned int depthStencilIndex = s_attachmentIndex[AttachmentPoint_DepthStencil];
		if (attachmentPoint == AttachmentPoint_Depth && m_impl->attachments.size() > depthStencilIndex &&
			m_impl->attachments[depthStencilIndex].isUsed)
		{
			NazaraError("Depth target already attached by DepthStencil attachment");
			return false;
		}

		if (!texture || !texture->IsValid())
		{
			NazaraError("Invalid texture");
			return false;
		}

		unsigned int depth = (texture->GetType() == ImageType_Cubemap) ? 6 : texture->GetDepth();
		if (z >= depth)
		{
			NazaraError("Z value exceeds depth (" + String::Number(z) + " >= (" + String::Number(depth) + ')');
			return false;
		}

		AttachmentPoint targetAttachmentPoint = FormatTypeToAttachment(texture->GetFormat());
		if (targetAttachmentPoint != attachmentPoint && targetAttachmentPoint != AttachmentPoint_DepthStencil &&
			attachmentPoint != AttachmentPoint_Depth && attachmentPoint != AttachmentPoint_Stencil)
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
			case ImageType_1D:
				glFramebufferTexture1D(GL_DRAW_FRAMEBUFFER, OpenGL::Attachment[attachmentPoint]+index, GL_TEXTURE_1D, texture->GetOpenGLID(), 0);
				break;

			case ImageType_1D_Array:
			case ImageType_2D_Array:
				glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, OpenGL::Attachment[attachmentPoint]+index, texture->GetOpenGLID(), 0, z);
				break;

			case ImageType_2D:
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, OpenGL::Attachment[attachmentPoint]+index, GL_TEXTURE_2D, texture->GetOpenGLID(), 0);
				break;

			case ImageType_3D:
				glFramebufferTexture3D(GL_DRAW_FRAMEBUFFER, OpenGL::Attachment[attachmentPoint]+index, GL_TEXTURE_3D, texture->GetOpenGLID(), 0, z);
				break;

			case ImageType_Cubemap:
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, OpenGL::Attachment[attachmentPoint]+index, OpenGL::CubemapFace[z], texture->GetOpenGLID(), 0);
				break;
		}

		Unlock();

		unsigned int attachIndex = s_attachmentIndex[attachmentPoint] + index;
		if (attachIndex >= m_impl->attachments.size())
			m_impl->attachments.resize(attachIndex+1);

		Attachment& attachment = m_impl->attachments[attachIndex];
		attachment.attachmentPoint = attachmentPoint;
		attachment.isBuffer = false;
		attachment.isUsed = true;
		attachment.height = texture->GetHeight();
		attachment.texture = texture;
		attachment.textureDestroySlot.Connect(texture->OnTextureDestroy, std::bind(&RenderTexture::OnTextureDestroy, this, std::placeholders::_1, attachIndex));
		attachment.width = texture->GetWidth();

		InvalidateSize();
		InvalidateTargets();

		return true;
	}

	bool RenderTexture::Create(bool lock)
	{
		Destroy();

		#if NAZARA_RENDERER_SAFE
		if (Context::GetCurrent() == nullptr)
		{
			NazaraError("No active context");
			return false;
		}
		#endif

		std::unique_ptr<RenderTextureImpl> impl(new RenderTextureImpl);

		impl->fbo = 0;
		glGenFramebuffers(1, &impl->fbo);

		if (!impl->fbo)
		{
			NazaraError("Failed to create framebuffer");
			return false;
		}

		m_impl = impl.release();
		m_impl->context = Context::GetCurrent();
		m_impl->contextDestroySlot.Connect(m_impl->context->OnContextDestroy, this, &RenderTexture::OnContextDestroy);

		m_checked = false;
		m_drawBuffersUpdated = true;
		m_sizeUpdated = false;
		m_targetsUpdated = true;

		if (lock)
		{
			// En cas d'exception, la ressource sera quand même libérée
			CallOnExit onExit([this] ()
			{
				Destroy();
			});

			if (!Lock())
			{
				NazaraError("Failed to lock render texture");
				return false;
			}

			onExit.Reset();
		}

		OnRenderTargetParametersChange(this);
		OnRenderTargetSizeChange(this);

		return true;
	}

	void RenderTexture::Destroy()
	{
		if (m_impl)
		{
			if (IsActive())
				Renderer::SetTarget(nullptr);

			// Le FBO devant être supprimé dans son contexte d'origine, nous déléguons sa suppression à la classe OpenGL
			// Celle-ci va libérer le FBO dès que possible (la prochaine fois que son contexte d'origine sera actif)
			OpenGL::DeleteFrameBuffer(m_impl->context, m_impl->fbo);

			delete m_impl; // Enlève également une références sur les Texture/RenderBuffer
			m_impl = nullptr;
		}
	}

	void RenderTexture::Detach(AttachmentPoint attachmentPoint, UInt8 index)
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_impl)
		{
			NazaraError("Render texture not created");
			return;
		}

		if (attachmentPoint != AttachmentPoint_Color && index > 0)
		{
			NazaraError("Index must be 0 for non-color attachments");
			return;
		}
		#endif

		unsigned int attachIndex = s_attachmentIndex[attachmentPoint] + index;
		if (attachIndex >= m_impl->attachments.size())
			return;

		Attachment& attachement = m_impl->attachments[attachIndex];
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
			glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, OpenGL::Attachment[attachmentPoint]+index, GL_RENDERBUFFER, 0);

			attachement.buffer = nullptr;
			attachement.renderBufferDestroySlot.Disconnect();
		}
		else
		{
			if (glFramebufferTexture)
				glFramebufferTexture(GL_DRAW_FRAMEBUFFER, OpenGL::Attachment[attachmentPoint]+index, 0, 0);
			else
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, OpenGL::Attachment[attachmentPoint]+index, 0, 0, 0);

			attachement.texture = nullptr;
			attachement.textureDestroySlot.Disconnect();
		}

		InvalidateSize();

		if (attachement.attachmentPoint == AttachmentPoint_Color)
			InvalidateTargets();

		Unlock();

		m_checked = false;
	}

	unsigned int RenderTexture::GetHeight() const
	{
		NazaraAssert(m_impl, "Invalid render texture");

		if (!m_sizeUpdated)
			UpdateSize();

		return m_impl->height;
	}

	RenderTargetParameters RenderTexture::GetParameters() const
	{
		NazaraAssert(m_impl, "Invalid render texture");

		///TODO
		return RenderTargetParameters();
	}

	Vector2ui RenderTexture::GetSize() const
	{
		NazaraAssert(m_impl, "Invalid render texture");

		if (!m_sizeUpdated)
			UpdateSize();

		return Vector2ui(m_impl->width, m_impl->height);
	}

	unsigned int RenderTexture::GetWidth() const
	{
		NazaraAssert(m_impl, "Invalid render texture");

		if (!m_sizeUpdated)
			UpdateSize();

		return m_impl->width;
	}

	bool RenderTexture::IsComplete() const
	{
		NazaraAssert(m_impl, "Invalid render texture");

		if (!m_checked)
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

			m_checked = true;
		}

		return m_impl->complete;
	}

	bool RenderTexture::IsRenderable() const
	{
		return IsComplete() && !m_impl->attachments.empty();
	}

	bool RenderTexture::Lock() const
	{
		NazaraAssert(m_impl, "Invalid render texture");

		#if NAZARA_RENDERER_SAFE
		if (Context::GetCurrent() != m_impl->context)
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

	void RenderTexture::SetColorTargets(const UInt8* targets, unsigned int targetCount) const
	{
		NazaraAssert(m_impl, "Invalid render texture");

		#if NAZARA_RENDERER_SAFE
		for (unsigned int i = 0; i < targetCount; ++i)
		{
			unsigned int index = s_attachmentIndex[AttachmentPoint_Color] + targets[i];
			if (index >= m_impl->attachments.size() || !m_impl->attachments[index].isUsed)
			{
				NazaraError("Target " + String::Number(targets[i]) + " not attached");
				return;
			}
		}
		#endif

		m_impl->colorTargets.resize(targetCount);
		std::memcpy(&m_impl->colorTargets[0], targets, targetCount*sizeof(UInt8));

		m_impl->userDefinedTargets = true;
		InvalidateTargets();
	}

	void RenderTexture::SetColorTargets(const std::initializer_list<UInt8>& targets) const
	{
		NazaraAssert(m_impl, "Invalid render texture");

		#if NAZARA_RENDERER_SAFE
		for (UInt8 target : targets)
		{
			unsigned int index = s_attachmentIndex[AttachmentPoint_Color] + target;
			if (index >= m_impl->attachments.size() || !m_impl->attachments[index].isUsed)
			{
				NazaraError("Target " + String::Number(target) + " not attached");
				return;
			}
		}
		#endif

		m_impl->colorTargets.resize(targets.size());

		UInt8* ptr = &m_impl->colorTargets[0];
		for (UInt8 index : targets)
			*ptr++ = index;

		m_impl->userDefinedTargets = true;
		InvalidateTargets();
	}

	void RenderTexture::Unlock() const
	{
		NazaraAssert(m_impl, "Invalid render texture");

		#if NAZARA_RENDERER_SAFE
		if (Context::GetCurrent() != m_impl->context)
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

	unsigned int RenderTexture::GetOpenGLID() const
	{
		NazaraAssert(m_impl, "Invalid render texture");

		#if NAZARA_RENDERER_SAFE
		if (Context::GetCurrent() != m_impl->context)
		{
			NazaraError("RenderTexture cannot be used with this context");
			return 0;
		}
		#endif

		return m_impl->fbo;
	}

	bool RenderTexture::HasContext() const
	{
		return false;
	}

	void RenderTexture::Blit(RenderTexture* src, Rectui srcRect, RenderTexture* dst, Rectui dstRect, UInt32 buffers, bool bilinearFilter)
	{
		NazaraAssert(src && src->IsValid(), "Invalid source render texture");
		NazaraAssert(dst && dst->IsValid(), "Invalid destination render texture");

		#if NAZARA_RENDERER_SAFE
		if (srcRect.x+srcRect.width > src->GetWidth() || srcRect.y+srcRect.height > src->GetHeight())
		{
			NazaraError("Source rectangle dimensions are out of bounds");
			return;
		}

		if (dstRect.x+dstRect.width > dst->GetWidth() || dstRect.y+dstRect.height > dst->GetHeight())
		{
			NazaraError("Destination rectangle dimensions are out of bounds");
			return;
		}

		if (bilinearFilter && (buffers & RendererBuffer_Depth || buffers & RendererBuffer_Stencil))
		{
			NazaraError("Filter cannot be bilinear when blitting depth/stencil buffers");
			return;
		}
		#endif

		GLbitfield mask = 0;
		if (buffers & RendererBuffer_Color)
			mask |= GL_COLOR_BUFFER_BIT;

		if (buffers & RendererBuffer_Depth)
			mask |= GL_DEPTH_BUFFER_BIT;

		if (buffers & RendererBuffer_Stencil)
			mask |= GL_STENCIL_BUFFER_BIT;

		GLint previousDrawBuffer, previousReadBuffer;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previousDrawBuffer);
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &previousReadBuffer);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->GetOpenGLID());
		glBindFramebuffer(GL_READ_FRAMEBUFFER, src->GetOpenGLID());

		glBlitFramebuffer(srcRect.x, srcRect.y, srcRect.x + srcRect.width, srcRect.y + srcRect.height,
						  dstRect.x, dstRect.y, dstRect.x + dstRect.width, dstRect.y + dstRect.height,
						  mask, (bilinearFilter) ? GL_LINEAR : GL_NEAREST);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, previousDrawBuffer);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, previousReadBuffer);
	}

	bool RenderTexture::Activate() const
	{
		NazaraAssert(m_impl, "Invalid render texture");

		#if NAZARA_RENDERER_SAFE
		if (Context::GetCurrent() != m_impl->context)
		{
			NazaraError("RenderTexture cannot be used with this context");
			return false;
		}
		#endif

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_impl->fbo);

		m_drawBuffersUpdated = false;

		return true;
	}

	void RenderTexture::Desactivate() const
	{
		NazaraAssert(m_impl, "Invalid render texture");

		#if NAZARA_RENDERER_SAFE
		if (Context::GetCurrent() != m_impl->context)
		{
			NazaraError("RenderTexture cannot be used with this context");
			return;
		}
		#endif

		glFlush();

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void RenderTexture::EnsureTargetUpdated() const
	{
		if (!m_drawBuffersUpdated)
			UpdateDrawBuffers();

		for (UInt8 index : m_impl->colorTargets)
		{
			Attachment& attachment = m_impl->attachments[s_attachmentIndex[AttachmentPoint_Color] + index];
			if (!attachment.isBuffer)
				attachment.texture->InvalidateMipmaps();
		}
	}

	void RenderTexture::OnContextDestroy(const Context* context)
	{
		NazaraAssert(m_impl, "Invalid internal state");
		NazaraUnused(context);

		#ifdef NAZARA_DEBUG
		if (m_impl->context != context)
		{
			NazaraInternalError("Not listening to " + String::Pointer(context));
			return;
		}
		#endif

		Destroy();
	}

	void RenderTexture::OnRenderBufferDestroy(const RenderBuffer* renderBuffer, unsigned int attachmentIndex)
	{
		NazaraAssert(m_impl, "Invalid internal state");
		NazaraAssert(attachmentIndex < m_impl->attachments.size(), "Invalid attachment index");
		NazaraAssert(m_impl->attachments[attachmentIndex].isBuffer, "Invalid attachment state");
		NazaraUnused(renderBuffer);

		Attachment& attachment = m_impl->attachments[attachmentIndex];
		attachment.buffer = nullptr;
		attachment.isUsed = false;
		attachment.renderBufferDestroySlot.Disconnect();

		InvalidateTargets();
	}

	void RenderTexture::OnTextureDestroy(const Texture* texture, unsigned int attachmentIndex)
	{
		NazaraAssert(m_impl, "Invalid internal state");
		NazaraAssert(attachmentIndex < m_impl->attachments.size(), "Invalid attachment index");
		NazaraAssert(!m_impl->attachments[attachmentIndex].isBuffer, "Invalid attachment state");
		NazaraUnused(texture);
		NazaraUnused(attachmentIndex);

		InvalidateTargets();
	}

	void RenderTexture::UpdateDrawBuffers() const
	{
		if (!m_targetsUpdated)
			UpdateTargets();

		glDrawBuffers(m_impl->drawBuffers.size(), &m_impl->drawBuffers[0]);

		m_drawBuffersUpdated = true;
	}

	void RenderTexture::UpdateSize() const
	{
		m_impl->width = 0;
		m_impl->height = 0;
		for (Attachment& attachment : m_impl->attachments)
		{
			if (attachment.isUsed)
			{
				m_impl->height = std::max(m_impl->height, attachment.height);
				m_impl->width = std::max(m_impl->width, attachment.width);
			}
		}

		m_sizeUpdated = true;
	}

	void RenderTexture::UpdateTargets() const
	{
		if (!m_impl->userDefinedTargets)
		{
			m_impl->colorTargets.clear();

			unsigned int colorIndex = 0;
			for (unsigned int index = s_attachmentIndex[AttachmentPoint_Color]; index < m_impl->attachments.size(); ++index)
				m_impl->colorTargets.push_back(colorIndex++);
		}

		if (m_impl->colorTargets.empty())
		{
			m_impl->drawBuffers.resize(1);
			m_impl->drawBuffers[0] = GL_NONE;
		}
		else
		{
			m_impl->drawBuffers.resize(m_impl->colorTargets.size());
			GLenum* ptr = &m_impl->drawBuffers[0];
			for (UInt8 index : m_impl->colorTargets)
				*ptr++ = GL_COLOR_ATTACHMENT0 + index;
		}

		m_targetsUpdated = true;
	}
}
