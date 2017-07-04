// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/CameraComponent.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Algorithm.hpp>

namespace Ndk
{
	/*!
	* \brief Constructs an CameraComponent object by default
	*/

	inline CameraComponent::CameraComponent() :
	m_visibilityHash(0U),
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

	/*!
	* \brief Constructs a CameraComponent object by copy semantic
	*
	* \param camera CameraComponent to copy
	*/

	inline CameraComponent::CameraComponent(const CameraComponent& camera) :
	Component(camera),
	AbstractViewer(camera),
	HandledObject(camera),
	m_visibilityHash(camera.m_visibilityHash),
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

	/*!
	* \brief Ensures the frustum is up to date
	*/

	inline void CameraComponent::EnsureFrustumUpdate() const
	{
		if (!m_frustumUpdated)
			UpdateFrustum();
	}

	/*!
	* \brief Ensures the projection matrix is up to date
	*/

	inline void CameraComponent::EnsureProjectionMatrixUpdate() const
	{
		if (!m_projectionMatrixUpdated)
			UpdateProjectionMatrix();
	}

	/*!
	* \brief Ensures the view matrix is up to date
	*/

	inline void CameraComponent::EnsureViewMatrixUpdate() const
	{
		if (!m_viewMatrixUpdated)
			UpdateViewMatrix();
	}

	/*!
	* \brief Ensures the view port is up to date
	*/

	inline void CameraComponent::EnsureViewportUpdate() const
	{
		if (!m_viewportUpdated)
			UpdateViewport();
	}

	/*!
	* \brief Gets the field of view of the camera
	* \return Field of view of the camera
	*/
	inline float CameraComponent::GetFOV() const
	{
		return m_fov;
	}

	/*!
	* \brief Gets the layer of the camera
	* \return Layer of the camera
	*/

	inline unsigned int CameraComponent::GetLayer() const
	{
		return m_layer;
	}

	/*!
	* \brief Gets the size of the camera
	* \return Size of the camera
	*/

	inline const Nz::Vector2f & CameraComponent::GetSize() const
	{
		return m_size;
	}

	/*!
	* \brief Gets the target region of the camera
	* \return A constant reference to the target region of the camera
	*/

	inline const Nz::Rectf& CameraComponent::GetTargetRegion() const
	{
		return m_targetRegion;
	}

	/*!
	* \brief Sets the field of view of the camera
	*
	* \param fov Field of view of the camera
	*
	* \remark Produces a NazaraAssert if angle is zero
	*/

	inline void CameraComponent::SetFOV(float fov)
	{
		NazaraAssert(!Nz::NumberEquals(fov, 0.f), "FOV must be different from zero");
		m_fov = fov;

		InvalidateProjectionMatrix();
	}

	/*!
	* \brief Sets the projection type of the camera
	*
	* \param projectionType Projection type of the camera
	*/

	inline void CameraComponent::SetProjectionType(Nz::ProjectionType projectionType)
	{
		m_projectionType = projectionType;

		InvalidateProjectionMatrix();
	}

	/*!
	* \brief Sets the size of the camera
	*
	* \param size Size of the camera
	*/

	inline void CameraComponent::SetSize(const Nz::Vector2f& size)
	{
		m_size = size;

		InvalidateProjectionMatrix();
	}

	/*!
	* \brief Sets the size of the camera
	*
	* \param width Size in X of the camera
	* \param height Size in Y of the camera
	*/

	inline void CameraComponent::SetSize(float width, float height)
	{
		SetSize({width, height});
	}

	/*!
	* \brief Sets the target of the camera
	*
	* \param renderTarget A constant reference to the render target of the camera
	*/

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

	/*!
	* \brief Sets the target region of the camera
	*
	* \param region A constant reference to the target region of the camera
	*/

	inline void CameraComponent::SetTargetRegion(const Nz::Rectf& region)
	{
		m_targetRegion = region;

		InvalidateViewport();
	}

	/*!
	* \brief Sets the view port of the camera
	*
	* \param viewport A constant reference to the view port of the camera
	*
	* \remark Produces a NazaraAssert if the camera has no target
	*/

	inline void CameraComponent::SetViewport(const Nz::Recti& viewport)
	{
		NazaraAssert(m_target, "Component has no render target");

		// We compute the region necessary to make this view port with the actual size of the target
		float invWidth = 1.f / m_target->GetWidth();
		float invHeight = 1.f / m_target->GetHeight();

		SetTargetRegion(Nz::Rectf(invWidth * viewport.x, invHeight * viewport.y, invWidth * viewport.width, invHeight * viewport.height));
	}

	/*!
	* \brief Sets the Z far distance of the camera
	*
	* \param zFar Z far distance of the camera
	*/

	inline void CameraComponent::SetZFar(float zFar)
	{
		m_zFar = zFar;

		InvalidateProjectionMatrix();
	}

	/*!
	* \brief Sets the Z near distance of the camera
	*
	* \param zNear Z near distance of the camera
	*
	* \remark Produces a NazaraAssert if zNear is zero
	*/

	inline void CameraComponent::SetZNear(float zNear)
	{
		NazaraAssert(!Nz::NumberEquals(zNear, 0.f), "zNear cannot be zero");
		m_zNear = zNear;

		InvalidateProjectionMatrix();
	}

	/*!
	* \brief Update the camera component visibility hash
	*
	* This is used with CullingList (which produce a visibility hash)
	*
	* \param visibilityHash New visibility hash
	*
	* \return True if the visibility hash is not the same as before
	*/
	inline bool CameraComponent::UpdateVisibility(std::size_t visibilityHash)
	{
		if (m_visibilityHash != visibilityHash)
		{
			m_visibilityHash = visibilityHash;
			return true;
		}

		return false;
	}

	/*!
	* \brief Invalidates the frustum
	*/

	inline void CameraComponent::InvalidateFrustum() const
	{
		m_frustumUpdated = false;
	}

	/*!
	* \brief Invalidates the projection matrix
	*/

	inline void CameraComponent::InvalidateProjectionMatrix() const
	{
		m_frustumUpdated = false;
		m_projectionMatrixUpdated = false;
	}

	/*!
	* \brief Invalidates the view matrix
	*/

	inline void CameraComponent::InvalidateViewMatrix() const
	{
		m_frustumUpdated = false;
		m_viewMatrixUpdated = false;
	}

	/*!
	* \brief Invalidates the view port
	*/

	inline void CameraComponent::InvalidateViewport() const
	{
		m_frustumUpdated = false;
		m_projectionMatrixUpdated = false;
		m_viewportUpdated = false;
	}
}
