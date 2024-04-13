// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline TextureAsset::~TextureAsset()
	{
		Destroy();
	}

	inline auto TextureAsset::GetEntry(RenderDevice& device) -> TextureEntry*
	{
		// As we will have one device, and probably no more than two, linear search is more than enough
		for (TextureEntry& entry : m_entries)
		{
			if (entry.device == &device)
				return &entry;
		}

		return nullptr;
	}

	inline auto TextureAsset::GetEntry(RenderDevice& device) const -> const TextureEntry*
	{
		// As we will have one device, and probably no more than two, linear search is more than enough
		for (const TextureEntry& entry : m_entries)
		{
			if (entry.device == &device)
				return &entry;
		}

		return nullptr;
	}

}
