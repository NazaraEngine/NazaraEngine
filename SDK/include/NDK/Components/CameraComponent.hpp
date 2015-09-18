// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_COMPONENTS_CAMERACOMPONENT_HPP
#define NDK_COMPONENTS_CAMERACOMPONENT_HPP

#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Utility/Node.hpp>
#include <NDK/Component.hpp>

namespace Ndk
{
	class Entity;

	class NDK_API CameraComponent : public Component<CameraComponent>, public NzAbstractViewer
	{
		public:
			inline CameraComponent();
			inline CameraComponent(const CameraComponent& camera);
			~CameraComponent() = default;

			void ApplyView() const override;

			inline void EnsureFrustumUpdate() const;
			inline void EnsureProjectionMatrixUpdate() const;
			inline void EnsureViewMatrixUpdate() const;
			inline void EnsureViewportUpdate() const;

			inline float GetAspectRatio() const;
			inline NzVector3f GetEyePosition() const;
			inline NzVector3f GetForward() const;
			inline float GetFOV() const;
			inline const NzFrustumf& GetFrustum() const;
			inline unsigned int GetLayer() const;
			inline const NzMatrix4f& GetProjectionMatrix() const;
			inline nzProjectionType GetProjectionType() const;
			inline const NzRenderTarget* GetTarget() const;
			inline const NzRectf& GetTargetRegion() const;
			inline const NzMatrix4f& GetViewMatrix() const;
			inline const NzRecti& GetViewport() const;
			inline float GetZFar() const;
			inline float GetZNear() const;

			inline void SetFOV(float fov);
			inline void SetLayer(unsigned int layer);
			inline void SetProjectionType(nzProjectionType projection);
			inline void SetTarget(const NzRenderTarget* renderTarget);
			inline void SetTargetRegion(const NzRectf& region);
			inline void SetViewport(const NzRecti& viewport);
			inline void SetZFar(float zFar);
			inline void SetZNear(float zNear);

			static ComponentIndex componentIndex;

		private:
			inline void InvalidateFrustum() const;
			inline void InvalidateProjectionMatrix() const;
			inline void InvalidateViewMatrix() const;
			inline void InvalidateViewport() const;

			void OnAttached() override;
			void OnComponentAttached(BaseComponent& component) override;
			void OnComponentDetached(BaseComponent& component) override;
			void OnDetached() override;
			void OnNodeInvalidated(const NzNode* node);
			void OnRenderTargetRelease(const NzRenderTarget* renderTarget);
			void OnRenderTargetSizeChange(const NzRenderTarget* renderTarget);

			void UpdateFrustum() const;
			void UpdateProjectionMatrix() const;
			void UpdateViewMatrix() const;
			void UpdateViewport() const;

			NazaraSlot(NzNode, OnNodeInvalidation, m_nodeInvalidationSlot);
			NazaraSlot(NzRenderTarget, OnRenderTargetRelease, m_targetReleaseSlot);
			NazaraSlot(NzRenderTarget, OnRenderTargetSizeChange, m_targetResizeSlot);

			nzProjectionType m_projectionType;
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
