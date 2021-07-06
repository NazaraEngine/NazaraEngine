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
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_GRAPHICS_API CameraComponent : public AbstractViewer
	{
		public:
			inline CameraComponent(const RenderTarget* renderTarget, ProjectionType projectionType = ProjectionType::Perspective);
			CameraComponent(const CameraComponent&) = default;
			CameraComponent(CameraComponent&&) = default;
			~CameraComponent() = default;

			const RenderTarget& GetRenderTarget() override;
			ViewerInstance& GetViewerInstance() override;
			const ViewerInstance& GetViewerInstance() const override;

			inline void UpdateTarget(const RenderTarget* framebuffer);
			inline void UpdateProjectionType(ProjectionType projectionType);

			CameraComponent& operator=(const CameraComponent&) = default;
			CameraComponent& operator=(CameraComponent&&) = default;

		private:
			inline void UpdateProjectionMatrix();

			const RenderTarget* m_renderTarget;
			ProjectionType m_projectionType;
			ViewerInstance m_viewerInstance;
	};
}

#include <Nazara/Graphics/Components/CameraComponent.inl>

#endif
