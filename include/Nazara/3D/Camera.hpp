// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CAMERA_HPP
#define NAZARA_CAMERA_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/3D/SceneNode.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>

class NzRenderTarget;

class NAZARA_API NzCamera : public NzSceneNode
{
	public:
		NzCamera();
		~NzCamera();

		void Activate() const;

		void EnsureFrustumUpdate() const;
		void EnsureProjectionMatrixUpdate() const;
		void EnsureViewMatrixUpdate() const;

		float GetAspectRatio() const;
		const NzBoundingBoxf& GetBoundingBox() const override;
		float GetFOV() const;
		const NzFrustumf& GetFrustum() const;
		const NzMatrix4f& GetProjectionMatrix() const;
		nzSceneNodeType GetSceneNodeType() const override;
		const NzVector3f& GetUpVector() const;
		const NzMatrix4f& GetViewMatrix() const;
		const NzRectf& GetViewport() const;
		float GetZFar() const;
		float GetZNear() const;

		void SetFOV(float fov);
		void SetUpVector(const NzVector3f& upVector);
		void SetViewport(const NzRectf& viewport);
		void SetZFar(float zFar);
		void SetZNear(float zNear);

	private:
		void AddToRenderQueue(NzRenderQueue& renderQueue) const;
		void Invalidate();
		bool IsVisible(const NzFrustumf& frustum) const override;
		void Register();
		void Unregister();
		void UpdateFrustum() const;
		void UpdateProjectionMatrix() const;
		void UpdateViewMatrix() const;

		mutable NzFrustumf m_frustum;
		mutable NzMatrix4f m_projectionMatrix;
		mutable NzMatrix4f m_viewMatrix;
		NzRectf m_viewport;
		NzVector3f m_upVector;
		mutable bool m_frustumUpdated;
		mutable bool m_projectionMatrixUpdated;
		mutable bool m_viewMatrixUpdated;
		mutable float m_aspectRatio;
		float m_fov;
		float m_zFar;
		float m_zNear;
};

#endif // NAZARA_CAMERA_HPP
