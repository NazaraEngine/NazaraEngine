// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CAMERACOMPONENT_HPP
#define NAZARA_CAMERACOMPONENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_GRAPHICS_API CameraComponent : public AbstractViewer
	{
		public:
			inline CameraComponent(const RenderTarget* renderTarget, ProjectionType projectionType = ProjectionType::Perspective);
			inline CameraComponent(const CameraComponent& camera);
			inline CameraComponent(CameraComponent&& camera) noexcept;
			~CameraComponent() = default;

			inline float GetAspectRatio() const;
			inline DegreeAnglef GetFOV() const;
			const RenderTarget& GetRenderTarget() override;
			inline const Vector2f& GetSize() const;
			inline const Rectf& GetTargetRegion() const;
			ViewerInstance& GetViewerInstance() override;
			const ViewerInstance& GetViewerInstance() const override;
			const Recti& GetViewport() const override;
			inline float GetZFar() const;
			inline float GetZNear() const;

			inline void UpdateFOV(DegreeAnglef fov);
			inline void UpdateProjectionType(ProjectionType projectionType);
			inline void UpdateSize(const Vector2f& size);
			void UpdateTarget(const RenderTarget* framebuffer);
			inline void UpdateTargetRegion(const Rectf& targetRegion);
			inline void UpdateViewport(const Recti& viewport);
			inline void UpdateZFar(float zFar);
			inline void UpdateZNear(float zNear);

			inline CameraComponent& operator=(const CameraComponent& camera);
			inline CameraComponent& operator=(CameraComponent&& camera) noexcept;

		private:
			inline void UpdateProjectionMatrix();
			inline void UpdateViewport();
			inline void UpdateViewport(Vector2ui renderTargetSize);

			NazaraSlot(RenderTarget, OnRenderTargetRelease, m_onRenderTargetRelease);
			NazaraSlot(RenderTarget, OnRenderTargetSizeChange, m_onRenderTargetSizeChange);

			const RenderTarget* m_renderTarget;
			DegreeAnglef m_fov;
			ProjectionType m_projectionType;
			Rectf m_targetRegion;
			Recti m_viewport;
			Vector2f m_size;
			ViewerInstance m_viewerInstance;
			float m_aspectRatio;
			float m_zFar;
			float m_zNear;
	};
}

#include <Nazara/Graphics/Components/CameraComponent.inl>

#endif
