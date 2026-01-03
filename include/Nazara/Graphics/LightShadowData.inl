// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline LightShadowData::LightShadowData() :
	m_isPerViewer(false)
	{
	}

	inline void LightShadowData::UpdatePerViewerStatus(bool isPerViewer)
	{
		m_isPerViewer = isPerViewer;
	}

	inline bool LightShadowData::IsPerViewer() const
	{
		return m_isPerViewer;
	}
}
