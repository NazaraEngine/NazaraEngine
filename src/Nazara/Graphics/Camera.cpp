// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Camera.hpp>
#include <stdexcept>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	const Color& Camera::GetClearColor() const
	{
		return m_clearColor;
	}

	UInt32 Camera::GetRenderMask() const
	{
		return m_renderMask;
	}

	const RenderTarget& Camera::GetRenderTarget() const
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
