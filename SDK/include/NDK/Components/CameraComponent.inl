// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/CameraComponent.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Algorithm.hpp>

namespace Ndk
{
	inline CameraComponent::CameraComponent() :
	m_projectionType(Nz::ProjectionType_Perspective),
	m_targetRegion(0.f, 0.f, 1.f, 1.f),
	m_target(nullptr),
	m_size(0.f),
	m_frustumUpdated(false),
	m_projectionMatrixUpdated(false),
	m_viewMatrixUpdated(false),
	m_viewportUpdated(false),
	m_aspectRatio(0.f),
	m_fov(70.f),
	m_zFar(100.f),
	m_zNear(1.f),
	m_layer(0)
	{
	}

	inline CameraComponent::CameraComponent(const CameraComponent& camera) :
	Component(camera),
	AbstractViewer(camera),
	m_projectionType(camera.m_projectionType),
	m_targetRegion(camera.m_targetRegion),
	m_target(nullptr),
	m_size(camera.m_size),
	m_frustumUpdated(false),
	m_projectionMatrixUpdated(false),
	m_viewMatrixUpdated(false),
	m_viewportUpdated(false),
	m_aspectRatio(camera.m_aspectRatio),
	m_fov(camera.m_fov),
	m_zFar(camera.m_zFar),
	m_zNear(camera.m_zNear),
	m_layer(camera.m_layer)
	{
		SetTarget(camera.m_target);
	}

	inline void CameraComponent::EnsureFrustumUpdate() const
	{
		if (!m_frustumUpdated)
			UpdateFrustum();
	}

	inline void CameraComponent::EnsureProjectionMatrixUpdate() const
	{
		if (!m_projectionMatrixUpdated)
			UpdateProjectionMatrix();
	}

	inline void CameraComponent::EnsureViewMatrixUpdate() const
	{
		if (!m_viewMatrixUpdated)
			UpdateViewMatrix();
	}

	inline void CameraComponent::EnsureViewportUpdate() const
	{
		if (!m_viewportUpdated)
			UpdateViewport();
	}

	inline float CameraComponent::GetAspectRatio() const
	{
		EnsureViewportUpdate();

		return m_aspectRatio;
	}

	inline float CameraComponent::GetFOV() const
	{
		return m_fov;
	}

	inline const Nz::Frustumf& CameraComponent::GetFrustum() const
	{
		EnsureFrustumUpdate();

		return m_frustum;
	}

	inline unsigned int CameraComponent::GetLayer() const
	{
		return m_layer;
	}

	inline const Nz::Matrix4f& CameraComponent::GetProjectionMatrix() const
	{
		EnsureProjectionMatrixUpdate();

		return m_projectionMatrix;
	}

	inline Nz::ProjectionType CameraComponent::GetProjectionType() const
	{
		return m_projectionType;
	}

	inline const Nz::Vector2f & CameraComponent::GetSize() const
	{
		return m_size;
	}

	inline const Nz::RenderTarget* CameraComponent::GetTarget() const
	{
		return m_target;
	}

	inline const Nz::Rectf& CameraComponent::GetTargetRegion() const
	{
		return m_targetRegion;
	}

	inline const Nz::Matrix4f& CameraComponent::GetViewMatrix() const
	{
		EnsureViewMatrixUpdate();

		return m_viewMatrix;
	}

	inline const Nz::Recti& CameraComponent::GetViewport() const
	{
		EnsureViewportUpdate();

		return m_viewport;
	}

	inline float CameraComponent::GetZFar() const
	{
		return m_zFar;
	}

	inline float CameraComponent::GetZNear() const
	{
		return m_zNear;
	}

	inline void CameraComponent::SetFOV(float fov)
	{
		NazaraAssert(!Nz::NumberEquals(fov, 0.f), "FOV must be different from zero");
		m_fov = fov;

		InvalidateProjectionMatrix();
	}

	inline void CameraComponent::SetProjectionType(Nz::ProjectionType projectionType)
	{
		m_projectionType = projectionType;

		InvalidateProjectionMatrix();
	}

	inline void CameraComponent::SetSize(const Nz::Vector2f& size)
	{
		m_size = size;

		InvalidateProjectionMatrix();
	}

	inline void CameraComponent::SetSize(float width, float height)
	{
		SetSize({width, height});
	}

	inline void CameraComponent::SetTarget(const Nz::RenderTarget* renderTarget)
	{
		m_target = renderTarget;
		if (m_target)
		{
			m_targetResizeSlot.Connect(m_target->OnRenderTargetSizeChange, this, &CameraComponent::OnRenderTargetSizeChange);
			m_targetReleaseSlot.Connect(m_target->OnRenderTargetRelease, this, &CameraComponent::OnRenderTargetRelease);
		}
		else
		{
			m_targetResizeSlot.Disconnect();
			m_targetReleaseSlot.Disconnect();
		}
	}

	inline void CameraComponent::SetTargetRegion(const Nz::Rectf& region)
	{
		m_targetRegion = region;

		InvalidateViewport();
	}

	inline void CameraComponent::SetViewport(const Nz::Recti& viewport)
	{
		NazaraAssert(m_target, "Component has no render target");

		// On calcule la région nécessaire pour produire ce viewport avec la taille actuelle de la cible
		float invWidth = 1.f/m_target->GetWidth();
		float invHeight = 1.f/m_target->GetHeight();

		SetTargetRegion(Nz::Rectf(invWidth * viewport.x, invHeight * viewport.y, invWidth * viewport.width, invHeight * viewport.height));
	}

	inline void CameraComponent::SetZFar(float zFar)
	{
		m_zFar = zFar;

		InvalidateProjectionMatrix();
	}

	inline void CameraComponent::SetZNear(float zNear)
	{
		NazaraAssert(!Nz::NumberEquals(zNear, 0.f), "zNear cannot be zero");
		m_zNear = zNear;

		InvalidateProjectionMatrix();
	}

	inline void CameraComponent::InvalidateFrustum() const
	{
		m_frustumUpdated = false;
	}

	inline void CameraComponent::InvalidateProjectionMatrix() const
	{
		m_frustumUpdated = false;
		m_projectionMatrixUpdated = false;
	}

	inline void CameraComponent::InvalidateViewMatrix() const
	{
		m_frustumUpdated = false;
		m_viewMatrixUpdated = false;
	}

	inline void CameraComponent::InvalidateViewport() const
	{
		m_frustumUpdated = false;
		m_projectionMatrixUpdated = false;
		m_viewportUpdated = false;
	}
}
