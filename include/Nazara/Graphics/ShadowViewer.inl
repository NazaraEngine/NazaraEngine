// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline void ShadowViewer::UpdateRenderMask(UInt32 renderMask)
	{
		m_renderMask = renderMask;
	}

	inline void ShadowViewer::UpdateViewport(const Recti& viewport)
	{
		m_viewport = viewport;
		m_viewerInstance.UpdateTargetSize({ float(m_viewport.width), float(m_viewport.height) });
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
