// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FORWARDFRAMEPIPELINE_HPP
#define NAZARA_GRAPHICS_FORWARDFRAMEPIPELINE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/DebugDrawPipelinePass.hpp>
#include <Nazara/Graphics/DepthPipelinePass.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/ForwardPipelinePass.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/LightShadowData.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/PostProcessPipelinePass.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Graphics/TransferInterface.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <NazaraUtils/MemoryPool.hpp>
#include <memory>
#include <optional>
#include <span>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Nz
{
	class LightShadowData;
	class RenderFrame;
	class RenderTarget;

	class NAZARA_GRAPHICS_API ForwardFramePipeline : public FramePipeline
	{
		public:
			ForwardFramePipeline(ElementRendererRegistry& elementRegistry);
			ForwardFramePipeline(const ForwardFramePipeline&) = delete;
			ForwardFramePipeline(ForwardFramePipeline&&) = delete;
			~ForwardFramePipeline();

			const std::vector<FramePipelinePass::VisibleRenderable>& FrustumCull(const Frustumf& frustum, UInt32 mask, std::size_t& visibilityHash) const override;

			void ForEachRegisteredMaterialInstance(FunctionRef<void(const MaterialInstance& materialInstance)> callback) override;

			void QueueTransfer(TransferInterface* transfer) override;

			std::size_t RegisterLight(const Light* light, UInt32 renderMask) override;
			std::size_t RegisterRenderable(std::size_t worldInstanceIndex, std::size_t skeletonInstanceIndex, const InstancedRenderable* instancedRenderable, UInt32 renderMask, const Recti& scissorBox) override;
			std::size_t RegisterSkeleton(SkeletonInstancePtr skeletonInstance) override;
			std::size_t RegisterViewer(PipelineViewer* viewerInstance, Int32 renderOrder) override;
			std::size_t RegisterWorldInstance(WorldInstancePtr worldInstance) override;

			const Light* RetrieveLight(std::size_t lightIndex) const override;
			const LightShadowData* RetrieveLightShadowData(std::size_t lightIndex) const override;
			const Texture* RetrieveLightShadowmap(std::size_t lightIndex, const AbstractViewer* viewer) const override;

			void Render(RenderResources& renderResources) override;

			void UnregisterLight(std::size_t lightIndex) override;
			void UnregisterRenderable(std::size_t renderableIndex) override;
			void UnregisterSkeleton(std::size_t skeletonIndex) override;
			void UnregisterViewer(std::size_t viewerIndex) override;
			void UnregisterWorldInstance(std::size_t worldInstance) override;

			void UpdateLightRenderMask(std::size_t lightIndex, UInt32 renderMask) override;
			void UpdateRenderableRenderMask(std::size_t renderableIndex, UInt32 renderMask) override;
			void UpdateRenderableScissorBox(std::size_t renderableIndex, const Recti& scissorBox) override;
			void UpdateRenderableSkeletonInstance(std::size_t renderableIndex, std::size_t skeletonIndex) override;
			void UpdateViewerRenderOrder(std::size_t viewerIndex, Int32 renderOrder) override;

			ForwardFramePipeline& operator=(const ForwardFramePipeline&) = delete;
			ForwardFramePipeline& operator=(ForwardFramePipeline&&) = delete;

		private:
			struct ViewerData;

			BakedFrameGraph BuildFrameGraph();

			void RegisterMaterialInstance(MaterialInstance* materialPass);
			void UnregisterMaterialInstance(MaterialInstance* material);

			static std::size_t BuildMergePass(FrameGraph& frameGraph, std::span<ViewerData*> targetViewers);

			struct LightData
			{
				std::unique_ptr<LightShadowData> shadowData;
				const Light* light;
				UInt32 renderMask;

				NazaraSlot(Light, OnLightDataInvalided, onLightInvalidated);
				NazaraSlot(Light, OnLightShadowCastingChanged, onLightShadowCastingChanged);
			};

			struct MaterialInstanceData
			{
				std::size_t usedCount = 0;

				NazaraSlot(TransferInterface, OnTransferRequired, onTransferRequired);
			};

			struct RenderableData
			{
				std::size_t skeletonInstanceIndex;
				std::size_t worldInstanceIndex;
				const InstancedRenderable* renderable;
				Recti scissorBox;
				UInt32 renderMask = 0;
				UInt8 generation;

				NazaraSlot(InstancedRenderable, OnElementInvalidated, onElementInvalidated);
				NazaraSlot(InstancedRenderable, OnMaterialInvalidated, onMaterialInvalidated);
			};

			struct RenderTargetData
			{
				std::vector<const ViewerData*> viewers;
			};

			struct SkeletonInstanceData
			{
				SkeletonInstancePtr skeleton;

				NazaraSlot(TransferInterface, OnTransferRequired, onTransferRequired);
			};

			struct ViewerData
			{
				struct FrameData
				{
					Bitset<UInt64> visibleLights;
					Frustumf frustum;
				};

				std::size_t finalColorAttachment;
				std::vector<std::unique_ptr<FramePipelinePass>> passes;
				FrameData frame;
				PipelineViewer* viewer;
				Int32 renderOrder = 0;
				RenderQueueRegistry forwardRegistry;
				RenderQueue<RenderElement*> forwardRenderQueue;
				ShaderBindingPtr blitShaderBinding;
				UInt32 renderMask;
				bool pendingDestruction = false;

				NazaraSlot(AbstractViewer, OnRenderMaskUpdated, onRenderMaskUpdated);
				NazaraSlot(TransferInterface, OnTransferRequired, onTransferRequired);
			};

			struct WorldInstanceData
			{
				WorldInstancePtr worldInstance;

				NazaraSlot(TransferInterface, OnTransferRequired, onTransferRequired);
			};

			std::unordered_map<const RenderTarget*, RenderTargetData> m_renderTargets;
			std::unordered_map<MaterialInstance*, MaterialInstanceData> m_materialInstances;
			mutable std::vector<FramePipelinePass::VisibleRenderable> m_visibleRenderables;
			std::vector<ViewerData*> m_orderedViewers;
			robin_hood::unordered_set<TransferInterface*> m_transferSet;
			BakedFrameGraph m_bakedFrameGraph;
			Bitset<UInt64> m_activeLights;
			Bitset<UInt64> m_removedLightInstances;
			Bitset<UInt64> m_removedSkeletonInstances;
			Bitset<UInt64> m_removedViewerInstances;
			Bitset<UInt64> m_removedWorldInstances;
			Bitset<UInt64> m_shadowCastingLights;
			Bitset<UInt64> m_visibleShadowCastingLights;
			ElementRendererRegistry& m_elementRegistry;
			MemoryPool<RenderableData> m_renderablePool;
			MemoryPool<LightData> m_lightPool;
			MemoryPool<SkeletonInstanceData> m_skeletonInstances;
			MemoryPool<ViewerData> m_viewerPool;
			MemoryPool<WorldInstanceData> m_worldInstances;
			UInt8 m_generationCounter;
			bool m_rebuildFrameGraph;
	};
}

#include <Nazara/Graphics/ForwardFramePipeline.inl>

#endif // NAZARA_GRAPHICS_FORWARDFRAMEPIPELINE_HPP
