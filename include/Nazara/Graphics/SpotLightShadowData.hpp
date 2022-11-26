// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SPOTLIGHTSHADOWDATA_HPP
#define NAZARA_GRAPHICS_SPOTLIGHTSHADOWDATA_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/DepthPipelinePass.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/LightShadowData.hpp>
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

			void PrepareRendering(RenderFrame& renderFrame) override;

			void RegisterMaterialInstance(const MaterialInstance& matInstance) override;
			void RegisterPassInputs(FramePass& pass) override;
			void RegisterToFrameGraph(FrameGraph& frameGraph) override;

			const Texture* RetrieveLightShadowmap(const BakedFrameGraph& bakedGraph) const override;

			void UnregisterMaterialInstance(const MaterialInstance& matInstance) override;

			SpotLightShadowData& operator=(const SpotLightShadowData&) = delete;
			SpotLightShadowData& operator=(SpotLightShadowData&&) = delete;

		private:
			NazaraSlot(Light, OnLightShadowMapSettingChange, m_onLightShadowMapSettingChange);
			NazaraSlot(Light, OnLightTransformInvalided, m_onLightTransformInvalidated);

			std::optional<DepthPipelinePass> m_depthPass;
			std::size_t m_attachmentIndex;
			FramePipeline& m_pipeline;
			const SpotLight& m_light;
			ShadowViewer m_viewer;
	};
}

#include <Nazara/Graphics/SpotLightShadowData.inl>

#endif // NAZARA_GRAPHICS_SPOTLIGHTSHADOWDATA_HPP
