// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/View.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzView::NzView() :
m_targetRegion(0.f, 0.f, 1.f, 1.f),
m_size(0.f),
m_target(nullptr),
m_frustumUpdated(false),
m_invViewProjMatrixUpdated(false),
m_projectionMatrixUpdated(false),
m_viewMatrixUpdated(false),
m_viewProjMatrixUpdated(false),
m_viewportUpdated(false),
m_zFar(1.f),
m_zNear(-1.f)
{
}

NzView::NzView(const NzVector2f& size) :
NzView() // On délègue
{
	SetSize(size);
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

NzVector3f NzView::GetGlobalForward() const
{
	return NzVector3f::UnitZ();
}

NzVector3f NzView::GetGlobalRight() const
{
	return NzVector3f::UnitX();
}

NzVector3f NzView::GetGlobalUp() const
{
	return -NzVector3f::UnitY();
}

const NzMatrix4f& NzView::GetInvViewProjMatrix() const
{
	if (!m_invViewProjMatrixUpdated)
		UpdateInvViewProjMatrix();

	return m_invViewProjMatrix;
}

const NzMatrix4f& NzView::GetProjectionMatrix() const
{
	if (!m_projectionMatrixUpdated)
		UpdateProjectionMatrix();

	return m_projectionMatrix;
}

const NzVector2f& NzView::GetSize() const
{
	return m_size;
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

const NzMatrix4f& NzView::GetViewProjMatrix() const
{
	if (!m_viewProjMatrixUpdated)
		UpdateViewProjMatrix();

	return m_viewProjMatrix;
}

const NzRecti& NzView::GetViewport() const
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

NzVector2f NzView::MapPixelToWorld(const NzVector2i& pixel)
{
	if (!m_invViewProjMatrixUpdated)
		UpdateInvViewProjMatrix();

	if (!m_viewportUpdated)
		UpdateViewport();

	// Conversion du viewport en flottant
	NzRectf viewport(m_viewport);

	NzVector2f normalized;
	normalized.x = -1.f + 2.f * (pixel.x - viewport.x) / viewport.width;
	normalized.y =  1.f - 2.f * (pixel.y - viewport.y) / viewport.height;

	return m_invViewProjMatrix.Transform(normalized);
}

NzVector2i NzView::MapWorldToPixel(const NzVector2f& coords)
{
	if (!m_viewProjMatrixUpdated)
		UpdateViewProjMatrix();

	if (!m_viewportUpdated)
		UpdateViewport();

	// Conversion du viewport en flottant
	NzRectf viewport(m_viewport);

	NzVector2f normalized = m_viewProjMatrix.Transform(coords);

	NzVector2i pixel;
	pixel.x = static_cast<int>(( normalized.x + 1.f) * viewport.width / 2.f + viewport.x);
	pixel.y = static_cast<int>((-normalized.y + 1.f) * viewport.width / 2.f + viewport.y);

	return pixel;
}

void NzView::SetSize(const NzVector2f& size)
{
	SetSize(size.x, size.y);
}

void NzView::SetSize(float width, float height)
{
	m_size.Set(width, height);
	m_projectionMatrixUpdated = false;
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
	m_invViewProjMatrixUpdated = false;
	m_projectionMatrixUpdated = false;
	m_viewProjMatrixUpdated = false;
	m_viewportUpdated = false;
}

void NzView::SetViewport(const NzRecti& viewport)
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
	m_invViewProjMatrixUpdated = false;
	m_projectionMatrixUpdated = false;
	m_viewProjMatrixUpdated = false;
}

void NzView::SetZNear(float zNear)
{
	m_zNear = zNear;

	m_frustumUpdated = false;
	m_invViewProjMatrixUpdated = false;
	m_projectionMatrixUpdated = false;
	m_viewProjMatrixUpdated = false;
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

void NzView::InvalidateNode()
{
	NzNode::InvalidateNode();

	// Le frustum et la view matrix dépendent des paramètres du node, invalidons-les
	m_frustumUpdated = false;
	m_invViewProjMatrixUpdated = false;
	m_viewMatrixUpdated = false;
	m_viewProjMatrixUpdated = false;
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

void NzView::UpdateInvViewProjMatrix() const
{
	if (!m_viewProjMatrixUpdated)
		UpdateViewProjMatrix();

	m_viewProjMatrix.GetInverseAffine(&m_invViewProjMatrix);
	m_invViewProjMatrixUpdated = true;
}

void NzView::UpdateProjectionMatrix() const
{
	if (m_size.x <= 0.f || m_size.y <= 0.f) // Si la taille est nulle, on prendra la taille du viewport
	{
		if (!m_viewportUpdated)
			UpdateViewport();

		m_projectionMatrix.MakeOrtho(0.f, static_cast<float>(m_viewport.width), 0.f, static_cast<float>(m_viewport.height), m_zNear, m_zFar);
	}
	else
		m_projectionMatrix.MakeOrtho(0.f, m_size.x, 0.f, m_size.y, m_zNear, m_zFar);

	m_projectionMatrixUpdated = true;
}

void NzView::UpdateViewMatrix() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	m_viewMatrix.MakeViewMatrix(m_derivedPosition, m_derivedRotation);
	m_viewMatrixUpdated = true;
}

void NzView::UpdateViewProjMatrix() const
{
	if (!m_projectionMatrixUpdated)
		UpdateProjectionMatrix();

	if (!m_viewMatrixUpdated)
		UpdateViewMatrix();

	// La matrice de projection orthogonale est affine
	m_viewProjMatrix = NzMatrix4f::ConcatenateAffine(m_viewMatrix, m_projectionMatrix);
	m_viewProjMatrixUpdated = true;
}

void NzView::UpdateViewport() const
{
	unsigned int width = m_target->GetWidth();
	unsigned int height = std::max(m_target->GetHeight(), 1U);

	m_viewport.x = static_cast<int>(width * m_targetRegion.x);
	m_viewport.y = static_cast<int>(height * m_targetRegion.y);
	m_viewport.width = static_cast<int>(width * m_targetRegion.width);
	m_viewport.height = static_cast<int>(height * m_targetRegion.height);
	m_viewportUpdated = true;
}
