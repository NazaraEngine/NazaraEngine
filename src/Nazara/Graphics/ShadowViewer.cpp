// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ShadowViewer.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	const Color& ShadowViewer::GetClearColor() const
	{
		throw std::runtime_error("no clear color");
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
}
