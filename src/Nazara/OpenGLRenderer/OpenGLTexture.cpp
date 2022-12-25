// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLTexture::OpenGLTexture(OpenGLDevice& device, const TextureInfo& textureInfo) :
	m_textureInfo(textureInfo)
	{
		if (!m_texture.Create(device))
			throw std::runtime_error("failed to create texture object");

		auto format = DescribeTextureFormat(textureInfo.pixelFormat);
		if (!format)
			throw std::runtime_error("unsupported texture format");

		const GL::Context& context = m_texture.EnsureDeviceContext();
		context.ClearErrorStack();

		switch (textureInfo.type)
		{
			case ImageType::E2D:
				m_texture.TexStorage2D(GL::TextureTarget::Target2D, textureInfo.levelCount, format->internalFormat, textureInfo.width, textureInfo.height);
				break;

			case ImageType::E2D_Array:
				m_texture.TexStorage3D(GL::TextureTarget::Target2D_Array, textureInfo.levelCount, format->internalFormat, textureInfo.width, textureInfo.height, textureInfo.layerCount);
				break;

			case ImageType::E3D:
				m_texture.TexStorage3D(GL::TextureTarget::Target3D, textureInfo.levelCount, format->internalFormat, textureInfo.width, textureInfo.height, textureInfo.depth);
				break;

			case ImageType::Cubemap:
				m_texture.TexStorage2D(GL::TextureTarget::Cubemap, textureInfo.levelCount, format->internalFormat, textureInfo.width, textureInfo.height);
				break;

			// OpenGL ES doesn't support 1D textures, use 2D textures with a height of 1 instead
			case ImageType::E1D:
				m_texture.TexStorage2D(GL::TextureTarget::Target2D, textureInfo.levelCount, format->internalFormat, textureInfo.width, 1);
				break;

			case ImageType::E1D_Array:
				m_texture.TexStorage2D(GL::TextureTarget::Target2D, textureInfo.levelCount, format->internalFormat, textureInfo.width, textureInfo.layerCount);
				break;
		}

		if (!context.DidLastCallSucceed())
			throw std::runtime_error("failed to create texture");

		m_texture.SetParameteri(GL_TEXTURE_MAX_LEVEL, m_textureInfo.levelCount);
		m_texture.SetParameteri(GL_TEXTURE_SWIZZLE_R, format->swizzleR);
		m_texture.SetParameteri(GL_TEXTURE_SWIZZLE_G, format->swizzleG);
		m_texture.SetParameteri(GL_TEXTURE_SWIZZLE_B, format->swizzleB);
		m_texture.SetParameteri(GL_TEXTURE_SWIZZLE_A, format->swizzleA);
	}

	OpenGLTexture::OpenGLTexture(std::shared_ptr<OpenGLTexture> parentTexture, const TextureViewInfo& viewInfo) :
	m_parentTexture(std::move(parentTexture))
	{
		const GL::Context& context = m_parentTexture->m_texture.EnsureDeviceContext();

		NazaraAssert(viewInfo.layerCount <= m_parentTexture->m_textureInfo.layerCount - viewInfo.baseArrayLayer, "layer count exceeds number of layers");
		NazaraAssert(viewInfo.levelCount <= m_parentTexture->m_textureInfo.levelCount - viewInfo.baseMipLevel, "level count exceeds number of levels");

		m_textureInfo = ApplyView(m_parentTexture->m_textureInfo, viewInfo);
		m_viewInfo = viewInfo;

		// Try to use texture views if supported (core in GL 4.3 or extension)
		if (context.IsExtensionSupported(GL::Extension::TextureView))
		{
			if (m_texture.Create(*m_parentTexture->m_texture.GetDevice()))
			{
				auto format = DescribeTextureFormat(viewInfo.reinterpretFormat);

				context.ClearErrorStack();

				m_texture.TextureView(ToTextureTarget(viewInfo.viewType), m_parentTexture->m_texture.GetObjectId(), format->internalFormat, viewInfo.baseMipLevel, viewInfo.levelCount, viewInfo.baseArrayLayer, viewInfo.layerCount);

				if (!context.DidLastCallSucceed())
					m_texture.Destroy();
			}
		}

		// If texture views are not supported, they will be emulated when using them as attachments
	}

	bool OpenGLTexture::Copy(const Texture& source, const Boxui& srcBox, const Vector3ui& dstPos)
	{
		const OpenGLTexture& glTexture = static_cast<const OpenGLTexture&>(source);

		const GL::Context& context = m_texture.EnsureDeviceContext();
		return context.CopyTexture(glTexture, *this, srcBox, dstPos);
	}

	std::shared_ptr<Texture> OpenGLTexture::CreateView(const TextureViewInfo& viewInfo)
	{
		if (m_parentTexture)
		{
			assert(m_viewInfo);
			NazaraAssert(viewInfo.layerCount <= m_viewInfo->layerCount - viewInfo.baseArrayLayer, "layer count exceeds number of layers");
			NazaraAssert(viewInfo.levelCount <= m_viewInfo->levelCount - viewInfo.baseMipLevel, "level count exceeds number of levels");

			TextureViewInfo ajustedView = viewInfo;
			ajustedView.baseArrayLayer += m_viewInfo->baseArrayLayer;
			ajustedView.baseMipLevel += m_viewInfo->baseMipLevel;

			return m_parentTexture->CreateView(ajustedView);
		}

		return std::make_shared<OpenGLTexture>(std::static_pointer_cast<OpenGLTexture>(shared_from_this()), viewInfo);
	}

	bool OpenGLTexture::Update(const void* ptr, const Boxui& box, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
	{
		auto format = DescribeTextureFormat(m_textureInfo.pixelFormat);
		assert(format);

		const GL::Context& context = m_texture.EnsureDeviceContext();

		UInt8 bpp = PixelFormatInfo::GetBytesPerPixel(m_textureInfo.pixelFormat);
		if (bpp % 8 == 0)
			context.glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
		else if (bpp % 4 == 0)
			context.glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		else if (bpp % 2 == 0)
			context.glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		else
			context.glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		context.glPixelStorei(GL_UNPACK_ROW_LENGTH,   srcWidth);
		context.glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, srcHeight);

		switch (m_textureInfo.type)
		{
			case ImageType::E1D:
				break;

			case ImageType::E1D_Array:
				break;

			case ImageType::E2D:
				m_texture.TexSubImage2D(GL::TextureTarget::Target2D, level, box.x, box.y, box.width, box.height, format->format, format->type, ptr);
				break;

			case ImageType::E2D_Array:
				break;

			case ImageType::E3D:
				break;

			case ImageType::Cubemap:
			{
				std::size_t faceSize = PixelFormatInfo::ComputeSize(m_textureInfo.pixelFormat, m_textureInfo.width, m_textureInfo.height, 1);
				const UInt8* facePtr = static_cast<const UInt8*>(ptr);

				for (GL::TextureTarget face : { GL::TextureTarget::CubemapPositiveX, GL::TextureTarget::CubemapNegativeX, GL::TextureTarget::CubemapPositiveY, GL::TextureTarget::CubemapNegativeY, GL::TextureTarget::CubemapPositiveZ, GL::TextureTarget::CubemapNegativeZ })
				{
					m_texture.TexSubImage2D(face, level, box.x, box.y, box.width, box.height, format->format, format->type, facePtr);
					facePtr += faceSize;
				}
				break;
			}

			default:
				break;
		}

		return true;
	}

	void OpenGLTexture::UpdateDebugName(std::string_view name)
	{
		m_texture.SetDebugName(name);
	}
}
