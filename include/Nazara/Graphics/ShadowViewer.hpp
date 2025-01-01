// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SHADOWVIEWER_HPP
#define NAZARA_GRAPHICS_SHADOWVIEWER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API ShadowViewer : public AbstractViewer
	{
		public:
			ShadowViewer() = default;
			ShadowViewer(const ShadowViewer&) = delete;
			ShadowViewer(ShadowViewer&&) = delete;
			~ShadowViewer() = default;

			const Color& GetClearColor() const override;
			float GetClearDepth() const override;
			DebugDrawer* GetDebugDrawer() override;
			UInt32 GetRenderMask() const override;
			const RenderTarget& GetRenderTarget() const override;
			ViewerInstance& GetViewerInstance() override;
			const ViewerInstance& GetViewerInstance() const override;
			const Recti& GetViewport() const override;

			bool IsZReversed() const override;

			inline void UpdateRenderMask(UInt32 renderMask);
			inline void UpdateViewport(const Recti& viewport);

			ShadowViewer& operator=(const ShadowViewer&) = delete;
			ShadowViewer& operator=(ShadowViewer&&) = delete;

		private:
			Recti m_viewport;
			ViewerInstance m_viewerInstance;
			UInt32 m_renderMask;
	};
}

#include <Nazara/Graphics/ShadowViewer.inl>

#endif // NAZARA_GRAPHICS_SHADOWVIEWER_HPP
