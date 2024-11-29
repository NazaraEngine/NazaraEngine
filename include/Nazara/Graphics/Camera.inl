// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline Camera::Camera(std::shared_ptr<const RenderTarget> renderTarget, std::shared_ptr<PipelinePassList> pipelinePasses, ProjectionType projectionType) :
	m_framePipelinePasses(std::move(pipelinePasses)),
	m_clearColor(Color::Black()),
	m_fov(90.f),
	m_renderOrder(0),
	m_projectionType(projectionType),
	m_targetRegion(0.f, 0.f, 1.f, 1.f),
	m_viewport(0, 0, 0, 0),
	m_size(-1.f, -1.f),
	m_renderMask(0xFFFFFFFF),
	m_zFar((projectionType == ProjectionType::Perspective) ? 1000.f : 1.f),
	m_zNear((projectionType == ProjectionType::Perspective) ? 1.f : -1.f)
	{
		UpdateTarget(std::move(renderTarget));
	}

	inline Camera::Camera(const Camera& camera) :
	m_framePipelinePasses(camera.m_framePipelinePasses),
	m_clearColor(camera.m_clearColor),
	m_fov(camera.m_fov),
	m_renderOrder(camera.m_renderOrder),
	m_projectionType(camera.m_projectionType),
	m_targetRegion(camera.m_targetRegion),
	m_viewport(camera.m_viewport),
	m_size(camera.m_size),
	m_renderMask(camera.m_renderMask),
	m_aspectRatio(camera.m_aspectRatio),
	m_zFar(camera.m_zFar),
	m_zNear(camera.m_zNear)
	{
		UpdateTarget(camera.m_renderTarget);
	}

	inline Camera::Camera(Camera&& camera) noexcept :
	m_framePipelinePasses(std::move(camera.m_framePipelinePasses)),
	m_debugDrawer(std::move(camera.m_debugDrawer)),
	m_clearColor(camera.m_clearColor),
	m_fov(camera.m_fov),
	m_renderOrder(camera.m_renderOrder),
	m_projectionType(camera.m_projectionType),
	m_targetRegion(camera.m_targetRegion),
	m_viewport(camera.m_viewport),
	m_size(camera.m_size),
	m_renderMask(camera.m_renderMask),
	m_aspectRatio(camera.m_aspectRatio),
	m_zFar(camera.m_zFar),
	m_zNear(camera.m_zNear)
	{
		UpdateTarget(std::move(camera.m_renderTarget));
	}

	inline float Camera::GetAspectRatio() const
	{
		return m_aspectRatio;
	}

	inline DegreeAnglef Camera::GetFOV() const
	{
		return m_fov;
	}

	inline ProjectionType Camera::GetProjectionType() const
	{
		return m_projectionType;
	}

	inline Int32 Camera::GetRenderOrder() const
	{
		return m_renderOrder;
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

	inline void Camera::UpdateClearColor(Color color)
	{
		m_clearColor = color;
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
		NazaraAssertMsg(!NumberEquals(zNear, 0.f), "zNear cannot be zero");

		m_zNear = zNear;
		UpdateProjectionMatrix();
	}

	inline Camera& Camera::operator=(const Camera& camera)
	{
		m_framePipelinePasses = camera.m_framePipelinePasses;
		m_clearColor = camera.m_clearColor;
		m_fov = camera.m_fov;
		m_renderOrder = camera.m_renderOrder;
		m_projectionType = camera.m_projectionType;
		m_targetRegion = camera.m_targetRegion;
		m_viewport = camera.m_viewport;
		m_size = camera.m_size;
		m_renderMask = camera.m_renderMask;
		m_aspectRatio = camera.m_aspectRatio;
		m_zFar = camera.m_zFar;
		m_zNear = camera.m_zNear;

		UpdateTarget(camera.m_renderTarget);

		if (m_renderTarget)
			UpdateViewport();
		else
			UpdateViewport(m_viewport);

		return *this;
	}

	inline Camera& Camera::operator=(Camera&& camera) noexcept
	{
		m_framePipelinePasses = std::move(camera.m_framePipelinePasses);
		m_clearColor = camera.m_clearColor;
		m_fov = camera.m_fov;
		m_renderOrder = camera.m_renderOrder;
		m_projectionType = camera.m_projectionType;
		m_targetRegion = camera.m_targetRegion;
		m_viewport = camera.m_viewport;
		m_size = camera.m_size;
		m_renderMask = camera.m_renderMask;
		m_aspectRatio = camera.m_aspectRatio;
		m_zFar = camera.m_zFar;
		m_zNear = camera.m_zNear;

		UpdateTarget(std::move(camera.m_renderTarget));
		camera.UpdateTarget({});

		if (m_renderTarget)
			UpdateViewport();
		else
			UpdateViewport(m_viewport);

		return *this;
	}

	inline void Camera::UpdateProjectionType(ProjectionType projectionType)
	{
		m_projectionType = projectionType;
		UpdateProjectionMatrix();
	}

	inline void Camera::UpdateRenderMask(UInt32 renderMask)
	{
		m_renderMask = renderMask;

		OnRenderMaskUpdated(this, renderMask);
	}

	inline void Camera::UpdateRenderOrder(Int32 renderOrder)
	{
		m_renderOrder = renderOrder;

		// TODO: Signal RenderTechnique to regenerate commandbuffers
	}

	inline void Camera::UpdateTargetRegion(const Rectf& targetRegion)
	{
		m_targetRegion = targetRegion;
		UpdateViewport();
	}

	inline void Camera::UpdateViewport(const Recti& viewport)
	{
		if (m_renderTarget)
		{
			// We compute the region necessary to make this view port with the actual size of the target
			Vector2f invSize = 1.f / Vector2f(m_renderTarget->GetSize());

			UpdateTargetRegion(Rectf(invSize.x * viewport.x, invSize.y * viewport.y, invSize.x * viewport.width, invSize.y * viewport.height));
		}
		else
		{
			m_aspectRatio = float(viewport.width) / float(viewport.height);
			m_viewport = viewport;
			m_viewerInstance.UpdateTargetSize(Vector2f(viewport.GetLengths()));

			UpdateProjectionMatrix();
		}
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
					m_viewerInstance.UpdateProjectionMatrix(Matrix4f::Ortho(float(m_viewport.x), float(m_viewport.x + m_viewport.width), float(m_viewport.y), float(m_viewport.y + m_viewport.height), m_zNear, m_zFar));
				else
					m_viewerInstance.UpdateProjectionMatrix(Matrix4f::Ortho(0.f, m_size.x, 0.f, m_size.y, m_zNear, m_zFar));
				break;

			case ProjectionType::Perspective:
				m_viewerInstance.UpdateProjectionMatrix(Matrix4f::Perspective(m_fov, m_aspectRatio, m_zNear, m_zFar));
				break;
		}

		m_viewerInstance.UpdateNearFarPlanes(m_zNear, m_zFar);
	}

	inline void Camera::UpdateViewport()
	{
		NazaraAssertMsg(m_renderTarget, "no rendertarget!");
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
		m_viewport = Recti(fViewport);

		m_viewerInstance.UpdateTargetSize(fViewport.GetLengths());

		UpdateProjectionMatrix();
	}
}

