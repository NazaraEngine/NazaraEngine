// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/View.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzView::NzView() :
m_targetRegion(0.f, 0.f, 1.f, 1.f),
m_target(nullptr),
m_frustumUpdated(false),
m_projectionMatrixUpdated(false),
m_viewMatrixUpdated(false),
m_viewportUpdated(false),
m_zFar(1.f),
m_zNear(-1.f)
{
}

NzView::~NzView()
{
	if (m_target)
		m_target->RemoveListener(this);
}

void NzView::EnsureFrustumUpdate() const
{
	if (!m_frustumUpdated)
		UpdateFrustum();
}

void NzView::EnsureProjectionMatrixUpdate() const
{
	if (!m_projectionMatrixUpdated)
		UpdateProjectionMatrix();
}

void NzView::EnsureViewMatrixUpdate() const
{
	if (!m_viewMatrixUpdated)
		UpdateViewMatrix();
}

void NzView::EnsureViewportUpdate() const
{
	if (!m_viewportUpdated)
		UpdateViewport();
}

float NzView::GetAspectRatio() const
{
	return 1.f;
}

NzVector3f NzView::GetEyePosition() const
{
	return GetPosition(nzCoordSys_Global);
}

NzVector3f NzView::GetForward() const
{
	return NzNode::GetForward();
}

const NzFrustumf& NzView::GetFrustum() const
{
	if (!m_frustumUpdated)
		UpdateFrustum();

	return m_frustum;
}

const NzMatrix4f& NzView::GetProjectionMatrix() const
{
	if (!m_projectionMatrixUpdated)
		UpdateProjectionMatrix();

	return m_projectionMatrix;
}

const NzRenderTarget* NzView::GetTarget() const
{
	return m_target;
}

const NzRectf& NzView::GetTargetRegion() const
{
	return m_targetRegion;
}

const NzMatrix4f& NzView::GetViewMatrix() const
{
	if (!m_viewMatrixUpdated)
		UpdateViewMatrix();

	return m_viewMatrix;
}

const NzRectui& NzView::GetViewport() const
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_target)
	{
		NazaraError("Camera has no render target");
		return m_viewport;
	}
	#endif

	if (!m_viewportUpdated)
		UpdateViewport();

	return m_viewport;
}

float NzView::GetZFar() const
{
	return m_zFar;
}

float NzView::GetZNear() const
{
	return m_zNear;
}

void NzView::SetTarget(const NzRenderTarget* renderTarget)
{
	if (m_target)
		m_target->RemoveListener(this);

	m_target = renderTarget;
	if (m_target)
		m_target->AddListener(this);
}

void NzView::SetTarget(const NzRenderTarget& renderTarget)
{
	SetTarget(&renderTarget);
}

void NzView::SetTargetRegion(const NzRectf& region)
{
	m_targetRegion = region;

	m_frustumUpdated = false;
	m_projectionMatrixUpdated = false;
	m_viewportUpdated = false;
}

void NzView::SetViewport(const NzRectui& viewport)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_target)
	{
		NazaraError("Camera has no render target");
		return;
	}
	#endif

	// On calcule la région nécessaire pour produire ce viewport avec la taille actuelle de la cible
	float invWidth = 1.f/m_target->GetWidth();
	float invHeight = 1.f/m_target->GetHeight();

	SetTargetRegion(NzRectf(invWidth * viewport.x, invHeight * viewport.y, invWidth * viewport.width, invHeight * viewport.height));
}

void NzView::SetZFar(float zFar)
{
	m_zFar = zFar;

	m_frustumUpdated = false;
	m_projectionMatrixUpdated = false;
}

void NzView::SetZNear(float zNear)
{
	m_zNear = zNear;

	m_frustumUpdated = false;
	m_projectionMatrixUpdated = false;
}

void NzView::ApplyView() const
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_target)
	{
		NazaraError("Camera has no render target");
		return;
	}
	#endif

	if (!m_projectionMatrixUpdated)
		UpdateProjectionMatrix();

	if (!m_viewMatrixUpdated)
		UpdateViewMatrix();

	if (!m_viewportUpdated)
		UpdateViewport();

	NzRenderer::SetMatrix(nzMatrixType_Projection, m_projectionMatrix);
	NzRenderer::SetMatrix(nzMatrixType_View, m_viewMatrix);
	NzRenderer::SetTarget(m_target);
	NzRenderer::SetViewport(m_viewport);
}

void NzView::Invalidate()
{
	NzNode::Invalidate();

	// Le frustum et la view matrix dépendent des paramètres du node, invalidons-les
	m_frustumUpdated = false;
	m_viewMatrixUpdated = false;
}

void NzView::OnRenderTargetReleased(const NzRenderTarget* renderTarget, void* userdata)
{
	NazaraUnused(userdata);

	if (renderTarget == m_target)
		m_target = nullptr;
	else
		NazaraInternalError("Not listening to " + NzString::Pointer(renderTarget));
}

bool NzView::OnRenderTargetSizeChange(const NzRenderTarget* renderTarget, void* userdata)
{
	NazaraUnused(userdata);

	if (renderTarget == m_target)
	{
		m_frustumUpdated = false;
		m_projectionMatrixUpdated = false;
		m_viewportUpdated = false;
	}
	else
		NazaraInternalError("Not listening to " + NzString::Pointer(renderTarget));

	return true;
}

void NzView::UpdateFrustum() const
{
	if (!m_projectionMatrixUpdated)
		UpdateProjectionMatrix();

	if (!m_viewMatrixUpdated)
		UpdateViewMatrix();

	m_frustum.Extract(m_viewMatrix, m_projectionMatrix);
	m_frustumUpdated = true;
}

void NzView::UpdateProjectionMatrix() const
{
	if (!m_viewportUpdated)
		UpdateViewport();

	m_projectionMatrix.MakeOrtho(m_viewport.x, m_viewport.x + m_viewport.width, m_viewport.y, m_viewport.y + m_viewport.height, m_zNear, m_zFar);
	m_projectionMatrixUpdated = true;
}

void NzView::UpdateViewMatrix() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	m_viewMatrix.MakeViewMatrix(m_derivedPosition, m_derivedRotation);
	m_viewMatrixUpdated = true;
}

void NzView::UpdateViewport() const
{
	unsigned int width = m_target->GetWidth();
	unsigned int height = std::max(m_target->GetHeight(), 1U);

	m_viewport.x = width * m_targetRegion.x;
	m_viewport.y = height * m_targetRegion.y;
	m_viewport.width = width * m_targetRegion.width;
	m_viewport.height = height * m_targetRegion.height;
	m_viewportUpdated = true;
}
