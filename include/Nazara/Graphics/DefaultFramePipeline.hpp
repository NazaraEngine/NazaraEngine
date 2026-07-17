// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_DEFAULTFRAMEPIPELINE_HPP
#define NAZARA_GRAPHICS_DEFAULTFRAMEPIPELINE_HPP

#include <Nazara/Graphics/DefaultFramePipeline.hpp>
#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/DebugDrawPipelinePass.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/GpuDynamicArray.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/LightShadowData.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/PostProcessPipelinePass.hpp>
#include <Nazara/Graphics/RasterPipelinePass.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Graphics/ShaderBindingCache.hpp>
#include <Nazara/Graphics/ShadowAtlasPipelinePass.hpp>
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
	class RenderQueue;
	class RenderTarget;
	class Texture;

	class NAZARA_GRAPHICS_API DefaultFramePipeline : public FramePipeline
	{
		public:
			DefaultFramePipeline(ElementRendererRegistry& elementRegistry);
			DefaultFramePipeline(const DefaultFramePipeline&) = delete;
			DefaultFramePipeline(DefaultFramePipeline&&) = delete;
			~DefaultFramePipeline();

			void DequeueTransfer(TransferInterface* transfer) override;

			void ForEachRegisteredMaterialInstance(FunctionRef<void(const MaterialInstance& materialInstance)> callback) override;
			void ForEachShadowCastingLight(FunctionRef<void(std::size_t lightIndex, const Light* light, LightShadowData* lightShadowData)> callback) override;

			const std::shared_ptr<RenderBuffer>& GetDirectionalLightBuffer() const override;
			const std::shared_ptr<RenderBuffer>& GetDirectionalShadowMappingBuffer() const override;
			const std::shared_ptr<RenderBuffer>& GetInstanceBuffer() const override;
			const std::shared_ptr<RenderBuffer>& GetPointLightBuffer() const override;
			const std::shared_ptr<RenderBuffer>& GetPointShadowMappingBuffer() const override;
			RenderQueue& GetRenderQueue(std::size_t renderQueueIndex) override;
			ShaderBindingCache* GetShaderBindingCache() const override;
			const std::shared_ptr<Texture>& GetShadowAtlasTexture() const override;
			const std::shared_ptr<RenderBuffer>& GetSpotLightBuffer() const override;
			const std::shared_ptr<RenderBuffer>& GetSpotShadowMappingBuffer() const override;

			void QueueTransfer(TransferInterface* transfer) override;

			UInt32 RegisterInstance() override;
			std::size_t RegisterLight(const Light* light, UInt32 renderMask) override;
			std::size_t RegisterRenderable(UInt32 instanceIndex, std::size_t skeletonInstanceIndex, const InstancedRenderable* instancedRenderable, UInt32 renderMask, const Recti& scissorBox) override;
			std::size_t RegisterSkeleton(SkeletonInstancePtr skeletonInstance) override;
			std::size_t RegisterViewer(PipelineViewer* viewerInstance, Int32 renderOrder) override;

			const Light* RetrieveLight(std::size_t lightIndex) const override;
			const LightShadowData* RetrieveLightShadowData(std::size_t lightIndex) const override;

			void Render(RenderResources& renderResources) override;

			void UnregisterInstance(UInt32 instanceIndex) override;
			void UnregisterLight(std::size_t lightIndex) override;
			void UnregisterRenderable(std::size_t renderableIndex) override;
			void UnregisterSkeleton(std::size_t skeletonIndex) override;
			void UnregisterViewer(std::size_t viewerIndex) override;

			using FramePipeline::UpdateInstanceData;
			void UpdateInstanceData(UInt32 instanceIndex, const Matrix4f& worldMatrix, const Matrix4f& invWorldMatrix) override;
			void UpdateLightRenderMask(std::size_t lightIndex, UInt32 renderMask) override;
			void UpdateRenderableRenderMask(std::size_t renderableIndex, UInt32 renderMask) override;
			void UpdateRenderableScissorBox(std::size_t renderableIndex, const Recti& scissorBox) override;
			void UpdateRenderableSkeletonInstance(std::size_t renderableIndex, std::size_t skeletonIndex) override;
			void UpdateViewerRenderOrder(std::size_t viewerIndex, Int32 renderOrder) override;

			DefaultFramePipeline& operator=(const DefaultFramePipeline&) = delete;
			DefaultFramePipeline& operator=(DefaultFramePipeline&&) = delete;

		protected:
			void BuildRenderQueues() override;

			void RegisterRenderQueue(std::string_view renderQueueName, std::string_view materialPass, RenderQueueFlags flags = {});

		private:
			struct LightData;
			struct RenderableData;
			struct ViewerData;

			void BroadcastRenderable(const RenderableData& renderableData);

			BakedFrameGraph BuildFrameGraph();

			std::size_t InsertTransferPass(FrameGraph& frameGraph, std::function<void()> callback);

			void ProcesRemovedData(RenderResources& renderResources);

			void RegisterMaterialInstance(MaterialInstance* materialPass, std::size_t renderableIndex);
			void RegisterShadowCaster(std::size_t lightIndex, LightData* lightData);

			void UnregisterMaterialInstance(MaterialInstance* material, std::size_t renderableIndex);
			void UnregisterShadowCaster(std::size_t lightIndex, LightData* lightData);

			static std::size_t BuildMergePass(FrameGraph& frameGraph, std::span<ViewerData*> targetViewers);

			static constexpr std::size_t InvalidElementIndex = MaxValue();
			static constexpr UInt32 InvalidShadowMappingEntry = MaxValue();

			struct LightData
			{
				std::unique_ptr<LightShadowData> shadowData;
				const Light* light;
				UInt32 entryIndex;
				UInt32 renderMask;
				UInt32 shadowMappingEntry;

				NazaraSlot(Light, OnLightDataInvalidated, onLightInvalidated);
				NazaraSlot(Light, OnLightShadowCastingChanged, onLightShadowCastingChanged);
				NazaraSlot(Light, OnLightShadowMapSettingChange, onLightShadowMapSettingChange);
			};

			struct MaterialInstanceData
			{
				std::size_t materialInstanceIndex;
				std::size_t usedCount = 0;
				std::unordered_map<std::size_t /*renderableIndex*/, std::size_t> renderableUsage;

				NazaraSlot(MaterialInstance, OnMaterialInstancePipelineInvalidated, onMaterialInstancePipelineInvalidated);
				NazaraSlot(TransferInterface, OnTransferRequired, onTransferRequired);
			};

			struct RenderableData
			{
				std::size_t firstElementIndex = InvalidElementIndex;
				std::size_t elementCount;
				std::size_t renderableIndex;
				std::size_t skeletonInstanceIndex;
				const InstancedRenderable* renderable;
				Recti scissorBox;
				UInt32 instanceIndex;
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
				ShaderBindingPtr blitShaderBinding;
				UInt32 renderMask;
				bool pendingDestruction = false;

				NazaraSlot(AbstractViewer, OnRenderMaskUpdated, onRenderMaskUpdated);
				NazaraSlot(TransferInterface, OnTransferRequired, onTransferRequired);
			};

			std::optional<ShadowAtlasPipelinePass> m_shadowAtlasPipelinePass;
			std::unordered_map<const RenderTarget*, RenderTargetData> m_renderTargets;
			std::unordered_map<MaterialInstance*, MaterialInstanceData*> m_materialInstances;
			std::vector<std::unique_ptr<ElementRendererData>> m_elementRendererData;
			std::vector<std::unique_ptr<RenderQueue>> m_renderQueues;
			std::vector<std::size_t> m_directionalLightEntriesToIndices;
			std::vector<std::size_t> m_directionalShadowEntriesToIndices;
			std::vector<std::size_t> m_pointLightEntriesToIndices;
			std::vector<std::size_t> m_pointShadowEntriesToIndices;
			std::vector<std::size_t> m_spotLightEntriesToIndices;
			std::vector<std::size_t> m_spotShadowEntriesToIndices;
			std::vector<ViewerData*> m_orderedViewers;
			ankerl::unordered_dense::set<TransferInterface*> m_transferSet;
			BakedFrameGraph m_bakedFrameGraph;
			Bitset<UInt64> m_activeLights;
			Bitset<UInt64> m_freeInstanceIds;
			Bitset<UInt64> m_invalidatedRenderables;
			Bitset<UInt64> m_removedLightInstances;
			Bitset<UInt64> m_removedSkeletonInstances;
			Bitset<UInt64> m_removedViewerInstances;
			Bitset<UInt64> m_removedInstances;
			Bitset<UInt64> m_shadowCastingLights;
			Bitset<UInt64> m_visibleShadowCastingLights;
			GpuDynamicArray m_directionalLights;
			GpuDynamicArray m_directionalShadowAtlasEntries;
			GpuDynamicArray m_instanceBuffer;
			GpuDynamicArray m_pointLights;
			GpuDynamicArray m_pointShadowAtlasEntries;
			GpuDynamicArray m_spotLights;
			GpuDynamicArray m_spotShadowAtlasEntries;
			ElementRendererRegistry& m_elementRegistry;
			MemoryPool<RenderableData> m_renderablePool;
			MemoryPool<LightData> m_lightPool;
			MemoryPool<MaterialInstanceData> m_materialInstancePool;
			MemoryPool<SkeletonInstanceData> m_skeletonInstancePool;
			MemoryPool<ViewerData> m_viewerPool;
			mutable ShaderBindingCache m_shaderBindingCache;
			UInt8 m_generationCounter;
			bool m_rebuildFrameGraph;
	};
}

#include <Nazara/Graphics/DefaultFramePipeline.inl>

#endif // NAZARA_GRAPHICS_DEFAULTFRAMEPIPELINE_HPP
