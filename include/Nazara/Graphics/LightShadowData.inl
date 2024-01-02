// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

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

#include <Nazara/Graphics/DebugOff.hpp>
