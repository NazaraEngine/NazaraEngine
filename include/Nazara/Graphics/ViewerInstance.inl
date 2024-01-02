// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const Vector3f& ViewerInstance::GetEyePosition() const
	{
		return m_eyePosition;
	}

	inline float ViewerInstance::GetFarPlane() const
	{
		return m_farPlane;
	}

	inline const Matrix4f& ViewerInstance::GetInvProjectionMatrix() const
	{
		return m_invProjectionMatrix;
	}

	inline const Matrix4f& ViewerInstance::GetInvViewMatrix() const
	{
		return m_invViewMatrix;
	}

	inline const Matrix4f& ViewerInstance::GetInvViewProjMatrix() const
	{
		return m_invViewProjMatrix;
	}

	inline float ViewerInstance::GetNearPlane() const
	{
		return m_nearPlane;
	}

	inline const Matrix4f& ViewerInstance::GetProjectionMatrix() const
	{
		return m_projectionMatrix;
	}

	inline const Vector2f& ViewerInstance::GetTargetSize() const
	{
		return m_targetSize;
	}

	inline const Matrix4f& ViewerInstance::GetViewMatrix() const
	{
		return m_viewMatrix;
	}

	inline const Matrix4f& ViewerInstance::GetViewProjMatrix() const
	{
		return m_viewProjMatrix;
	}

	inline std::shared_ptr<RenderBuffer>& ViewerInstance::GetViewerBuffer()
	{
		return m_viewerDataBuffer;
	}

	inline const std::shared_ptr<RenderBuffer>& ViewerInstance::GetViewerBuffer() const
	{
		return m_viewerDataBuffer;
	}

	inline void ViewerInstance::UpdateEyePosition(const Vector3f& eyePosition)
	{
		m_eyePosition = eyePosition;

		InvalidateData();
	}

	inline void ViewerInstance::UpdateNearFarPlanes(float nearPlane, float farPlane)
	{
		m_farPlane = farPlane;
		m_nearPlane = nearPlane;

		InvalidateData();
	}

	inline void ViewerInstance::UpdateProjectionMatrix(const Matrix4f& projectionMatrix)
	{
		m_projectionMatrix = projectionMatrix;
		if (!m_projectionMatrix.GetInverse(&m_invProjectionMatrix))
			NazaraError("failed to inverse projection matrix");

		m_viewProjMatrix = m_viewMatrix * m_projectionMatrix;
		m_invViewProjMatrix = m_invProjectionMatrix * m_invViewMatrix;

		InvalidateData();
	}

	inline void ViewerInstance::UpdateProjectionMatrix(const Matrix4f& projectionMatrix, const Matrix4f& invProjectionMatrix)
	{
		m_projectionMatrix = projectionMatrix;
		m_invProjectionMatrix = invProjectionMatrix;

		m_viewProjMatrix = m_viewMatrix * m_projectionMatrix;
		m_invViewProjMatrix = m_invProjectionMatrix * m_invViewMatrix;

		InvalidateData();
	}

	inline void ViewerInstance::UpdateProjViewMatrices(const Matrix4f& projectionMatrix, const Matrix4f& viewMatrix)
	{
		m_projectionMatrix = projectionMatrix;
		if (!m_projectionMatrix.GetInverse(&m_invProjectionMatrix))
			NazaraError("failed to inverse projection matrix");

		m_viewMatrix = viewMatrix;
		if (!m_viewMatrix.GetInverseTransform(&m_invViewMatrix))
			NazaraError("failed to inverse view matrix");

		m_viewProjMatrix = m_viewMatrix * m_projectionMatrix;
		m_invViewProjMatrix = m_invProjectionMatrix * m_invViewMatrix;

		InvalidateData();
	}

	inline void ViewerInstance::UpdateProjViewMatrices(const Matrix4f& projectionMatrix, const Matrix4f& invProjectionMatrix, const Matrix4f& viewMatrix, const Matrix4f& invViewMatrix)
	{
		m_projectionMatrix = projectionMatrix;
		m_viewMatrix = viewMatrix;
		m_invProjectionMatrix = invProjectionMatrix;
		m_invViewMatrix = invViewMatrix;

		m_viewProjMatrix = m_viewMatrix * m_projectionMatrix;
		m_invViewProjMatrix = m_invProjectionMatrix * m_invViewMatrix;

		InvalidateData();
	}

	inline void ViewerInstance::UpdateProjViewMatrices(const Matrix4f& projectionMatrix, const Matrix4f& invProjectionMatrix, const Matrix4f& viewMatrix, const Matrix4f& invViewMatrix, const Matrix4f& viewProjMatrix, const Matrix4f& invViewProjMatrix)
	{
		m_projectionMatrix = projectionMatrix;
		m_viewMatrix = viewMatrix;
		m_invProjectionMatrix = invProjectionMatrix;
		m_invViewMatrix = invViewMatrix;

		m_viewProjMatrix = viewProjMatrix;
		m_invViewProjMatrix = invViewProjMatrix;

		InvalidateData();
	}

	inline void ViewerInstance::UpdateTargetSize(const Vector2f& targetSize)
	{
		m_targetSize = targetSize;

		InvalidateData();
	}

	inline void ViewerInstance::UpdateViewMatrix(const Matrix4f& viewMatrix)
	{
		m_viewMatrix = viewMatrix;
		if (!m_viewMatrix.GetInverseTransform(&m_invViewMatrix))
			NazaraError("failed to inverse view matrix");

		m_viewProjMatrix = m_viewMatrix * m_projectionMatrix;
		m_invViewProjMatrix = m_invProjectionMatrix * m_invViewMatrix;

		InvalidateData();
	}

	inline void ViewerInstance::UpdateViewMatrix(const Matrix4f& viewMatrix, const Matrix4f& invViewMatrix)
	{
		m_viewMatrix = viewMatrix;
		m_invViewMatrix = invViewMatrix;

		m_viewProjMatrix = m_viewMatrix * m_projectionMatrix;
		m_invViewProjMatrix = m_invProjectionMatrix * m_invViewMatrix;

		InvalidateData();
	}

	inline void ViewerInstance::InvalidateData()
	{
		m_dataInvalidated = true;
		OnTransferRequired(this);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
