// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp


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
