// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FORWARDPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_FORWARDPIPELINEPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Renderer/UploadPool.hpp>

namespace Nz
{
	class AbstractViewer;
	class FrameGraph;
	class FramePipeline;
	class Light;
	class Material;

	class NAZARA_GRAPHICS_API ForwardPipelinePass : public FramePipelinePass
	{
		public:
			ForwardPipelinePass(FramePipeline& owner, AbstractViewer* viewer);
			ForwardPipelinePass(const ForwardPipelinePass&) = delete;
			ForwardPipelinePass(ForwardPipelinePass&&) = delete;
			~ForwardPipelinePass();

			inline void ForceInvalidation();

			void Prepare(RenderFrame& renderFrame, const Frustumf& frustum, const std::vector<FramePipelinePass::VisibleRenderable>& visibleRenderables, const std::vector<const Light*>& visibleLights, std::size_t visibilityHash);

			void RegisterMaterial(const Material& material);
			void RegisterToFrameGraph(FrameGraph& frameGraph, std::size_t colorBufferIndex, std::size_t depthBufferIndex, bool hasDepthPrepass);

			void UnregisterMaterial(const Material& material);

			ForwardPipelinePass& operator=(const ForwardPipelinePass&) = delete;
			ForwardPipelinePass& operator=(ForwardPipelinePass&&) = delete;

			static constexpr std::size_t MaxLightCountPerDraw = 3;

		private:
			struct MaterialPassEntry
			{
				std::size_t usedCount = 1;

				NazaraSlot(MaterialPass, OnMaterialPassPipelineInvalidated, onMaterialPipelineInvalidated);
				NazaraSlot(MaterialPass, OnMaterialPassShaderBindingInvalidated, onMaterialShaderBindingInvalidated);
			};

			using LightKey = std::array<const Light*, MaxLightCountPerDraw>;

			struct LightKeyHasher
			{
				inline std::size_t operator()(const LightKey& lightKey) const;
			};

			struct LightDataUbo
			{
				std::shared_ptr<RenderBuffer> renderBuffer;
				std::size_t offset = 0;
				UploadPool::Allocation* allocation = nullptr;
			};

			struct LightUboPool
			{
				std::vector<std::shared_ptr<RenderBuffer>> lightUboBuffers;
			};

			std::size_t m_forwardPassIndex;
			std::size_t m_lastVisibilityHash;
			std::shared_ptr<LightUboPool> m_lightUboPool;
			std::vector<std::unique_ptr<ElementRendererData>> m_elementRendererData;
			std::vector<std::unique_ptr<RenderElement>> m_renderElements;
			std::vector<ElementRenderer::RenderStates> m_renderStates;
			std::unordered_map<MaterialPass*, MaterialPassEntry> m_materialPasses;
			std::unordered_map<const RenderElement*, RenderBufferView> m_lightPerRenderElement;
			std::unordered_map<LightKey, RenderBufferView, LightKeyHasher> m_lightBufferPerLights;
			std::vector<LightDataUbo> m_lightDataBuffers;
			std::vector<const Light*> m_renderableLights;
			RenderQueue<RenderElement*> m_renderQueue;
			RenderQueueRegistry m_renderQueueRegistry;
			AbstractViewer* m_viewer;
			FramePipeline& m_pipeline;
			bool m_rebuildCommandBuffer;
			bool m_rebuildElements;
	};
}

#include <Nazara/Graphics/ForwardPipelinePass.inl>

#endif // NAZARA_GRAPHICS_FORWARDPIPELINEPASS_HPP
