// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Graphics/Components/CameraComponent.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline CameraComponent::CameraComponent(const RenderTarget* renderTarget, ProjectionType projectionType) :
	m_projectionType(projectionType),
	m_renderTarget(renderTarget)
	{
		UpdateProjectionMatrix();
	}

	inline void CameraComponent::UpdateTarget(const RenderTarget* renderTarget)
	{
		m_renderTarget = renderTarget;
	}

	inline void CameraComponent::UpdateProjectionType(ProjectionType projectionType)
	{
		m_projectionType = projectionType;
		UpdateProjectionMatrix();
	}

	inline void CameraComponent::UpdateProjectionMatrix()
	{
		//FIXME
		switch (m_projectionType)
		{
			case ProjectionType::Orthographic:
				m_viewerInstance.UpdateProjectionMatrix(Nz::Matrix4f::Ortho(0.f, 1920.f, 0.f, 1080.f));
				break;

			case ProjectionType::Perspective:
				m_viewerInstance.UpdateProjectionMatrix(Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), 16.f / 9.f, 1.f, 1000.f));
				break;
		}
	}
}
