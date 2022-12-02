// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLFboFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPass.hpp>
#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <Nazara/Utils/StackArray.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLFboFramebuffer::OpenGLFboFramebuffer(OpenGLDevice& device, const std::vector<std::shared_ptr<Texture>>& attachments) :
	OpenGLFramebuffer(FramebufferType::Texture)
	{
		if (!m_framebuffer.Create(device.GetReferenceContext()))
			throw std::runtime_error("failed to create framebuffer object");

		std::size_t colorAttachmentCount = 0;
		bool hasDepth = false;
		bool hasStencil = false;

		m_attachmentSizes.resize(attachments.size());
		for (std::size_t i = 0; i < attachments.size(); ++i)
		{
			assert(attachments[i]);
			const OpenGLTexture& glTexture = static_cast<const OpenGLTexture&>(*attachments[i]);

			Vector2ui textureSize = Vector2ui(glTexture.GetSize());
			m_attachmentSizes[i] = textureSize;

			if (i == 0)
				m_size = textureSize;
			else
				m_size.Minimize(textureSize);

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
					throw std::runtime_error("unhandled pixel format " + PixelFormatInfo::GetName(textureFormat));
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
						m_framebuffer.Texture2D(GL_COLOR_ATTACHMENT0, texTarget, parentTexture.GetTexture().GetObjectId(), texViewInfo.baseMipLevel);
						break;
					}

					case ImageType::E1D:
					case ImageType::E2D:
						m_framebuffer.Texture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, parentTexture.GetTexture().GetObjectId(), texViewInfo.baseMipLevel);
						break;

					case ImageType::E1D_Array:
					case ImageType::E2D_Array:
					case ImageType::E3D:
						m_framebuffer.TextureLayer(GL_COLOR_ATTACHMENT0, parentTexture.GetTexture().GetObjectId(), texViewInfo.baseArrayLayer, texViewInfo.baseMipLevel);
						break;
				}
			}
			else
				m_framebuffer.Texture2D(attachment, ToOpenGL(OpenGLTexture::ToTextureTarget(glTexture.GetType())), glTexture.GetTexture().GetObjectId());
		}

		GLenum status = m_framebuffer.Check();
		if (status != GL_FRAMEBUFFER_COMPLETE)
			throw std::runtime_error("invalid framebuffer: 0x" + NumberToString(status, 16));

		m_colorAttachmentCount = colorAttachmentCount;

		if (m_colorAttachmentCount > 0)
		{
			StackArray<GLenum> fboDrawBuffers = NazaraStackArrayNoInit(GLenum, m_colorAttachmentCount);
			for (std::size_t i = 0; i < m_colorAttachmentCount; ++i)
				fboDrawBuffers[i] = GLenum(GL_COLOR_ATTACHMENT0 + i);

			m_framebuffer.DrawBuffers(SafeCast<GLsizei>(m_colorAttachmentCount), fboDrawBuffers.data());
		}
		else
		{
			GLenum buffer = GL_NONE;
			m_framebuffer.DrawBuffers(1, &buffer);
		}
	}

	void OpenGLFboFramebuffer::Activate() const
	{
		const GL::Context& context = m_framebuffer.EnsureContext();

		context.BindFramebuffer(GL::FramebufferTarget::Draw, m_framebuffer.GetObjectId());
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
		m_framebuffer.SetDebugName(name);
	}
}
