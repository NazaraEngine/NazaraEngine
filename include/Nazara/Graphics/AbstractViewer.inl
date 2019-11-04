// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const UniformBufferRef& AbstractViewer::GetViewerData() const
	{
		if (!m_viewerDataUpdated)
			UpdateViewerData();

		return m_viewerData;
	}

	inline void AbstractViewer::InvalidateViewerData() const
	{
		m_viewerDataUpdated = false;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
