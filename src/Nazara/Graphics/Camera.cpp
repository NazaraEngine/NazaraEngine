// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Graphics/Camera.hpp>
#include <stdexcept>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	const RenderTarget& Camera::GetRenderTarget()
	{
		if (!m_renderTarget)
			throw std::runtime_error("no rendertarget set");

		return* m_renderTarget;
	}

	ViewerInstance& Camera::GetViewerInstance()
	{
		return m_viewerInstance;
	}

	const ViewerInstance& Camera::GetViewerInstance() const
	{
		return m_viewerInstance;
	}

	const Recti& Camera::GetViewport() const
	{
		return m_viewport;
	}

	void Camera::UpdateTarget(const RenderTarget* renderTarget)
	{
		m_onRenderTargetRelease.Disconnect();
		m_onRenderTargetSizeChange.Disconnect();

		m_renderTarget = renderTarget;
		if (m_renderTarget)
		{
			m_onRenderTargetRelease.Connect(m_renderTarget->OnRenderTargetRelease, [this](const RenderTarget*)
			{
				UpdateTarget(nullptr);
			});

			m_onRenderTargetSizeChange.Connect(m_renderTarget->OnRenderTargetSizeChange, [this](const RenderTarget*, const Vector2ui& newSize)
			{
				UpdateViewport(newSize);
			});

			UpdateViewport();
		}
	}
}
