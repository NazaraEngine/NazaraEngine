// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline TextureAsset::~TextureAsset()
	{
		Destroy();
	}

	inline PixelFormat TextureAsset::GetFormat() const
	{
		return m_textureInfo.pixelFormat;
	}

	inline UInt8 TextureAsset::GetLevelCount() const
	{
		return m_textureInfo.levelCount;
	}

	inline Vector3ui TextureAsset::GetSize(UInt8 level) const
	{
		return Vector3ui(ImageUtils::GetLevelSize(m_textureInfo.width, level), ImageUtils::GetLevelSize(m_textureInfo.height, level), ImageUtils::GetLevelSize(m_textureInfo.depth, level));
	}

	inline const TextureInfo& TextureAsset::GetTextureInfo() const
	{
		return m_textureInfo;
	}

	inline ImageType TextureAsset::GetType() const
	{
		return m_textureInfo.type;
	}

	inline auto TextureAsset::GetEntry(RenderDevice& device) const -> TextureEntry*
	{
		// As we will have one device, and probably no more than two, linear search is more than enough
		for (TextureEntry& entry : m_entries)
		{
			if (entry.device == &device)
				return &entry;
		}

		return nullptr;
	}
}
