// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Graphics/Components/CameraComponent.hpp>
#include <stdexcept>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	const RenderTarget& CameraComponent::GetRenderTarget()
	{
		if (!m_renderTarget)
			throw std::runtime_error("no rendertarget set");

		return* m_renderTarget;
	}

	ViewerInstance& CameraComponent::GetViewerInstance()
	{
		return m_viewerInstance;
	}

	const ViewerInstance& CameraComponent::GetViewerInstance() const
	{
		return m_viewerInstance;
	}
}
