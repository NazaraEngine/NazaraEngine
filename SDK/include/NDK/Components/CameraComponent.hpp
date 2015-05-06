// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_COMPONENTS_CAMERACOMPONENT_HPP
#define NDK_COMPONENTS_CAMERACOMPONENT_HPP

#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Utility/Node.hpp>
#include <NDK/Component.hpp>

namespace Ndk
{
	class Entity;

	class NDK_API CameraComponent : public Component<CameraComponent>, NzNode::Listener, NzRenderTarget::Listener
	{
		public:
			CameraComponent();
			~CameraComponent();

			void EnsureFrustumUpdate() const;
			void EnsureProjectionMatrixUpdate() const;
			void EnsureViewMatrixUpdate() const;
			void EnsureViewportUpdate() const;

			float GetAspectRatio() const;
			NzVector3f GetEyePosition() const;
			NzVector3f GetForward() const;
			float GetFOV() const;
			const NzFrustumf& GetFrustum() const;
			unsigned int GetLayer() const;
			const NzMatrix4f& GetProjectionMatrix() const;
			const NzRenderTarget* GetTarget() const;
			const NzRectf& GetTargetRegion() const;
			const NzMatrix4f& GetViewMatrix() const;
			const NzRecti& GetViewport() const;
			float GetZFar() const;
			float GetZNear() const;

			void SetFOV(float fov);
			void SetLayer(unsigned int layer);
			void SetTarget(const NzRenderTarget* renderTarget);
			void SetTargetRegion(const NzRectf& region);
			void SetViewport(const NzRecti& viewport);
			void SetZFar(float zFar);
			void SetZNear(float zNear);

			static ComponentIndex componentIndex;

		private:
			void InvalidateFrustum() const;
			void InvalidateProjectionMatrix() const;
			void InvalidateViewMatrix() const;
			void InvalidateViewport() const;

			void OnAttached() override;
			void OnComponentAttached(BaseComponent& component) override;
			void OnComponentDetached(BaseComponent& component) override;
			void OnDetached() override;
			bool OnNodeInvalidated(const NzNode* node, void* userdata) override;
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
			unsigned int m_layer;
	};
}

#include <NDK/Components/CameraComponent.inl>

#endif // NDK_COMPONENTS_CAMERACOMPONENT_HPP
