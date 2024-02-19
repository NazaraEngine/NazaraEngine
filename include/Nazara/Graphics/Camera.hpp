// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_CAMERA_HPP
#define NAZARA_GRAPHICS_CAMERA_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/PipelineViewer.hpp>
#include <Nazara/Graphics/RenderTarget.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>

namespace Nz
{
	class PipelinePassList;

	class NAZARA_GRAPHICS_API Camera : public PipelineViewer
	{
		public:
			inline Camera(std::shared_ptr<const RenderTarget> renderTarget, std::shared_ptr<PipelinePassList> pipelinePasses, ProjectionType projectionType = ProjectionType::Perspective);
			Camera(std::shared_ptr<const RenderTarget> renderTarget, ProjectionType projectionType = ProjectionType::Perspective);
			inline Camera(const Camera& camera);
			inline Camera(Camera&& camera) noexcept;
			~Camera() = default;

			std::vector<std::unique_ptr<FramePipelinePass>> BuildPasses(FramePipelinePass::PassData& passData) const override;

			inline float GetAspectRatio() const;
			const Color& GetClearColor() const override;
			inline DegreeAnglef GetFOV() const;
			inline ProjectionType GetProjectionType() const;
			UInt32 GetRenderMask() const override;
			inline Int32 GetRenderOrder() const;
			const RenderTarget& GetRenderTarget() const override;
			inline const Vector2f& GetSize() const;
			inline const Rectf& GetTargetRegion() const;
			ViewerInstance& GetViewerInstance() override;
			const ViewerInstance& GetViewerInstance() const override;
			const Recti& GetViewport() const override;
			inline float GetZFar() const;
			inline float GetZNear() const;

			std::size_t RegisterPasses(const std::vector<std::unique_ptr<FramePipelinePass>>& passes, FrameGraph& frameGraph, std::optional<unsigned int> viewerIndex, const FunctionRef<void(std::size_t passIndex, FramePass& framePass, FramePipelinePassFlags flags)>& passCallback = nullptr) const override;

			inline void UpdateClearColor(Color color);
			inline void UpdateFOV(DegreeAnglef fov);
			inline void UpdateProjectionType(ProjectionType projectionType);
			inline void UpdateRenderMask(UInt32 renderMask);
			inline void UpdateRenderOrder(Int32 renderOrder);
			inline void UpdateSize(const Vector2f& size);
			void UpdateTarget(std::shared_ptr<const RenderTarget> renderTarget);
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

			std::shared_ptr<PipelinePassList> m_framePipelinePasses;
			std::shared_ptr<const RenderTarget> m_renderTarget;
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
