// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SHADOWATLASPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_SHADOWATLASPIPELINEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <Nazara/Graphics/ShadowAtlas.hpp>

namespace Nz
{
	class ComputePipeline;
	class ElementRendererRegistry;
	class FrameGraph;
	class FramePass;
	class FramePipeline;
	class RenderPipelineLayout;
	class UberShader;

	class NAZARA_GRAPHICS_API ShadowAtlasPipelinePass : public FramePipelinePass
	{
		public:
			ShadowAtlasPipelinePass(PassData& passData);
			ShadowAtlasPipelinePass(const ShadowAtlasPipelinePass&) = delete;
			ShadowAtlasPipelinePass(ShadowAtlasPipelinePass&&) = delete;
			~ShadowAtlasPipelinePass() = default;

			inline ShadowAtlas& GetAtlas();
			inline const ShadowAtlas& GetAtlas() const;

			void Prepare(FrameData& frameData) override;

			FramePass& RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs) override;

			ShadowAtlasPipelinePass& operator=(const ShadowAtlasPipelinePass&) = delete;
			ShadowAtlasPipelinePass& operator=(ShadowAtlasPipelinePass&&) = delete;

		private:
			void BuildCullingPipeline();

			struct LightData
			{
				// FIXME: HybridVector doesn't seem to be movable when containing move-only types
				std::vector<std::vector<std::unique_ptr<ElementRendererData>>> elementRendererData;
			};

			std::shared_ptr<ComputePipeline> m_computePipeline;
			std::shared_ptr<RenderPipelineLayout> m_computePipelineLayout;
			std::shared_ptr<UberShader> m_frustumCullingShader;
			std::size_t m_passIndex;
			std::size_t m_renderQueueHash;
			std::unordered_map<std::size_t, LightData> m_lightData;
			ShadowAtlas m_shadowAtlas;
			ElementRendererRegistry& m_elementRegistry;
			FramePipeline& m_pipeline;
			UInt32 m_renderMask;
	};
}

#include <Nazara/Graphics/ShadowAtlasPipelinePass.inl>

#endif // NAZARA_GRAPHICS_SHADOWATLASPIPELINEPASS_HPP
