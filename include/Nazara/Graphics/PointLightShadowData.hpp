// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_POINTLIGHTSHADOWDATA_HPP
#define NAZARA_GRAPHICS_POINTLIGHTSHADOWDATA_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/DepthPipelinePass.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/LightShadowData.hpp>
#include <Nazara/Graphics/ShadowViewer.hpp>
#include <array>

namespace Nz
{
	class FramePipeline;
	class PointLight;

	class NAZARA_GRAPHICS_API PointLightShadowData : public LightShadowData
	{
		public:
			PointLightShadowData(FramePipeline& pipeline, ElementRendererRegistry& elementRegistry, const PointLight& light);
			PointLightShadowData(const PointLightShadowData&) = delete;
			PointLightShadowData(PointLightShadowData&&) = delete;
			~PointLightShadowData() = default;

			void PrepareRendering(RenderFrame& renderFrame, const AbstractViewer* viewer) override;

			void RegisterMaterialInstance(const MaterialInstance& matInstance) override;
			void RegisterPassInputs(FramePass& pass, const AbstractViewer* viewer) override;
			void RegisterToFrameGraph(FrameGraph& frameGraph, const AbstractViewer* viewer) override;

			const Texture* RetrieveLightShadowmap(const BakedFrameGraph& bakedGraph, const AbstractViewer* viewer) const override;

			void UnregisterMaterialInstance(const MaterialInstance& matInstance) override;

			PointLightShadowData& operator=(const PointLightShadowData&) = delete;
			PointLightShadowData& operator=(PointLightShadowData&&) = delete;

		private:
			NazaraSlot(Light, OnLightShadowMapSettingChange, m_onLightShadowMapSettingChange);
			NazaraSlot(Light, OnLightTransformInvalided, m_onLightTransformInvalidated);

			struct DirectionData
			{
				std::optional<DepthPipelinePass> depthPass;
				std::size_t attachmentIndex;
				ShadowViewer viewer;
			};

			std::array<DirectionData, 6> m_directions;
			std::size_t m_cubeAttachmentIndex;
			FramePipeline& m_pipeline;
			const PointLight& m_light;
	};
}

#include <Nazara/Graphics/PointLightShadowData.inl>

#endif // NAZARA_GRAPHICS_POINTLIGHTSHADOWDATA_HPP
