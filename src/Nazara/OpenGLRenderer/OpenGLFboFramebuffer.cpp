// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/OpenGLFboFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPass.hpp>
#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <stdexcept>

namespace Nz
{
	OpenGLFboFramebuffer::OpenGLFboFramebuffer(OpenGLDevice& device, std::vector<std::shared_ptr<Texture>> attachments) :
	OpenGLFramebuffer(FramebufferType::Texture),
	m_attachments(std::move(attachments)),
	m_device(device)
	{
		m_colorAttachmentCount = 0;
		for (std::size_t i = 0; i < m_attachments.size(); ++i)
		{
			assert(m_attachments[i]);
			Vector2ui textureSize = Vector2ui(m_attachments[i]->GetSize());
			if (i == 0)
				m_size = textureSize;
			else
				m_size.Minimize(textureSize);

			PixelFormat textureFormat = m_attachments[i]->GetFormat();
			if (PixelFormatInfo::GetContent(textureFormat) == PixelFormatContent::ColorRGBA)
				m_colorAttachmentCount++;
		}

		// Create a framebuffer with the current context to ensure its completeness
		const GL::Context* currentContext = GL::Context::GetCurrentContext();
		if (!currentContext)
		{
			currentContext = &m_device.GetReferenceContext();
			GL::Context::SetCurrentContext(currentContext);
		}

		CreateFramebuffer(*currentContext);
	}

	void OpenGLFboFramebuffer::Activate() const
	{
		const GL::Context* currentContext = GL::Context::GetCurrentContext();
		if (!currentContext)
		{
			currentContext = &m_device.GetReferenceContext();
			GL::Context::SetCurrentContext(currentContext);
		}

		GLuint fbo;
		auto it = m_framebuffers.find(currentContext);
		if (it == m_framebuffers.end())
			fbo = CreateFramebuffer(*currentContext).GetObjectId();
		else
			fbo = it->second.framebuffer.GetObjectId();

		currentContext->BindFramebuffer(GL::FramebufferTarget::Draw, fbo);
	}

	std::size_t OpenGLFboFramebuffer::GetColorBufferCount() const
	{
		return m_colorAttachmentCount;
	}

	const Vector2ui& OpenGLFboFramebuffer::GetSize() const
	{
		return m_size;
	}

	void OpenGLFboFramebuffer::UpdateDebugName(std::string_view name)
	{
		m_debugName = name;
		for (auto&& [context, framebufferEntry] : m_framebuffers)
			framebufferEntry.framebuffer.SetDebugName(m_debugName);
	}

	GL::Framebuffer& OpenGLFboFramebuffer::CreateFramebuffer(const GL::Context& context) const
	{
		GL::Framebuffer framebuffer;
		if (!framebuffer.Create(context))
			throw std::runtime_error("failed to create framebuffer object");

		std::size_t colorAttachmentCount = 0;
		bool hasDepth = false;
		bool hasStencil = false;

		for (std::size_t i = 0; i < m_attachments.size(); ++i)
		{
			assert(m_attachments[i]);
			const OpenGLTexture& glTexture = SafeCast<const OpenGLTexture&>(*m_attachments[i]);

			PixelFormat textureFormat = glTexture.GetFormat();

			GLenum attachment;
			switch (PixelFormatInfo::GetContent(textureFormat))
			{
				case PixelFormatContent::ColorRGBA:
					attachment = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + colorAttachmentCount);
					colorAttachmentCount++;
					break;

				case PixelFormatContent::Depth:
					if (hasDepth)
						throw std::runtime_error("a framebuffer can only have one depth attachment");

					attachment = GL_DEPTH_ATTACHMENT;
					hasDepth = true;
					break;

				case PixelFormatContent::DepthStencil:
					if (hasDepth)
						throw std::runtime_error("a framebuffer can only have one depth attachment");

					if (hasStencil)
						throw std::runtime_error("a framebuffer can only have one stencil attachment");

					attachment = GL_DEPTH_STENCIL_ATTACHMENT;
					hasDepth = true;
					hasStencil = true;
					break;

				case PixelFormatContent::Stencil:
					if (hasStencil)
						throw std::runtime_error("a framebuffer can only have one stencil attachment");

					attachment = GL_STENCIL_ATTACHMENT;
					hasStencil = true;
					break;

				case PixelFormatContent::Undefined:
				default:
					throw std::runtime_error("unhandled pixel format " + std::string(PixelFormatInfo::GetName(textureFormat)));
			}

			if (glTexture.RequiresTextureViewEmulation())
			{
				const TextureViewInfo& texViewInfo = glTexture.GetTextureViewInfo();
				if (texViewInfo.viewType != ImageType::E2D)
					throw std::runtime_error("unrestricted texture views can only be used as 2D texture attachment");

				const OpenGLTexture& parentTexture = *glTexture.GetParentTexture();

				switch (parentTexture.GetType())
				{
					case ImageType::Cubemap:
					{
						constexpr std::array<GLenum, 6> faceTargets = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };
						assert(texViewInfo.baseArrayLayer < faceTargets.size());

						GLenum texTarget = faceTargets[texViewInfo.baseArrayLayer];
						framebuffer.Texture2D(attachment, texTarget, parentTexture.GetTexture().GetObjectId(), texViewInfo.baseMipLevel);
						break;
					}

					case ImageType::E1D:
					case ImageType::E2D:
						framebuffer.Texture2D(attachment, GL_TEXTURE_2D, parentTexture.GetTexture().GetObjectId(), texViewInfo.baseMipLevel);
						break;

					case ImageType::E1D_Array:
					case ImageType::E2D_Array:
					case ImageType::E3D:
						framebuffer.TextureLayer(attachment, parentTexture.GetTexture().GetObjectId(), texViewInfo.baseMipLevel, texViewInfo.baseArrayLayer);
						break;
				}
			}
			else
				framebuffer.Texture2D(attachment, ToOpenGL(OpenGLTexture::ToTextureTarget(glTexture.GetType())), glTexture.GetTexture().GetObjectId());
		}

		GLenum status = framebuffer.Check();
		if (status != GL_FRAMEBUFFER_COMPLETE)
			throw std::runtime_error("invalid framebuffer: 0x" + NumberToString(status, 16));

		if (!m_debugName.empty())
			framebuffer.SetDebugName(m_debugName);

		assert(m_colorAttachmentCount == colorAttachmentCount);

		if (m_colorAttachmentCount > 0)
		{
			StackArray<GLenum> fboDrawBuffers = NazaraStackArrayNoInit(GLenum, m_colorAttachmentCount);
			for (std::size_t i = 0; i < m_colorAttachmentCount; ++i)
				fboDrawBuffers[i] = GLenum(GL_COLOR_ATTACHMENT0 + i);

			framebuffer.DrawBuffers(SafeCaster(m_colorAttachmentCount), fboDrawBuffers.data());
		}
		else
		{
			GLenum buffer = GL_NONE;
			framebuffer.DrawBuffers(1, &buffer);
		}

		auto& framebufferEntry = m_framebuffers[&context];
		framebufferEntry.framebuffer = std::move(framebuffer);
		framebufferEntry.onContextDestruction.Connect(context.OnContextDestruction, [this](GL::Context* context)
		{
			m_framebuffers.erase(context);
		});

		return framebufferEntry.framebuffer;
	}
}
