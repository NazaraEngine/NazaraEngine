// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Algorithm.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
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

		// La matrice de vue dépend directement des données du node, invalidons-là
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

		m_frustum.Extract(m_viewMatrix, m_projectionMatrix);
		m_frustumUpdated = true;
	}

	void CameraComponent::UpdateProjectionMatrix() const
	{
		EnsureViewportUpdate(); // Peut affecter l'aspect ratio

		m_projectionMatrix.MakePerspective(m_fov, m_aspectRatio, m_zNear, m_zFar);
		m_projectionMatrixUpdated = true;
	}

	void CameraComponent::UpdateViewMatrix() const
	{
		NazaraAssert(m_entity && m_entity->HasComponent<NodeComponent>(), "CameraComponent requires NodeComponent");

		NodeComponent& nodeComponent = m_entity->GetComponent<NodeComponent>();

		m_viewMatrix.MakeViewMatrix(nodeComponent.GetPosition(nzCoordSys_Global), nodeComponent.GetRotation(nzCoordSys_Global));
		m_viewMatrixUpdated = true;
	}

	void CameraComponent::UpdateViewport() const
	{
		NazaraAssert(m_target, "CameraComponent has no target");

		unsigned int targetWidth = m_target->GetWidth();
		unsigned int targetHeight = std::max(m_target->GetHeight(), 1U); // Pour éviter une division par zéro

		// La zone visée est définie en "pourcentage" (0...1), on calcule les valeurs en pixel
		NzRectf fViewport(m_targetRegion);
		fViewport.x *= targetWidth;
		fViewport.y *= targetHeight;
		fViewport.width *= targetWidth;
		fViewport.height *= targetHeight;

		// On calcule le rapport largeur/hauteur, et s'il est différent on invalide
		float aspectRatio = fViewport.width/fViewport.height;
		if (!NzNumberEquals(m_aspectRatio, aspectRatio, 0.001f))
		{
			m_aspectRatio = aspectRatio;

			InvalidateProjectionMatrix();
		}

		m_viewport.Set(fViewport);
		m_viewportUpdated = true;
	}

	ComponentIndex CameraComponent::componentIndex;
}
