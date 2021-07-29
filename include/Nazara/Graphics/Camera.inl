// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Graphics/Components/CameraComponent.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline Camera::Camera(const RenderTarget* renderTarget, ProjectionType projectionType) :
	m_renderTarget(nullptr),
	m_fov(70.f),
	m_projectionType(projectionType),
	m_targetRegion(0.f, 0.f, 1.f, 1.f),
	m_viewport(0, 0, 0, 0),
	m_size(-1.f, -1.f),
	m_zFar(1000.f),
	m_zNear(1.f)
	{
		UpdateTarget(renderTarget);
	}

	inline Camera::Camera(const Camera& camera) :
	m_renderTarget(nullptr),
	m_fov(camera.m_fov),
	m_projectionType(camera.m_projectionType),
	m_targetRegion(camera.m_targetRegion),
	m_viewport(camera.m_viewport),
	m_size(camera.m_size),
	m_aspectRatio(camera.m_aspectRatio),
	m_zFar(camera.m_zFar),
	m_zNear(camera.m_zNear)
	{
		UpdateTarget(camera.m_renderTarget);
	}

	inline Camera::Camera(Camera&& camera) noexcept :
	m_renderTarget(nullptr),
	m_fov(camera.m_fov),
	m_projectionType(camera.m_projectionType),
	m_targetRegion(camera.m_targetRegion),
	m_viewport(camera.m_viewport),
	m_size(camera.m_size),
	m_aspectRatio(camera.m_aspectRatio),
	m_zFar(camera.m_zFar),
	m_zNear(camera.m_zNear)
	{
		UpdateTarget(camera.m_renderTarget);
	}

	inline float Camera::GetAspectRatio() const
	{
		return m_aspectRatio;
	}

	inline DegreeAnglef Camera::GetFOV() const
	{
		return m_fov;
	}

	inline const Vector2f& Camera::GetSize() const
	{
		return m_size;
	}

	inline const Rectf& Camera::GetTargetRegion() const
	{
		return m_targetRegion;
	}

	inline float Camera::GetZFar() const
	{
		return m_zFar;
	}

	inline float Camera::GetZNear() const
	{
		return m_zNear;
	}

	inline void Camera::UpdateFOV(DegreeAnglef fov)
	{
		m_fov = fov;
		UpdateProjectionMatrix();
	}

	inline void Camera::UpdateZFar(float zFar)
	{
		m_zFar = zFar;
		UpdateProjectionMatrix();
	}

	inline void Camera::UpdateZNear(float zNear)
	{
		NazaraAssert(!NumberEquals(zNear, 0.f), "zNear cannot be zero");

		m_zNear = zNear;
		UpdateProjectionMatrix();
	}

	inline Camera& Camera::operator=(const Camera& camera)
	{
		m_fov = camera.m_fov;
		m_projectionType = camera.m_projectionType;
		m_targetRegion = camera.m_targetRegion;
		m_viewport = camera.m_viewport;
		m_size = camera.m_size;
		m_aspectRatio = camera.m_aspectRatio;
		m_zFar = camera.m_zFar;
		m_zNear = camera.m_zNear;

		UpdateTarget(camera.m_renderTarget);

		return *this;
	}

	inline Camera& Camera::operator=(Camera&& camera) noexcept
	{
		m_fov = camera.m_fov;
		m_projectionType = camera.m_projectionType;
		m_targetRegion = camera.m_targetRegion;
		m_viewport = camera.m_viewport;
		m_size = camera.m_size;
		m_aspectRatio = camera.m_aspectRatio;
		m_zFar = camera.m_zFar;
		m_zNear = camera.m_zNear;

		UpdateTarget(camera.m_renderTarget);

		return *this;
	}

	inline void Camera::UpdateProjectionType(ProjectionType projectionType)
	{
		m_projectionType = projectionType;
		UpdateProjectionMatrix();
	}

	inline void Camera::UpdateTargetRegion(const Rectf& targetRegion)
	{
		m_targetRegion = targetRegion;
		UpdateViewport();
	}

	inline void Camera::UpdateViewport(const Recti& viewport)
	{
		NazaraAssert(m_renderTarget, "no render target");

		// We compute the region necessary to make this view port with the actual size of the target
		Vector2f invSize = 1.f / Vector2f(m_renderTarget->GetSize());

		UpdateTargetRegion(Rectf(invSize.x * viewport.x, invSize.y * viewport.y, invSize.x * viewport.width, invSize.y * viewport.height));
	}

	inline void Camera::UpdateSize(const Vector2f& size)
	{
		m_size = size;
		UpdateProjectionMatrix();
	}

	inline void Camera::UpdateProjectionMatrix()
	{
		switch (m_projectionType)
		{
			case ProjectionType::Orthographic:
				if (m_size.x < 0.f || m_size.y < 0.f)
					m_viewerInstance.UpdateProjectionMatrix(Matrix4f::Ortho(0.f, float(m_viewport.x), 0.f, float(m_viewport.y), m_zNear, m_zFar));
				else
					m_viewerInstance.UpdateProjectionMatrix(Matrix4f::Ortho(0.f, m_size.x, 0.f, m_size.y, m_zNear, m_zFar));
				break;

			case ProjectionType::Perspective:
				m_viewerInstance.UpdateProjectionMatrix(Matrix4f::Perspective(m_fov, m_aspectRatio, m_zNear, m_zFar));
				break;
		}
	}

	inline void Camera::UpdateViewport()
	{
		NazaraAssert(m_renderTarget, "no rendertarget!");
		return UpdateViewport(m_renderTarget->GetSize());
	}

	inline void Camera::UpdateViewport(Vector2ui renderTargetSize)
	{
		renderTargetSize.y = std::max(renderTargetSize.y, 1U); // Let's make sure we won't divide by zero

		// Our target region is expressed as % of the viewport dimensions, let's compute it in pixels
		Rectf fViewport(m_targetRegion);
		fViewport.x *= renderTargetSize.x;
		fViewport.y *= renderTargetSize.y;
		fViewport.width *= renderTargetSize.x;
		fViewport.height *= renderTargetSize.y;

		m_aspectRatio = fViewport.width / fViewport.height;

		// Convert it back to int
		m_viewport.Set(fViewport);

		UpdateProjectionMatrix();
	}
}
