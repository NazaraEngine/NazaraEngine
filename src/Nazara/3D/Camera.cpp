// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/3D/Camera.hpp>
#include <Nazara/3D/Scene.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/3D/Debug.hpp>

NzCamera::NzCamera() :
m_viewport(0.f, 0.f, 1.f, 1.f),
m_upVector(NzVector3f::Up()),
m_frustumUpdated(false),
m_projectionMatrixUpdated(false),
m_viewMatrixUpdated(false),
m_aspectRatio(0.f),
m_fov(70.f),
m_zFar(100.f),
m_zNear(1.f)
{
}

NzCamera::~NzCamera() = default;

void NzCamera::Activate() const
{
	#ifdef NAZARA_3D_SAFE
	if (!m_target)
	{
		NazaraError("No render target !");
		return;
	}
	#endif

	NzRenderer::SetTarget(m_target);

	unsigned int width = m_target->GetWidth();
	unsigned int height = std::max(m_target->GetHeight(), 1U);

	float vWidth = width * m_viewport.width;
	float vHeight = height * m_viewport.height;

	NzRectui viewport;
	viewport.x = width * m_viewport.x;
	viewport.y = height * m_viewport.x;
	viewport.width = vWidth;
	viewport.height = height * m_viewport.height;
	NzRenderer::SetViewport(viewport);

	float aspectRatio = vWidth/vHeight;

	if (!NzNumberEquals(m_aspectRatio, aspectRatio))
	{
		m_aspectRatio = aspectRatio;
		m_frustumUpdated = false;
		m_projectionMatrixUpdated = false;
	}

	if (!m_projectionMatrixUpdated)
		UpdateProjectionMatrix();

	if (!m_viewMatrixUpdated)
		UpdateViewMatrix();

	NzRenderer::SetMatrix(nzMatrixType_Projection, m_projectionMatrix);
	NzRenderer::SetMatrix(nzMatrixType_View, m_viewMatrix);

	if (m_scene)
		m_scene->SetActiveCamera(this);
}

void NzCamera::EnsureFrustumUpdate() const
{
	if (!m_frustumUpdated)
		UpdateFrustum();
}

void NzCamera::EnsureProjectionMatrixUpdate() const
{
	if (!m_projectionMatrixUpdated)
		UpdateProjectionMatrix();
}

void NzCamera::EnsureViewMatrixUpdate() const
{
	if (!m_viewMatrixUpdated)
		UpdateViewMatrix();
}

float NzCamera::GetAspectRatio() const
{
	return m_aspectRatio;
}

const NzBoundingBoxf& NzCamera::GetBoundingBox() const
{
	///TODO: Remplacer par la bounding box du Frustum ?
	static NzBoundingBoxf dummy(nzExtend_Null);
	return dummy;
}

float NzCamera::GetFOV() const
{
	return m_fov;
}

const NzFrustumf& NzCamera::GetFrustum() const
{
	if (!m_frustumUpdated)
		UpdateFrustum();

	return m_frustum;
}

const NzMatrix4f& NzCamera::GetProjectionMatrix() const
{
	if (!m_projectionMatrixUpdated)
		UpdateProjectionMatrix();

	return m_projectionMatrix;
}

nzSceneNodeType NzCamera::GetSceneNodeType() const
{
	return nzSceneNodeType_Camera;
}

const NzRenderTarget* NzCamera::GetTarget() const
{
	return m_target;
}

const NzVector3f& NzCamera::GetUpVector() const
{
	return m_upVector;
}

const NzMatrix4f& NzCamera::GetViewMatrix() const
{
	if (!m_viewMatrixUpdated)
		UpdateViewMatrix();

	return m_viewMatrix;
}

const NzRectf& NzCamera::GetViewport() const
{
	return m_viewport;
}

float NzCamera::GetZFar() const
{
	return m_zFar;
}

float NzCamera::GetZNear() const
{
	return m_zNear;
}

void NzCamera::SetFOV(float fov)
{
	m_fov = fov;

	m_frustumUpdated = false;
	m_projectionMatrixUpdated= false;
}

void NzCamera::SetTarget(const NzRenderTarget* renderTarget)
{
	m_target = renderTarget;
}

void NzCamera::SetTarget(const NzRenderTarget& renderTarget)
{
	SetTarget(&renderTarget);
}

void NzCamera::SetUpVector(const NzVector3f& upVector)
{
	m_upVector = upVector;

	m_frustumUpdated = false;
	m_viewMatrixUpdated = false;
}

void NzCamera::SetViewport(const NzRectf& viewport)
{
	m_viewport = viewport;
}

void NzCamera::SetZFar(float zFar)
{
	m_zFar = zFar;

	m_frustumUpdated = false;
	m_projectionMatrixUpdated = false;
}

void NzCamera::SetZNear(float zNear)
{
	m_zNear = zNear;

	m_frustumUpdated = false;
	m_projectionMatrixUpdated = false;
}

void NzCamera::AddToRenderQueue(NzRenderQueue& renderQueue) const
{
	NazaraUnused(renderQueue);

	NazaraInternalError("SceneNode::AddToRenderQueue() called on SceneRoot");
}

void NzCamera::Invalidate()
{
	NzSceneNode::Invalidate();

	m_frustumUpdated = false;
	m_viewMatrixUpdated = false;
}

void NzCamera::Register()
{
}

void NzCamera::Unregister()
{
}

void NzCamera::UpdateFrustum() const
{
	m_frustum.Build(m_fov, m_aspectRatio, m_zNear, m_zFar, m_derivedPosition, m_derivedPosition + m_derivedRotation*NzVector3f::Forward(), m_upVector);
	m_frustumUpdated = true;
}

void NzCamera::UpdateProjectionMatrix() const
{
	m_projectionMatrix.MakePerspective(m_fov, m_aspectRatio, m_zNear, m_zFar);
	m_projectionMatrixUpdated = true;
}

void NzCamera::UpdateViewMatrix() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	m_viewMatrix.MakeLookAt(m_derivedPosition, m_derivedPosition + m_derivedRotation*NzVector3f::Forward(), m_upVector);
	m_viewMatrixUpdated = true;
}

bool NzCamera::VisibilityTest(const NzFrustumf& frustum)
{
	NazaraUnused(frustum);
	//NazaraInternalError("SceneNode::IsVisible() called on Camera");
	return false;
}
