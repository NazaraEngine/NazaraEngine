// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SHADOWATLASPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_SHADOWATLASPIPELINEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderElementOwner.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Graphics/ShadowAtlas.hpp>
#include <Nazara/Math/Frustum.hpp>

namespace Nz
{
	class AbstractViewer;
	class ElementRendererRegistry;
	class FrameGraph;
	class FramePass;
	class FramePipeline;

	class NAZARA_GRAPHICS_API ShadowAtlasPipelinePass : public FramePipelinePass
	{
		public:
			ShadowAtlasPipelinePass(PassData& passData);
			ShadowAtlasPipelinePass(const ShadowAtlasPipelinePass&) = delete;
			ShadowAtlasPipelinePass(ShadowAtlasPipelinePass&&) = delete;
			~ShadowAtlasPipelinePass() = default;

			inline ShadowAtlas& GetAtlas();
			inline const ShadowAtlas& GetAtlas() const;

			void InvalidateElements() override;

			void Prepare(FrameData& frameData) override;

			void RegisterMaterialInstance(const MaterialInstance& materialInstance) override;

			FramePass& RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs) override;

			void UnregisterMaterialInstance(const MaterialInstance& materialInstance) override;

			ShadowAtlasPipelinePass& operator=(const ShadowAtlasPipelinePass&) = delete;
			ShadowAtlasPipelinePass& operator=(ShadowAtlasPipelinePass&&) = delete;

		private:
			struct MaterialPassEntry
			{
				std::size_t usedCount = 1;

				NazaraSlot(MaterialInstance, OnMaterialInstancePipelineInvalidated, onMaterialInstancePipelineInvalidated);
				NazaraSlot(MaterialInstance, OnMaterialInstanceShaderBindingInvalidated, onMaterialInstanceShaderBindingInvalidated);
			};

			struct LightData
			{
				HybridVector<std::unique_ptr<ElementRendererData>, BasicRenderElementCount> elementRendererData;
			};

			std::size_t m_passIndex;
			std::size_t m_lastVisibilityHash;
			std::vector<RenderElementOwner> m_renderElements;
			std::unordered_map<std::size_t, LightData> m_lightData;
			std::unordered_map<const MaterialInstance*, MaterialPassEntry> m_materialInstances;
			RenderQueue<const RenderElement*> m_renderQueue;
			RenderQueueRegistry m_renderQueueRegistry;
			ShadowAtlas m_shadowAtlas;
			ElementRendererRegistry& m_elementRegistry;
			FramePipeline& m_pipeline;
			bool m_rebuildElements;
	};
}

#include <Nazara/Graphics/ShadowAtlasPipelinePass.inl>

#endif // NAZARA_GRAPHICS_SHADOWATLASPIPELINEPASS_HPP
