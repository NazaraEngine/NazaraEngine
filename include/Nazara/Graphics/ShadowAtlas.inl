// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline std::optional<Rectf> ShadowAtlas::GetNormalizedRect(std::size_t shadowIndex) const
	{
		std::optional<Rectui32> rect = GetRect(shadowIndex);
		if (!rect)
			return std::nullopt;

		float invAtlasSize = 1.0f / m_atlasSize;

		return Rectf{
			rect->x * invAtlasSize, rect->y * invAtlasSize,
			rect->width * invAtlasSize, rect->height * invAtlasSize
		};
	}

	inline std::optional<Rectui32> ShadowAtlas::GetRect(std::size_t shadowIndex) const
	{
		NazaraAssert(shadowIndex < m_entries.size());
		if (m_entries[shadowIndex].rect.x == InvalidPosition)
			return std::nullopt;

		return m_entries[shadowIndex].rect;
	}

	inline const std::shared_ptr<Texture>& ShadowAtlas::GetTexture() const
	{
		return m_atlasTexture;
	}

	inline bool ShadowAtlas::IsEmpty() const
	{
		return m_entries.empty();
	}

	inline std::size_t ShadowAtlas::Register(UInt32 maxSize, std::size_t count)
	{
		std::size_t entryIndex = m_entries.size();
		for (std::size_t i = 0; i < count; ++i)
		{
			m_entries.push_back({
				.rect = Rectui32(0, 0, maxSize, maxSize)
			});
		}

		return entryIndex;
	}

	inline std::size_t ShadowAtlas::Register(std::initializer_list<UInt32> maxSizes)
	{
		std::size_t entryIndex = m_entries.size();
		for (UInt32 maxSize : maxSizes)
		{
			m_entries.push_back({
				.rect = Rectui32(0, 0, maxSize, maxSize)
			});
		}

		return entryIndex;
	}
}
