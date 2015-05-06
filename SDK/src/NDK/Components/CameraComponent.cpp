// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Algorithm.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	void CameraComponent::SetLayer(unsigned int layer)
	{
		m_layer = layer;

		m_entity->Invalidate(); // Invalidate the entity to make it passes through RenderSystem validation
	}

	void CameraComponent::OnAttached()
	{
		InvalidateViewMatrix();
	}

	void CameraComponent::OnComponentAttached(BaseComponent& component)
	{
		if (IsComponent<NodeComponent>(component))
		{
			NodeComponent& nodeComponent = static_cast<NodeComponent&>(component);
			nodeComponent.AddListener(this);

			InvalidateViewMatrix();
		}
	}

	void CameraComponent::OnComponentDetached(BaseComponent& component)
	{
		if (IsComponent<NodeComponent>(component))
		{
			NodeComponent& nodeComponent = static_cast<NodeComponent&>(component);
			nodeComponent.RemoveListener(this);

			InvalidateViewMatrix();
		}
	}

	void CameraComponent::OnDetached()
	{
		InvalidateViewMatrix();
	}

	bool CameraComponent::OnNodeInvalidated(const NzNode* node, void* userdata)
	{
		NazaraUnused(node);
		NazaraUnused(userdata);

		// Our view matrix depends on NodeComponent position/rotation
		InvalidateViewMatrix();
		return true;
	}

	void CameraComponent::OnRenderTargetReleased(const NzRenderTarget* renderTarget, void* userdata)
	{
		NazaraUnused(userdata);

		if (renderTarget == m_target)
			m_target = nullptr;
		else
			NazaraInternalError("Not listening to " + NzString::Pointer(renderTarget));
	}

	bool CameraComponent::OnRenderTargetSizeChange(const NzRenderTarget* renderTarget, void* userdata)
	{
		NazaraUnused(userdata);

		if (renderTarget == m_target)
		{
			InvalidateViewport();

			return true;
		}
		else
		{
			NazaraInternalError("Not listening to " + NzString::Pointer(renderTarget));
			return false;
		}
	}

	void CameraComponent::UpdateFrustum() const
	{
		EnsureProjectionMatrixUpdate();
		EnsureViewMatrixUpdate();

		// Extract the frustum from the view and projection matrices
		m_frustum.Extract(m_viewMatrix, m_projectionMatrix);
		m_frustumUpdated = true;
	}

	void CameraComponent::UpdateProjectionMatrix() const
	{
		EnsureViewportUpdate(); // Can affect aspect ratio

		m_projectionMatrix.MakePerspective(m_fov, m_aspectRatio, m_zNear, m_zFar);
		m_projectionMatrixUpdated = true;
	}

	void CameraComponent::UpdateViewMatrix() const
	{
		NazaraAssert(m_entity && m_entity->HasComponent<NodeComponent>(), "CameraComponent requires NodeComponent");

		NodeComponent& nodeComponent = m_entity->GetComponent<NodeComponent>();

		// Build the view matrix using the NodeComponent position/rotation
		m_viewMatrix.MakeViewMatrix(nodeComponent.GetPosition(nzCoordSys_Global), nodeComponent.GetRotation(nzCoordSys_Global));
		m_viewMatrixUpdated = true;
	}

	void CameraComponent::UpdateViewport() const
	{
		NazaraAssert(m_target, "CameraComponent has no target");

		unsigned int targetWidth = m_target->GetWidth();
		unsigned int targetHeight = std::max(m_target->GetHeight(), 1U); // Let's make sure we won't divide by zero

		// Our target region is expressed as % of the viewport dimensions, let's compute it in pixels
		NzRectf fViewport(m_targetRegion);
		fViewport.x *= targetWidth;
		fViewport.y *= targetHeight;
		fViewport.width *= targetWidth;
		fViewport.height *= targetHeight;

		// Compute the new aspect ratio, if it's different we need to invalidate the projection matrix
		float aspectRatio = fViewport.width/fViewport.height;
		if (!NzNumberEquals(m_aspectRatio, aspectRatio, 0.001f))
		{
			m_aspectRatio = aspectRatio;

			InvalidateProjectionMatrix();
		}

		// Convert it back to int
		m_viewport.Set(fViewport);
		m_viewportUpdated = true;
	}

	ComponentIndex CameraComponent::componentIndex;
}
