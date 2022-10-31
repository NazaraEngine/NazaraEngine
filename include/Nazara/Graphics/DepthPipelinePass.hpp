// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_DEPTHPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_DEPTHPIPELINEPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderElementOwner.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Math/Frustum.hpp>

namespace Nz
{
	class AbstractViewer;
	class ElementRendererRegistry;
	class FrameGraph;
	class FramePipeline;

	class NAZARA_GRAPHICS_API DepthPipelinePass : public FramePipelinePass
	{
		public:
			DepthPipelinePass(FramePipeline& owner, ElementRendererRegistry& elementRegistry, AbstractViewer* viewer);
			DepthPipelinePass(const DepthPipelinePass&) = delete;
			DepthPipelinePass(DepthPipelinePass&&) = delete;
			~DepthPipelinePass() = default;

			inline void InvalidateCommandBuffers();
			inline void InvalidateElements();

			void Prepare(RenderFrame& renderFrame, const Frustumf& frustum, const std::vector<FramePipelinePass::VisibleRenderable>& visibleRenderables, std::size_t visibilityHash);

			void RegisterMaterialInstance(const MaterialInstance& materialInstance);
			void RegisterToFrameGraph(FrameGraph& frameGraph, std::size_t depthBufferIndex);

			void UnregisterMaterialInstance(const MaterialInstance& materialInstance);

			DepthPipelinePass& operator=(const DepthPipelinePass&) = delete;
			DepthPipelinePass& operator=(DepthPipelinePass&&) = delete;

		private:
			struct MaterialPassEntry
			{
				std::size_t usedCount = 1;

				NazaraSlot(MaterialInstance, OnMaterialInstancePipelineInvalidated, onMaterialInstancePipelineInvalidated);
				NazaraSlot(MaterialInstance, OnMaterialInstanceShaderBindingInvalidated, onMaterialInstanceShaderBindingInvalidated);
			};

			std::size_t m_depthPassIndex;
			std::size_t m_lastVisibilityHash;
			std::vector<std::unique_ptr<ElementRendererData>> m_elementRendererData;
			std::vector<ElementRenderer::RenderStates> m_renderStates;
			std::vector<RenderElementOwner> m_renderElements;
			std::unordered_map<const MaterialInstance*, MaterialPassEntry> m_materialInstances;
			RenderQueue<const RenderElement*> m_renderQueue;
			RenderQueueRegistry m_renderQueueRegistry;
			AbstractViewer* m_viewer;
			ElementRendererRegistry& m_elementRegistry;
			FramePipeline& m_pipeline;
			bool m_rebuildCommandBuffer;
			bool m_rebuildElements;
	};
}

#include <Nazara/Graphics/DepthPipelinePass.inl>

#endif // NAZARA_GRAPHICS_DEPTHPIPELINEPASS_HPP
