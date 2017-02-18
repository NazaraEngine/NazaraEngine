// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SERVER
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
	class CameraComponent;
	class Entity;

	using CameraComponentHandle = Nz::ObjectHandle<CameraComponent>;

	class NDK_API CameraComponent : public Component<CameraComponent>, public Nz::AbstractViewer, public Nz::HandledObject<CameraComponent>
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

			float GetAspectRatio() const override;
			Nz::Vector3f GetEyePosition() const override;
			Nz::Vector3f GetForward() const override;
			inline float GetFOV() const;
			const Nz::Frustumf& GetFrustum() const override;
			inline unsigned int GetLayer() const;
			const Nz::Matrix4f& GetProjectionMatrix() const override;
			inline Nz::ProjectionType GetProjectionType() const;
			inline const Nz::Vector2f& GetSize() const;
			const Nz::RenderTarget* GetTarget() const override;
			inline const Nz::Rectf& GetTargetRegion() const;
			const Nz::Matrix4f& GetViewMatrix() const override;
			const Nz::Recti& GetViewport() const override;
			float GetZFar() const override;
			float GetZNear() const override;

			inline void SetFOV(float fov);
			void SetLayer(unsigned int layer);
			inline void SetProjectionType(Nz::ProjectionType projection);
			inline void SetSize(const Nz::Vector2f& size);
			inline void SetSize(float width, float height);
			inline void SetTarget(const Nz::RenderTarget* renderTarget);
			inline void SetTargetRegion(const Nz::Rectf& region);
			inline void SetViewport(const Nz::Recti& viewport);
			inline void SetZFar(float zFar);
			inline void SetZNear(float zNear);

			inline bool UpdateVisibility(std::size_t visibilityHash);

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
			void OnNodeInvalidated(const Nz::Node* node);
			void OnRenderTargetRelease(const Nz::RenderTarget* renderTarget);
			void OnRenderTargetSizeChange(const Nz::RenderTarget* renderTarget);

			void UpdateFrustum() const;
			void UpdateProjectionMatrix() const;
			void UpdateViewMatrix() const;
			void UpdateViewport() const;

			NazaraSlot(Nz::Node, OnNodeInvalidation, m_nodeInvalidationSlot);
			NazaraSlot(Nz::RenderTarget, OnRenderTargetRelease, m_targetReleaseSlot);
			NazaraSlot(Nz::RenderTarget, OnRenderTargetSizeChange, m_targetResizeSlot);

			std::size_t m_visibilityHash;
			Nz::ProjectionType m_projectionType;
			mutable Nz::Frustumf m_frustum;
			mutable Nz::Matrix4f m_projectionMatrix;
			mutable Nz::Matrix4f m_viewMatrix;
			Nz::Rectf m_targetRegion;
			mutable Nz::Recti m_viewport;
			const Nz::RenderTarget* m_target;
			Nz::Vector2f m_size;
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
#endif // NDK_SERVER
