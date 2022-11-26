// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_CAMERA_HPP
#define NAZARA_GRAPHICS_CAMERA_HPP

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
	class NAZARA_GRAPHICS_API Camera : public AbstractViewer
	{
		public:
			inline Camera(const RenderTarget* renderTarget, ProjectionType projectionType = ProjectionType::Perspective);
			inline Camera(const Camera& camera);
			inline Camera(Camera&& camera) noexcept;
			~Camera() = default;

			inline float GetAspectRatio() const;
			const Color& GetClearColor() const override;
			inline DegreeAnglef GetFOV() const;
			UInt32 GetRenderMask() const override;
			inline Int32 GetRenderOrder() const;
			const RenderTarget& GetRenderTarget() const;
			inline const Vector2f& GetSize() const;
			inline const Rectf& GetTargetRegion() const;
			ViewerInstance& GetViewerInstance() override;
			const ViewerInstance& GetViewerInstance() const override;
			const Recti& GetViewport() const override;
			inline float GetZFar() const;
			inline float GetZNear() const;

			inline void UpdateClearColor(Color color);
			inline void UpdateFOV(DegreeAnglef fov);
			inline void UpdateProjectionType(ProjectionType projectionType);
			inline void UpdateRenderMask(UInt32 renderMask);
			inline void UpdateRenderOrder(Int32 renderOrder);
			inline void UpdateSize(const Vector2f& size);
			void UpdateTarget(const RenderTarget* framebuffer);
			inline void UpdateTargetRegion(const Rectf& targetRegion);
			inline void UpdateViewport(const Recti& viewport);
			inline void UpdateZFar(float zFar);
			inline void UpdateZNear(float zNear);

			inline Camera& operator=(const Camera& camera);
			inline Camera& operator=(Camera&& camera) noexcept;

		private:
			inline void UpdateProjectionMatrix();
			inline void UpdateViewport();
			inline void UpdateViewport(Vector2ui renderTargetSize);

			NazaraSlot(RenderTarget, OnRenderTargetRelease, m_onRenderTargetRelease);
			NazaraSlot(RenderTarget, OnRenderTargetSizeChange, m_onRenderTargetSizeChange);

			const RenderTarget* m_renderTarget;
			Color m_clearColor;
			DegreeAnglef m_fov;
			Int32 m_renderOrder;
			ProjectionType m_projectionType;
			Rectf m_targetRegion;
			Recti m_viewport;
			Vector2f m_size;
			ViewerInstance m_viewerInstance;
			UInt32 m_renderMask;
			float m_aspectRatio;
			float m_zFar;
			float m_zNear;
	};
}

#include <Nazara/Graphics/Camera.inl>

#endif // NAZARA_GRAPHICS_CAMERA_HPP
