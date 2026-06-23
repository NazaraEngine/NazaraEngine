// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline LightShadowData::LightShadowData() :
	m_isPerViewer(false)
	{
	}

	inline std::size_t LightShadowData::GetViewCount() const
	{
		return m_shadowAtlasEntryCount;
	}

	inline bool LightShadowData::IsPerViewer() const
	{
		return m_isPerViewer;
	}

	inline void LightShadowData::UpdatePerViewerStatus(bool isPerViewer)
	{
		m_isPerViewer = isPerViewer;
	}

	inline void LightShadowData::UpdateShadowAtlasEntries(std::size_t firstIndex, std::size_t count)
	{
		m_firstShadowAtlasIndex = firstIndex;
		m_shadowAtlasEntryCount = count;
	}
}
