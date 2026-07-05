// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SHADOWATLASPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_SHADOWATLASPIPELINEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/BaseElementRenderPipelinePass.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderElementOwner.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Graphics/ShadowAtlas.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <NazaraUtils/FixedVector.hpp>

namespace Nz
{
	class AbstractViewer;
	class ElementRendererRegistry;
	class FrameGraph;
	class FramePass;
	class FramePipeline;

	class NAZARA_GRAPHICS_API ShadowAtlasPipelinePass : public BaseElementRenderPipelinePass
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
			struct LightData
			{
				// FIXME: HybridVector doesn't seem to be movable when containing move-only types
				std::vector<std::vector<std::unique_ptr<ElementRendererData>>> elementRendererData;
			};

			std::unordered_map<std::size_t, LightData> m_lightData;
			ShadowAtlas m_shadowAtlas;
			FramePipeline& m_pipeline;
	};
}

#include <Nazara/Graphics/ShadowAtlasPipelinePass.inl>

#endif // NAZARA_GRAPHICS_SHADOWATLASPIPELINEPASS_HPP
