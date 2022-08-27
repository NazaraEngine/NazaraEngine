// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FORWARDFRAMEPIPELINE_HPP
#define NAZARA_GRAPHICS_FORWARDFRAMEPIPELINE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/DebugDrawPipelinePass.hpp>
#include <Nazara/Graphics/DepthPipelinePass.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/ForwardPipelinePass.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <Nazara/Utils/MemoryPool.hpp>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Nz
{
	class RenderFrame;
	class RenderTarget;

	class NAZARA_GRAPHICS_API ForwardFramePipeline : public FramePipeline
	{
		public:
			ForwardFramePipeline();
			ForwardFramePipeline(const ForwardFramePipeline&) = delete;
			ForwardFramePipeline(ForwardFramePipeline&&) = delete;
			~ForwardFramePipeline();

			void InvalidateSkeletalInstance(std::size_t skeletalInstanceIndex) override;
			void InvalidateViewer(std::size_t viewerIndex) override;
			void InvalidateWorldInstance(std::size_t renderableIndex) override;

			std::size_t RegisterLight(std::shared_ptr<Light> light, UInt32 renderMask) override;
			void RegisterMaterialPass(MaterialPass* materialPass) override;
			std::size_t RegisterRenderable(std::size_t worldInstanceIndex, std::size_t skeletonInstanceIndex, const InstancedRenderable* instancedRenderable, UInt32 renderMask, const Recti& scissorBox) override;
			std::size_t RegisterSkeleton(SkeletonInstancePtr skeletonInstance) override;
			std::size_t RegisterViewer(AbstractViewer* viewerInstance, Int32 renderOrder) override;
			std::size_t RegisterWorldInstance(WorldInstancePtr worldInstance) override;

			void Render(RenderFrame& renderFrame) override;

			void UnregisterLight(std::size_t lightIndex) override;
			void UnregisterMaterialPass(MaterialPass* material) override;
			void UnregisterRenderable(std::size_t renderableIndex) override;
			void UnregisterSkeleton(std::size_t skeletonIndex) override;
			void UnregisterViewer(std::size_t viewerIndex) override;
			void UnregisterWorldInstance(std::size_t worldInstance) override;

			void UpdateLightRenderMask(std::size_t lightIndex, UInt32 renderMask) override;
			void UpdateRenderableRenderMask(std::size_t renderableIndex, UInt32 renderMask) override;
			void UpdateRenderableScissorBox(std::size_t renderableIndex, const Recti& scissorBox) override;
			void UpdateViewerRenderMask(std::size_t viewerIndex, Int32 renderOrder) override;

			ForwardFramePipeline& operator=(const ForwardFramePipeline&) = delete;
			ForwardFramePipeline& operator=(ForwardFramePipeline&&) = delete;

		private:
			BakedFrameGraph BuildFrameGraph();

			struct ViewerData;

			struct LightData
			{
				std::shared_ptr<Light> light;
				UInt32 renderMask;

				NazaraSlot(Light, OnLightDataInvalided, onLightInvalidated);
			};

			struct MaterialPassData
			{
				std::size_t usedCount = 0;

				NazaraSlot(MaterialPass, OnMaterialPassInvalidated, onMaterialPassInvalided);
			};

			struct RenderableData
			{
				std::size_t skeletonInstanceIndex;
				std::size_t worldInstanceIndex;
				const InstancedRenderable* renderable;
				Recti scissorBox;
				UInt32 renderMask = 0;

				NazaraSlot(InstancedRenderable, OnElementInvalidated, onElementInvalidated);
				NazaraSlot(InstancedRenderable, OnMaterialInvalidated, onMaterialInvalidated);
			};

			struct RenderTargetData
			{
				std::size_t finalAttachment;
				std::vector<const ViewerData*> viewers;
				ShaderBindingPtr blitShaderBinding;
			};

			struct ViewerData
			{
				std::size_t forwardColorAttachment;
				std::size_t debugColorAttachment;
				std::size_t depthStencilAttachment;
				std::unique_ptr<DepthPipelinePass> depthPrepass;
				std::unique_ptr<ForwardPipelinePass> forwardPass;
				std::unique_ptr<DebugDrawPipelinePass> debugDrawPass;
				AbstractViewer* viewer;
				Int32 renderOrder = 0;
				RenderQueueRegistry forwardRegistry;
				RenderQueue<RenderElement*> forwardRenderQueue;
				ShaderBindingPtr blitShaderBinding;
			};

			std::unordered_map<MaterialPass*, MaterialPassData> m_activeMaterialPasses;
			std::unordered_map<const RenderTarget*, RenderTargetData> m_renderTargets;
			std::unordered_set<MaterialPass*> m_invalidatedMaterialPasses;
			std::vector<ElementRenderer::RenderStates> m_renderStates;
			std::vector<FramePipelinePass::VisibleRenderable> m_visibleRenderables;
			std::vector<const Light*> m_visibleLights;
			BakedFrameGraph m_bakedFrameGraph;
			Bitset<UInt64> m_invalidatedSkeletonInstances;
			Bitset<UInt64> m_invalidatedViewerInstances;
			Bitset<UInt64> m_invalidatedWorldInstances;
			Bitset<UInt64> m_removedSkeletonInstances;
			Bitset<UInt64> m_removedViewerInstances;
			Bitset<UInt64> m_removedWorldInstances;
			MemoryPool<RenderableData> m_renderablePool;
			MemoryPool<LightData> m_lightPool;
			MemoryPool<SkeletonInstancePtr> m_skeletonInstances;
			MemoryPool<ViewerData> m_viewerPool;
			MemoryPool<WorldInstancePtr> m_worldInstances;
			RenderFrame* m_currentRenderFrame;
			bool m_rebuildFrameGraph;
	};
}

#include <Nazara/Graphics/ForwardFramePipeline.inl>

#endif // NAZARA_GRAPHICS_FORWARDFRAMEPIPELINE_HPP
