// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>

namespace Ndk
{
	inline CameraComponent::CameraComponent() :
	m_targetRegion(0.f, 0.f, 1.f, 1.f),
	m_target(nullptr),
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
	NzAbstractViewer(camera),
	m_targetRegion(camera.m_targetRegion),
	m_target(camera.m_target),
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

	inline const NzFrustumf& CameraComponent::GetFrustum() const
	{
		EnsureFrustumUpdate();

		return m_frustum;
	}

	inline unsigned int CameraComponent::GetLayer() const
	{
		return m_layer;
	}

	inline const NzMatrix4f& CameraComponent::GetProjectionMatrix() const
	{
		EnsureProjectionMatrixUpdate();

		return m_projectionMatrix;
	}

	inline const NzRenderTarget* CameraComponent::GetTarget() const
	{
		return m_target;
	}

	inline const NzRectf& CameraComponent::GetTargetRegion() const
	{
		return m_targetRegion;
	}

	inline const NzMatrix4f& CameraComponent::GetViewMatrix() const
	{
		EnsureViewMatrixUpdate();

		return m_viewMatrix;
	}

	inline const NzRecti& CameraComponent::GetViewport() const
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
		NazaraAssert(fov != 0.f, "FOV must be different from zero");

		m_fov = fov;
		InvalidateProjectionMatrix();
	}

	inline void CameraComponent::SetTarget(const NzRenderTarget* renderTarget)
	{
		m_target = renderTarget;
		if (m_target)
			m_targetReleaseSlot = NazaraConnectThis(*m_target, OnRenderTargetRelease, OnRenderTargetRelease);
		else
			NazaraDisconnect(m_targetReleaseSlot);
	}

	inline void CameraComponent::SetTargetRegion(const NzRectf& region)
	{
		m_targetRegion = region;
		InvalidateViewport();
	}

	inline void CameraComponent::SetViewport(const NzRecti& viewport)
	{
		NazaraAssert(m_target, "Component has no render target");

		// On calcule la région nécessaire pour produire ce viewport avec la taille actuelle de la cible
		float invWidth = 1.f/m_target->GetWidth();
		float invHeight = 1.f/m_target->GetHeight();

		SetTargetRegion(NzRectf(invWidth * viewport.x, invHeight * viewport.y, invWidth * viewport.width, invHeight * viewport.height));
	}

	inline void CameraComponent::SetZFar(float zFar)
	{
		m_zFar = zFar;
		InvalidateProjectionMatrix();
	}

	inline void CameraComponent::SetZNear(float zNear)
	{
		NazaraAssert(!NzNumberEquals(zNear, 0.f), "zNear cannot be zero");

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
