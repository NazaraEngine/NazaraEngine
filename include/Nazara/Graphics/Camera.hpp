// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CAMERA_HPP
#define NAZARA_CAMERA_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/SceneNode.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>

class NAZARA_API NzCamera : public NzAbstractViewer, public NzNode, NzRenderTarget::Listener
{
	public:
		NzCamera();
		~NzCamera();

		void EnsureFrustumUpdate() const;
		void EnsureProjectionMatrixUpdate() const;
		void EnsureViewMatrixUpdate() const;
		void EnsureViewportUpdate() const;

		float GetAspectRatio() const;
		NzVector3f GetEyePosition() const;
		NzVector3f GetForward() const;
		float GetFOV() const;
		const NzFrustumf& GetFrustum() const;
		const NzMatrix4f& GetProjectionMatrix() const;
		const NzRenderTarget* GetTarget() const;
		const NzRectf& GetTargetRegion() const;
		const NzMatrix4f& GetViewMatrix() const;
		const NzRecti& GetViewport() const;
		float GetZFar() const;
		float GetZNear() const;

		void SetFOV(float fov);
		void SetTarget(const NzRenderTarget* renderTarget);
		void SetTarget(const NzRenderTarget& renderTarget);
		void SetTargetRegion(const NzRectf& region);
		void SetViewport(const NzRecti& viewport);
		void SetZFar(float zFar);
		void SetZNear(float zNear);

	private:
		void ApplyView() const override;
		void InvalidateNode() override;

		void OnRenderTargetReleased(const NzRenderTarget* renderTarget, void* userdata) override;
		bool OnRenderTargetSizeChange(const NzRenderTarget* renderTarget, void* userdata) override;

		void UpdateFrustum() const;
		void UpdateProjectionMatrix() const;
		void UpdateViewMatrix() const;
		void UpdateViewport() const;

		mutable NzFrustumf m_frustum;
		mutable NzMatrix4f m_projectionMatrix;
		mutable NzMatrix4f m_viewMatrix;
		NzRectf m_targetRegion;
		mutable NzRecti m_viewport;
		const NzRenderTarget* m_target;
		mutable bool m_frustumUpdated;
		mutable bool m_projectionMatrixUpdated;
		mutable bool m_viewMatrixUpdated;
		mutable bool m_viewportUpdated;
		mutable float m_aspectRatio;
		float m_fov;
		float m_zFar;
		float m_zNear;
};

#endif // NAZARA_CAMERA_HPP
