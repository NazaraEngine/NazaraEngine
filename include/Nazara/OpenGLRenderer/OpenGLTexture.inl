// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <stdexcept>

namespace Nz
{
	inline void OpenGLTexture::GenerateMipmaps(UInt8 baseLevel, UInt8 levelCount)
	{
		NazaraAssertMsg(baseLevel + levelCount <= m_textureInfo.levelCount, "out of bounds");

		GL::Texture* targetTexture;
		if (RequiresTextureViewEmulation())
		{
			baseLevel += m_viewInfo->baseMipLevel;
			levelCount += m_viewInfo->baseMipLevel;
			targetTexture = &m_parentTexture->m_texture;
		}
		else
			targetTexture = &m_texture;

		if (baseLevel != 0)
			targetTexture->SetParameteri(GL_TEXTURE_BASE_LEVEL, baseLevel);

		if (levelCount != m_textureInfo.levelCount)
			targetTexture->SetParameteri(GL_TEXTURE_MAX_LEVEL, levelCount - 1);

		targetTexture->GenerateMipmap();

		// Reset level config
		if (baseLevel != 0)
			targetTexture->SetParameteri(GL_TEXTURE_BASE_LEVEL, 0);

		if (levelCount != m_textureInfo.levelCount)
			targetTexture->SetParameteri(GL_TEXTURE_MAX_LEVEL, m_textureInfo.levelCount - 1);
	}

	inline RenderDevice* OpenGLTexture::GetDevice()
	{
		return m_texture.GetDevice();
	}

	inline const RenderDevice* OpenGLTexture::GetDevice() const
	{
		return m_texture.GetDevice();
	}

	inline PixelFormat OpenGLTexture::GetFormat() const
	{
		return m_textureInfo.pixelFormat;
	}

	inline UInt8 OpenGLTexture::GetLevelCount() const
	{
		return m_textureInfo.levelCount;
	}

	inline OpenGLTexture* OpenGLTexture::GetParentTexture() const
	{
		return m_parentTexture.get();
	}

	inline Vector3ui OpenGLTexture::GetSize(UInt8 level) const
	{
		return Vector3ui(ImageUtils::GetLevelSize(m_textureInfo.width, level), ImageUtils::GetLevelSize(m_textureInfo.height, level), ImageUtils::GetLevelSize(m_textureInfo.depth, level));
	}

	inline const GL::Texture& OpenGLTexture::GetTexture() const
	{
		return m_texture;
	}

	inline const TextureInfo& OpenGLTexture::GetTextureInfo() const
	{
		return m_textureInfo;
	}

	inline const TextureViewInfo& OpenGLTexture::GetTextureViewInfo() const
	{
		assert(m_viewInfo);
		return *m_viewInfo;
	}

	inline ImageType OpenGLTexture::GetType() const
	{
		return m_textureInfo.type;
	}

	inline bool OpenGLTexture::RequiresTextureViewEmulation() const
	{
		return m_viewInfo.has_value() && !m_texture.IsValid();
	}

	inline GL::TextureTarget OpenGLTexture::ToTextureTarget(ImageType imageType)
	{
		switch (imageType)
		{
			case ImageType::E2D:       return GL::TextureTarget::Target2D;
			case ImageType::E2D_Array: return GL::TextureTarget::Target2D_Array;
			case ImageType::E3D:       return GL::TextureTarget::Target3D;
			case ImageType::Cubemap:   return GL::TextureTarget::Cubemap;

			// OpenGL ES doesn't support 1D textures, use 2D textures with a height of 1 instead
			case ImageType::E1D:       return GL::TextureTarget::Target2D;
			case ImageType::E1D_Array: return GL::TextureTarget::Target2D_Array;
		}

		throw std::runtime_error("unsupported texture type");
	}
}
