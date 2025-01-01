// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ShadowViewer.hpp>

namespace Nz
{
	const Color& ShadowViewer::GetClearColor() const
	{
		throw std::runtime_error("no clear color");
	}

	float ShadowViewer::GetClearDepth() const
	{
		return 1.0f;
	}

	DebugDrawer* ShadowViewer::GetDebugDrawer()
	{
		return nullptr;
	}

	UInt32 ShadowViewer::GetRenderMask() const
	{
		return m_renderMask;
	}

	const RenderTarget& ShadowViewer::GetRenderTarget() const
	{
		throw std::runtime_error("no render target");
	}

	ViewerInstance& ShadowViewer::GetViewerInstance()
	{
		return m_viewerInstance;
	}

	const ViewerInstance& ShadowViewer::GetViewerInstance() const
	{
		return m_viewerInstance;
	}

	const Recti& ShadowViewer::GetViewport() const
	{
		return m_viewport;
	}

	bool ShadowViewer::IsZReversed() const
	{
		return false;
	}
}
