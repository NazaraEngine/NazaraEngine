// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SPOTLIGHTSHADOWDATA_HPP
#define NAZARA_GRAPHICS_SPOTLIGHTSHADOWDATA_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/LightShadowData.hpp>
#include <Nazara/Graphics/RasterPipelinePass.hpp>
#include <Nazara/Graphics/ShadowViewer.hpp>

namespace Nz
{
	class FramePipeline;
	class SpotLight;

	class NAZARA_GRAPHICS_API SpotLightShadowData : public LightShadowData
	{
		public:
			SpotLightShadowData(FramePipeline& pipeline, ElementRendererRegistry& elementRegistry, const SpotLight& light);
			SpotLightShadowData(const SpotLightShadowData&) = delete;
			SpotLightShadowData(SpotLightShadowData&&) = delete;
			~SpotLightShadowData() = default;

			inline const ViewerInstance& GetViewerInstance() const;

			void PrepareRendering(RenderResources& renderResources, [[maybe_unused]] const AbstractViewer* viewer) override;

			void RegisterMaterialInstance(const MaterialInstance& matInstance) override;
			void RegisterPassInputs(FramePass& pass, const AbstractViewer* viewer) override;
			void RegisterToFrameGraph(FrameGraph& frameGraph, const AbstractViewer* viewer) override;

			const Texture* RetrieveLightShadowmap(const BakedFrameGraph& bakedGraph, const AbstractViewer* viewer) const override;

			void UnregisterMaterialInstance(const MaterialInstance& matInstance) override;

			SpotLightShadowData& operator=(const SpotLightShadowData&) = delete;
			SpotLightShadowData& operator=(SpotLightShadowData&&) = delete;

		private:
			NazaraSlot(Light, OnLightDataInvalidated, m_onLightDataInvalidated);
			NazaraSlot(Light, OnLightShadowMapSettingChange, m_onLightShadowMapSettingChange);
			NazaraSlot(Light, OnLightTransformInvalidated, m_onLightTransformInvalidated);

			std::optional<RasterPipelinePass> m_depthPass;
			std::size_t m_attachmentIndex;
			FramePipeline& m_pipeline;
			const SpotLight& m_light;
			ShadowViewer m_viewer;
	};
}

#include <Nazara/Graphics/SpotLightShadowData.inl>

#endif // NAZARA_GRAPHICS_SPOTLIGHTSHADOWDATA_HPP
