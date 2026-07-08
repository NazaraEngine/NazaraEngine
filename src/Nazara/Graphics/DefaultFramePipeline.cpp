// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/DefaultFramePipeline.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/PipelineViewer.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/RenderTarget.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/DrawIndirect.hpp>
#include <NazaraUtils/StackVector.hpp>

namespace Nz
{
	DefaultFramePipeline::DefaultFramePipeline(ElementRendererRegistry& elementRegistry) :
	m_directionalLights(*Graphics::Instance()->GetRenderDevice(), PredefinedDirectionalLightOffsets.totalSize, 16, PredefinedDirectionalLightsOffsets.totalSize),
	m_directionalShadowAtlasEntries(*Graphics::Instance()->GetRenderDevice(), PredefinedDirectionalShadowAtlasEntryOffsets.totalSize, 16),
	m_indirectCommandBuffer(*Graphics::Instance()->GetRenderDevice(), sizeof(DrawIndexedIndirectCommand)),
	m_instanceBuffer(*Graphics::Instance()->GetRenderDevice(), PredefinedInstanceOffsets.totalSize, 512),
	m_pointLights(*Graphics::Instance()->GetRenderDevice(), PredefinedPointLightOffsets.totalSize, 128, PredefinedPointLightsOffsets.totalSize),
	m_pointShadowAtlasEntries(*Graphics::Instance()->GetRenderDevice(), PredefinedPointShadowAtlasEntryOffsets.totalSize, 128),
	m_spotLights(*Graphics::Instance()->GetRenderDevice(), PredefinedSpotLightOffsets.totalSize, 128, PredefinedSpotLightsOffsets.totalSize),
	m_spotShadowAtlasEntries(*Graphics::Instance()->GetRenderDevice(), PredefinedSpotShadowAtlasEntryOffsets.totalSize, 128),
	m_elementRegistry(elementRegistry),
	m_renderablePool(4096),
	m_lightPool(64),
	m_skeletonInstances(1024),
	m_viewerPool(8),
	m_generationCounter(0),
	m_rebuildFrameGraph(true)
	{
		// OnBufferInvalidated
		m_directionalLights.OnBufferInvalidated.Connect([this](GpuDynamicArray* /*gpuDynamicArray*/) { m_shaderBindingCache.InvalidateSceneBindings(); });
		m_directionalShadowAtlasEntries.OnBufferInvalidated.Connect([this](GpuDynamicArray* /*gpuDynamicArray*/) { m_shaderBindingCache.InvalidateSceneBindings(); });
		m_instanceBuffer.OnBufferInvalidated.Connect([this](GpuDynamicArray* /*gpuDynamicArray*/) { m_shaderBindingCache.InvalidateSceneBindings(); });
		m_pointLights.OnBufferInvalidated.Connect([this](GpuDynamicArray* /*gpuDynamicArray*/) { m_shaderBindingCache.InvalidateSceneBindings(); });
		m_pointShadowAtlasEntries.OnBufferInvalidated.Connect([this](GpuDynamicArray* /*gpuDynamicArray*/) { m_shaderBindingCache.InvalidateSceneBindings(); });
		m_spotLights.OnBufferInvalidated.Connect([this](GpuDynamicArray* /*gpuDynamicArray*/) { m_shaderBindingCache.InvalidateSceneBindings(); });
		m_spotShadowAtlasEntries.OnBufferInvalidated.Connect([this](GpuDynamicArray* /*gpuDynamicArray*/) { m_shaderBindingCache.InvalidateSceneBindings(); });

		// OnTransferRequired
		m_directionalLights.OnTransferRequired.Connect([this](TransferInterface* transfer) { m_transferSet.insert(transfer); });
		m_directionalShadowAtlasEntries.OnTransferRequired.Connect([this](TransferInterface* transfer) { m_transferSet.insert(transfer); });
		m_indirectCommandBuffer.OnTransferRequired.Connect([this](TransferInterface* transfer) { m_transferSet.insert(transfer); });
		m_instanceBuffer.OnTransferRequired.Connect([this](TransferInterface* transfer) { m_transferSet.insert(transfer); });
		m_pointLights.OnTransferRequired.Connect([this](TransferInterface* transfer) { m_transferSet.insert(transfer); });
		m_pointShadowAtlasEntries.OnTransferRequired.Connect([this](TransferInterface* transfer) { m_transferSet.insert(transfer); });
		m_spotLights.OnTransferRequired.Connect([this](TransferInterface* transfer) { m_transferSet.insert(transfer); });
		m_spotShadowAtlasEntries.OnTransferRequired.Connect([this](TransferInterface* transfer) { m_transferSet.insert(transfer); });

		m_directionalLights.UpdateDebugName("Directional Light buffer");
		m_directionalShadowAtlasEntries.UpdateDebugName("Directional Shadow Atlas Entries");
		m_indirectCommandBuffer.UpdateDebugName("Indirect command buffer");
		m_instanceBuffer.UpdateDebugName("Instance buffer");
		m_pointLights.UpdateDebugName("Point Light buffer");
		m_pointShadowAtlasEntries.UpdateDebugName("Point Shadow Atlas Entries");
		m_spotLights.UpdateDebugName("Spot Light buffer");
		m_spotShadowAtlasEntries.UpdateDebugName("Spot Shadow Atlas Entries");
	}

	DefaultFramePipeline::~DefaultFramePipeline()
	{
		// Force viewer passes to unregister their materials
		m_viewerPool.Clear();
	}

	void DefaultFramePipeline::ForEachRegisteredMaterialInstance(FunctionRef<void(const MaterialInstance& materialInstance)> callback)
	{
		for (RenderableData& renderable : m_renderablePool)
		{
			std::size_t matCount = renderable.renderable->GetMaterialCount();
			for (std::size_t j = 0; j < matCount; ++j)
			{
				if (MaterialInstance* mat = renderable.renderable->GetMaterial(j).get())
					callback(*mat);
			}
		}
	}

	void DefaultFramePipeline::ForEachShadowCastingLight(FunctionRef<void(std::size_t lightIndex, const Light* light, LightShadowData* lightShadowData)> callback)
	{
		for (std::size_t lightIndex : m_visibleShadowCastingLights.IterBits())
		{
			LightData* lightData = m_lightPool.RetrieveFromIndex(lightIndex);
			callback(lightIndex, lightData->light, lightData->shadowData.get());
		}
	}

	const std::shared_ptr<RenderBuffer>& DefaultFramePipeline::GetDirectionalLightBuffer() const
	{
		return m_directionalLights.GetBuffer();
	}

	const std::shared_ptr<RenderBuffer>& DefaultFramePipeline::GetDirectionalShadowMappingBuffer() const
	{
		return m_directionalShadowAtlasEntries.GetBuffer();
	}

	const std::shared_ptr<RenderBuffer>& DefaultFramePipeline::GetInstanceBuffer() const
	{
		return m_instanceBuffer.GetBuffer();
	}

	const std::shared_ptr<RenderBuffer>& DefaultFramePipeline::GetPointLightBuffer() const
	{
		return m_pointLights.GetBuffer();
	}

	const std::shared_ptr<RenderBuffer>& DefaultFramePipeline::GetPointShadowMappingBuffer() const
	{
		return m_pointShadowAtlasEntries.GetBuffer();
	}

	auto DefaultFramePipeline::GetRenderQueue(std::size_t materialPass) -> RenderQueue&
	{
		if (materialPass >= m_renderQueues.size())
			m_renderQueues.resize(materialPass + 1);

		if (!m_renderQueues[materialPass])
		{
			m_renderQueues[materialPass] = std::make_unique<RenderQueue>(m_elementRegistry, materialPass);

			for (const RenderableData& renderableData : m_renderablePool)
			{
				const SkeletonInstance* skeletonInstance;
				if (renderableData.skeletonInstanceIndex != NoSkeletonInstance)
					skeletonInstance = m_skeletonInstances.RetrieveFromIndex(renderableData.skeletonInstanceIndex)->skeleton.get();
				else
					skeletonInstance = nullptr;

				for (auto& renderQueuePtr : m_renderQueues)
				{
					if (renderQueuePtr)
						renderQueuePtr->RegisterRenderable(renderableData.renderableIndex, renderableData.instanceIndex, *renderableData.renderable, skeletonInstance, renderableData.renderMask, renderableData.scissorBox);
				}
			}
		}

		return *m_renderQueues[materialPass];
	}

	ShaderBindingCache* DefaultFramePipeline::GetShaderBindingCache() const
	{
		return &m_shaderBindingCache;
	}

	const std::shared_ptr<Texture>& DefaultFramePipeline::GetShadowAtlasTexture() const
	{
		if (!m_shadowAtlasPipelinePass)
		{
			Graphics* graphics = Graphics::Instance();
			return graphics->GetDefaultTextures().depthTextures[ImageType::E2D]->GetOrCreateTexture(*graphics->GetRenderDevice());
		}

		return m_shadowAtlasPipelinePass->GetAtlas().GetTexture();
	}

	const std::shared_ptr<RenderBuffer>& DefaultFramePipeline::GetSpotLightBuffer() const
	{
		return m_spotLights.GetBuffer();
	}

	const std::shared_ptr<RenderBuffer>& DefaultFramePipeline::GetSpotShadowMappingBuffer() const
	{
		return m_spotShadowAtlasEntries.GetBuffer();
	}


	void DefaultFramePipeline::QueueTransfer(TransferInterface* transfer)
	{
		m_transferSet.insert(transfer);
	}

	UInt32 DefaultFramePipeline::RegisterInstance()
	{
		constexpr std::size_t InstanceGrowRate = 1024;

		std::size_t instanceIndex = m_freeInstanceIds.FindFirst();
		if (instanceIndex == m_freeInstanceIds.npos)
		{
			instanceIndex = m_freeInstanceIds.GetSize();
			m_freeInstanceIds.Resize(instanceIndex + InstanceGrowRate, true);
		}

		m_freeInstanceIds.Reset(instanceIndex);

		if (instanceIndex >= m_instanceBuffer.GetSize())
			m_instanceBuffer.Push();

		return SafeCaster(instanceIndex);
	}

	std::size_t DefaultFramePipeline::RegisterLight(const Light* light, UInt32 renderMask)
	{
		std::size_t lightIndex;
		LightData* lightData = m_lightPool.Allocate(lightIndex);
		lightData->light = light;
		lightData->renderMask = renderMask;
		lightData->shadowMappingEntry = InvalidShadowMappingEntry;

		lightData->onLightInvalidated.Connect(lightData->light->OnLightDataInvalidated, [this, lightData](Light* light)
		{
			switch (light->GetLightType())
			{
				case SafeCast<int>(BasicLightType::Directional):
				{
					UInt8* lightDataPtr = m_directionalLights.AccessEntry(lightData->entryIndex);
					light->WriteToShader(lightDataPtr, lightData->shadowMappingEntry);
					break;
				}

				case SafeCast<int>(BasicLightType::Point):
				{
					UInt8* lightDataPtr = m_pointLights.AccessEntry(lightData->entryIndex);
					light->WriteToShader(lightDataPtr, lightData->shadowMappingEntry);
					break;
				}

				case SafeCast<int>(BasicLightType::Spot):
				{
					UInt8* lightDataPtr = m_spotLights.AccessEntry(lightData->entryIndex);
					light->WriteToShader(lightDataPtr, lightData->shadowMappingEntry);
					break;
				}
			}
		});

		lightData->onLightShadowMapSettingChange.Connect(lightData->light->OnLightShadowMapSettingChange, [this, lightData, lightIndex](Light* light, PixelFormat /*newPixelFormat*/, UInt32 /*newSize*/)
		{
			if (light->IsShadowCaster())
			{
				// If shadow atlas has not been created yet and we require shadow mapping, rebuild the frame graph to add the shadow mapping pass
				if (!m_shadowAtlasPipelinePass)
					m_rebuildFrameGraph = true;
			}
		});

		lightData->onLightShadowCastingChanged.Connect(lightData->light->OnLightShadowCastingChanged, [this, lightData, lightIndex](Light* /*light*/, bool isCastingShadows)
		{
			if (isCastingShadows)
				RegisterShadowCaster(lightIndex, lightData);
			else
				UnregisterShadowCaster(lightIndex, lightData);
		});

		if (lightData->light->IsShadowCaster())
			RegisterShadowCaster(lightIndex, lightData);

		switch (light->GetLightType())
		{
			case SafeCast<int>(BasicLightType::Directional):
			{
				lightData->entryIndex = m_directionalLights.Push();
				UInt8* lightDataPtr = m_directionalLights.AccessEntry(lightData->entryIndex);
				light->WriteToShader(lightDataPtr, lightData->shadowMappingEntry);

				UInt8* lightHeaderPtr = m_directionalLights.AccessHeader();
				AccessByOffset<UInt32&>(lightHeaderPtr, PredefinedDirectionalLightsOffsets.lightCount) += 1;

				if (lightData->entryIndex >= m_directionalLightEntriesToIndices.size())
					m_directionalLightEntriesToIndices.resize(lightData->entryIndex + 1);

				m_directionalLightEntriesToIndices[lightData->entryIndex] = lightIndex;
				break;
			}

			case SafeCast<int>(BasicLightType::Point):
			{
				lightData->entryIndex = m_pointLights.Push();
				UInt8* lightDataPtr = m_pointLights.AccessEntry(lightData->entryIndex);
				light->WriteToShader(lightDataPtr, lightData->shadowMappingEntry);

				UInt8* lightHeaderPtr = m_pointLights.AccessHeader();
				AccessByOffset<UInt32&>(lightHeaderPtr, PredefinedPointLightsOffsets.lightCount) += 1;

				if (lightData->entryIndex >= m_directionalLightEntriesToIndices.size())
					m_directionalLightEntriesToIndices.resize(lightData->entryIndex + 1);

				m_directionalLightEntriesToIndices[lightData->entryIndex] = lightIndex;

				if (lightData->entryIndex >= m_pointLightEntriesToIndices.size())
					m_pointLightEntriesToIndices.resize(lightData->entryIndex + 1);

				m_pointLightEntriesToIndices[lightData->entryIndex] = lightIndex;
				break;
			}

			case SafeCast<int>(BasicLightType::Spot):
			{
				lightData->entryIndex = m_spotLights.Push();
				UInt8* lightDataPtr = m_spotLights.AccessEntry(lightData->entryIndex);
				light->WriteToShader(lightDataPtr, lightData->shadowMappingEntry);

				UInt8* lightHeaderPtr = m_spotLights.AccessHeader();
				AccessByOffset<UInt32&>(lightHeaderPtr, PredefinedSpotLightsOffsets.lightCount) += 1;

				if (lightData->entryIndex >= m_spotLightEntriesToIndices.size())
					m_spotLightEntriesToIndices.resize(lightData->entryIndex + 1);

				m_spotLightEntriesToIndices[lightData->entryIndex] = lightIndex;
				break;
			}
		}

		return lightIndex;
	}

	std::size_t DefaultFramePipeline::RegisterRenderable(UInt32 instanceIndex, std::size_t skeletonInstanceIndex, const InstancedRenderable* instancedRenderable, UInt32 renderMask, const Recti& scissorBox)
	{
		std::size_t renderableIndex;
		RenderableData* renderableData = m_renderablePool.Allocate(renderableIndex);
		renderableData->instanceIndex = instanceIndex;
		renderableData->generation = m_generationCounter++;
		renderableData->renderable = instancedRenderable;
		renderableData->renderableIndex = renderableIndex;
		renderableData->renderMask = renderMask;
		renderableData->scissorBox = scissorBox;
		renderableData->skeletonInstanceIndex = skeletonInstanceIndex;

		renderableData->onElementInvalidated.Connect(instancedRenderable->OnElementInvalidated, [this, renderableData](InstancedRenderable* /*instancedRenderable*/)
		{
			BroadcastRenderable(*renderableData);
		});
		BroadcastRenderable(*renderableData);

		renderableData->onMaterialInvalidated.Connect(instancedRenderable->OnMaterialInvalidated, [this](InstancedRenderable* instancedRenderable, std::size_t materialIndex, const std::shared_ptr<MaterialInstance>& newMaterial)
		{
			if (newMaterial)
				RegisterMaterialInstance(newMaterial.get());

			const auto& prevMaterial = instancedRenderable->GetMaterial(materialIndex);
			if (prevMaterial)
				UnregisterMaterialInstance(prevMaterial.get());
		});

		std::size_t matCount = instancedRenderable->GetMaterialCount();
		for (std::size_t i = 0; i < matCount; ++i)
		{
			if (MaterialInstance* mat = instancedRenderable->GetMaterial(i).get())
				RegisterMaterialInstance(mat);
		}

		return renderableIndex;
	}

	std::size_t DefaultFramePipeline::RegisterSkeleton(SkeletonInstancePtr skeletonInstance)
	{
		std::size_t skeletonInstanceIndex;
		SkeletonInstanceData& skeletonInstanceData = *m_skeletonInstances.Allocate(skeletonInstanceIndex);
		skeletonInstanceData.skeleton = std::move(skeletonInstance);
		skeletonInstanceData.onTransferRequired.Connect(skeletonInstanceData.skeleton->OnTransferRequired, [this](TransferInterface* transferInterface)
		{
			m_transferSet.insert(transferInterface);
		});
		m_transferSet.insert(skeletonInstanceData.skeleton.get());

		return skeletonInstanceIndex;
	}

	std::size_t DefaultFramePipeline::RegisterViewer(PipelineViewer* viewerInstance, Int32 renderOrder)
	{
		std::size_t viewerIndex;
		auto& viewerData = *m_viewerPool.Allocate(viewerIndex);
		viewerData.renderOrder = renderOrder;
		viewerData.viewer = viewerInstance;
		viewerData.onTransferRequired.Connect(viewerInstance->GetViewerInstance().OnTransferRequired, [this](TransferInterface* transferInterface)
		{
			m_transferSet.insert(transferInterface);
		});

		viewerData.onRenderMaskUpdated.Connect(viewerInstance->OnRenderMaskUpdated, [this, &viewerData](AbstractViewer* viewer, UInt32 newRenderMask)
		{
			for (std::size_t i : m_shadowCastingLights.IterBits())
			{
				LightData* lightData = m_lightPool.RetrieveFromIndex(i);
				if (lightData->shadowData->IsPerViewer())
				{
					if ((viewerData.renderMask & lightData->renderMask) != 0 && (newRenderMask & lightData->renderMask) == 0)
					{
						// Went from visible to not visible
						lightData->shadowData->UnregisterViewer(viewer);
					}
					else if ((newRenderMask & lightData->renderMask) != 0 && (viewerData.renderMask & lightData->renderMask) == 0)
					{
						// Went from not visible to visible
						lightData->shadowData->RegisterViewer(viewer);
					}
				}
			}

			viewerData.renderMask = newRenderMask;
		});

		FramePipelinePass::PassData passData = {
			viewerInstance,
			m_elementRegistry,
			*this
		};

		viewerData.passes = viewerInstance->BuildPasses(passData);

		m_transferSet.insert(&viewerInstance->GetViewerInstance());

		viewerData.renderMask = viewerInstance->GetRenderMask();
		for (std::size_t i : m_shadowCastingLights.IterBits())
		{
			LightData* lightData = m_lightPool.RetrieveFromIndex(i);
			if (lightData->shadowData->IsPerViewer() && (viewerData.renderMask & lightData->renderMask) != 0)
				lightData->shadowData->RegisterViewer(viewerInstance);
		}

		m_rebuildFrameGraph = true;

		return viewerIndex;
	}

	const Light* DefaultFramePipeline::RetrieveLight(std::size_t lightIndex) const
	{
		return m_lightPool.RetrieveFromIndex(lightIndex)->light;
	}

	const LightShadowData* DefaultFramePipeline::RetrieveLightShadowData(std::size_t lightIndex) const
	{
		if (!m_shadowCastingLights.UnboundedTest(lightIndex))
			return nullptr;

		return m_lightPool.RetrieveFromIndex(lightIndex)->shadowData.get();
	}

	void DefaultFramePipeline::Render(RenderResources& renderResources)
	{
		// Destroy instances at the end of the frame

		for (std::size_t lightIndex : m_removedLightInstances.IterBits())
		{
			auto& lightData = *m_lightPool.RetrieveFromIndex(lightIndex);

			renderResources.PushForRelease(std::move(lightData));
			m_lightPool.Free(lightIndex);
		}
		m_removedLightInstances.Clear();

		for (std::size_t skeletonInstanceIndex : m_removedSkeletonInstances.IterBits())
		{
			auto& skeletonData = *m_skeletonInstances.RetrieveFromIndex(skeletonInstanceIndex);

			renderResources.PushForRelease(std::move(skeletonData));
			m_skeletonInstances.Free(skeletonInstanceIndex);
		}
		m_removedSkeletonInstances.Clear();

		for (std::size_t viewerIndex : m_removedViewerInstances.IterBits())
		{
			auto& viewerData = *m_viewerPool.RetrieveFromIndex(viewerIndex);

			m_shaderBindingCache.DestroyViewerCache(viewerData.viewer->GetViewerInstance());

			renderResources.PushForRelease(std::move(viewerData));
			m_viewerPool.Free(viewerIndex);
		}
		m_removedViewerInstances.Clear();

		if (std::size_t removedInstanceCount = m_removedInstances.Count(); removedInstanceCount != 0)
		{
			HybridVector<std::size_t, 8> removedInstances;
			removedInstances.reserve(removedInstanceCount);

			for (std::size_t instanceIndex : m_removedInstances.IterBits())
				removedInstances.push_back(instanceIndex);

			m_removedInstances.Clear();

			// TODO: Use a shared_ptr for this data
			renderResources.PushReleaseCallback([this, instanceIndices = std::move(removedInstances)]
			{
				for (std::size_t instanceIndex : instanceIndices)
					m_freeInstanceIds.UnboundedSet(instanceIndex);
			});
		}

		for (auto& renderQueuePtr : m_renderQueues)
		{
			if (renderQueuePtr)
				renderQueuePtr->Prepare(renderResources);
		}

		bool frameGraphInvalidated = false;
		if (m_rebuildFrameGraph)
		{
			renderResources.PushForRelease(std::move(m_bakedFrameGraph));
			m_bakedFrameGraph = BuildFrameGraph();
			frameGraphInvalidated = true;
		}

		StackVector<Vector2ui> viewerSizes = NazaraStackVector(Vector2ui, m_orderedViewers.size());
		for (ViewerData* viewerData : m_orderedViewers)
		{
			Recti viewport = viewerData->viewer->GetViewport();
			viewerSizes.emplace_back(Vector2i(viewport.width, viewport.height));
		}

		frameGraphInvalidated |= m_bakedFrameGraph.Resize(renderResources, viewerSizes);
		if (frameGraphInvalidated)
		{
			for (ViewerData& viewerData : m_viewerPool)
			{
				if (viewerData.blitShaderBinding)
					renderResources.PushForRelease(std::move(viewerData.blitShaderBinding));
			}
		}

		// Find active lights (i.e. visible in any frustum)
		m_activeLights.Clear();
		for (ViewerData* viewerData : m_orderedViewers)
		{
			// Extract frustum from viewproj matrix
			const Matrix4f& viewProjMatrix = viewerData->viewer->GetViewerInstance().GetViewProjMatrix();
			viewerData->frame.frustum = Frustumf::Extract(viewProjMatrix, viewerData->viewer->IsZReversed());

			viewerData->frame.visibleLights.Clear();
			for (auto it = m_lightPool.begin(); it != m_lightPool.end(); ++it)
			{
				const LightData& lightData = *it;
				std::size_t lightIndex = it.GetIndex();

				if ((lightData.renderMask & viewerData->renderMask) == 0)
					continue;

				m_activeLights.UnboundedSet(lightIndex);
				viewerData->frame.visibleLights.UnboundedSet(lightIndex);
			}
		}

		m_visibleShadowCastingLights.PerformsAND(m_activeLights, m_shadowCastingLights);

		// Shadow map handling (for active lights)
		if (m_shadowAtlasPipelinePass)
		{
			FramePipelinePass::FrameData passData = {
				nullptr,
				Nz::Frustumf{},
				renderResources
			};

			m_shadowAtlasPipelinePass->Prepare(passData);

			ShadowAtlas& shadowAtlas = m_shadowAtlasPipelinePass->GetAtlas();
			for (std::size_t lightIndex : m_shadowCastingLights.IterBits())
			{
				LightData* lightData = m_lightPool.RetrieveFromIndex(lightIndex);
				NazaraAssert(lightData->shadowData);
				lightData->shadowData->PrepareRendering(renderResources);

				// Write global states (independent from viewers)
				if (!lightData->shadowData->IsPerViewer())
				{
					switch (lightData->light->GetLightType())
					{
						case SafeCast<int>(BasicLightType::Directional):
							lightData->shadowData->WriteToShader(shadowAtlas, nullptr, m_directionalShadowAtlasEntries.AccessEntry(lightData->shadowMappingEntry));
							break;

						case SafeCast<int>(BasicLightType::Point):
							lightData->shadowData->WriteToShader(shadowAtlas, nullptr, m_pointShadowAtlasEntries.AccessEntry(lightData->shadowMappingEntry));
							break;

						case SafeCast<int>(BasicLightType::Spot):
							lightData->shadowData->WriteToShader(shadowAtlas, nullptr, m_spotShadowAtlasEntries.AccessEntry(lightData->shadowMappingEntry));
							break;
					}
				}
			}
		}

		// Viewer handling (second pass)
		for (ViewerData* viewerData : m_orderedViewers)
		{
			FramePipelinePass::FrameData passData = {
				&viewerData->frame.visibleLights,
				viewerData->frame.frustum,
				renderResources
			};

			for (auto& passPtr : viewerData->passes)
				passPtr->Prepare(passData);
		}

		// Rendering
		m_bakedFrameGraph.Execute(renderResources);
		m_rebuildFrameGraph = false;
	}

	void DefaultFramePipeline::UnregisterInstance(UInt32 worldInstance)
	{
		// Defer instance release
		m_removedInstances.UnboundedSet(worldInstance);
	}

	void DefaultFramePipeline::UnregisterLight(std::size_t lightIndex)
	{
		LightData* lightData = m_lightPool.RetrieveFromIndex(lightIndex);

		if (lightData->light->IsShadowCaster())
			UnregisterShadowCaster(lightIndex, lightData);

		switch (lightData->light->GetLightType())
		{
			case SafeCast<int>(BasicLightType::Directional):
			{
				UInt32 lightCount = m_directionalLights.GetSize();
				UInt32 lastLightIndex = lightCount - 1;
				if (lightData->entryIndex != lastLightIndex)
				{
					// Swap and pop idiom
					UInt8* lastLight = m_directionalLights.AccessEntry(lastLightIndex);
					UInt8* freeSlot = m_directionalLights.AccessEntry(lightData->entryIndex);
					std::memcpy(freeSlot, lastLight, m_directionalLights.GetEntrySize());

					// Re-assign the light corresponding to the last entry to the new free slot
					LightData& lastLightData = *m_lightPool.RetrieveFromIndex(m_directionalLightEntriesToIndices[lastLightIndex]);
					lastLightData.entryIndex = lightData->entryIndex;

					m_directionalLightEntriesToIndices[lightData->entryIndex] = m_directionalLightEntriesToIndices[lastLightIndex];
				}
				m_directionalLightEntriesToIndices[lastLightIndex] = MaxValue();
				m_directionalLights.Pop();

				UInt8* lightHeaderPtr = m_directionalLights.AccessHeader();
				AccessByOffset<UInt32&>(lightHeaderPtr, PredefinedDirectionalLightsOffsets.lightCount) -= 1;
				break;
			}

			case SafeCast<int>(BasicLightType::Point):
			{
				UInt32 lightCount = m_pointLights.GetSize();
				UInt32 lastLightIndex = lightCount - 1;
				if (lightData->entryIndex != lastLightIndex)
				{
					// Swap and pop idiom
					UInt8* lastLight = m_pointLights.AccessEntry(lastLightIndex);
					UInt8* freeSlot = m_pointLights.AccessEntry(lightData->entryIndex);
					std::memcpy(freeSlot, lastLight, m_pointLights.GetEntrySize());

					// Re-assign the light corresponding to the last entry to the new free slot
					LightData& lastLightData = *m_lightPool.RetrieveFromIndex(m_pointLightEntriesToIndices[lastLightIndex]);
					lastLightData.entryIndex = lightData->entryIndex;

					m_pointLightEntriesToIndices[lightData->entryIndex] = m_pointLightEntriesToIndices[lastLightIndex];
				}
				m_pointLightEntriesToIndices[lastLightIndex] = MaxValue();
				m_pointLights.Pop();

				UInt8* lightHeaderPtr = m_pointLights.AccessHeader();
				AccessByOffset<UInt32&>(lightHeaderPtr, PredefinedPointLightsOffsets.lightCount) -= 1;
				break;
			}

			case SafeCast<int>(BasicLightType::Spot):
			{
				UInt32 lightCount = m_spotLights.GetSize();
				UInt32 lastLightIndex = lightCount - 1;
				if (lightData->entryIndex != lastLightIndex)
				{
					// Swap and pop idiom
					UInt8* lastLight = m_spotLights.AccessEntry(lastLightIndex);
					UInt8* freeSlot = m_spotLights.AccessEntry(lightData->entryIndex);
					std::memcpy(freeSlot, lastLight, m_spotLights.GetEntrySize());

					// Re-assign the light corresponding to the last entry to the new free slot
					LightData& lastLightData = *m_lightPool.RetrieveFromIndex(m_spotLightEntriesToIndices[lastLightIndex]);
					lastLightData.entryIndex = lightData->entryIndex;

					m_spotLightEntriesToIndices[lightData->entryIndex] = m_spotLightEntriesToIndices[lastLightIndex];
				}
				m_spotLightEntriesToIndices[lastLightIndex] = MaxValue();
				m_spotLights.Pop();

				UInt8* lightHeaderPtr = m_spotLights.AccessHeader();
				AccessByOffset<UInt32&>(lightHeaderPtr, PredefinedSpotLightsOffsets.lightCount) -= 1;
				break;
			}
		}

		m_removedLightInstances.UnboundedSet(lightIndex);
	}

	void DefaultFramePipeline::UnregisterRenderable(std::size_t renderableIndex)
	{
		RenderableData& renderableData = *m_renderablePool.RetrieveFromIndex(renderableIndex);

		for (auto& renderQueuePtr : m_renderQueues)
		{
			if (renderQueuePtr)
				renderQueuePtr->UnregisterRenderable(renderableIndex);
		}

		std::size_t matCount = renderableData.renderable->GetMaterialCount();
		for (std::size_t i = 0; i < matCount; ++i)
		{
			const auto& material = renderableData.renderable->GetMaterial(i);
			UnregisterMaterialInstance(material.get());
		}

		m_renderablePool.Free(renderableIndex);
	}

	void DefaultFramePipeline::UnregisterSkeleton(std::size_t skeletonIndex)
	{
		auto& skeletonData = *m_skeletonInstances.RetrieveFromIndex(skeletonIndex);
		m_transferSet.erase(skeletonData.skeleton.get());

		// Defer instance release
		m_removedSkeletonInstances.UnboundedSet(skeletonIndex);
	}

	void DefaultFramePipeline::UnregisterViewer(std::size_t viewerIndex)
	{
		auto& viewerData = *m_viewerPool.RetrieveFromIndex(viewerIndex);
		viewerData.pendingDestruction = true;

		m_transferSet.erase(&viewerData.viewer->GetViewerInstance());

		for (std::size_t i : m_shadowCastingLights.IterBits())
		{
			LightData* lightData = m_lightPool.RetrieveFromIndex(i);
			if (lightData->shadowData->IsPerViewer() && (viewerData.renderMask & lightData->renderMask) != 0)
				lightData->shadowData->UnregisterViewer(viewerData.viewer);
		}

		// Defer instance release
		m_removedViewerInstances.UnboundedSet(viewerIndex);
		m_rebuildFrameGraph = true;
	}

	void DefaultFramePipeline::UpdateInstanceData(UInt32 instanceIndex, const Matrix4f& worldMatrix, const Matrix4f& invWorldMatrix)
	{
		UInt8* ptr = m_instanceBuffer.AccessEntry(instanceIndex);
		AccessByOffset<Matrix4f&>(ptr, PredefinedInstanceOffsets.worldMatrixOffset) = worldMatrix;
		AccessByOffset<Matrix4f&>(ptr, PredefinedInstanceOffsets.invWorldMatrixOffset) = invWorldMatrix;
	}

	void DefaultFramePipeline::UpdateLightRenderMask(std::size_t lightIndex, UInt32 renderMask)
	{
		LightData* lightData = m_lightPool.RetrieveFromIndex(lightIndex);
		lightData->renderMask = renderMask;
	}

	void DefaultFramePipeline::UpdateRenderableRenderMask(std::size_t renderableIndex, UInt32 renderMask)
	{
		RenderableData* renderableData = m_renderablePool.RetrieveFromIndex(renderableIndex);
		renderableData->renderMask = renderMask;
		BroadcastRenderable(*renderableData);
	}

	void DefaultFramePipeline::UpdateRenderableScissorBox(std::size_t renderableIndex, const Recti& scissorBox)
	{
		RenderableData* renderableData = m_renderablePool.RetrieveFromIndex(renderableIndex);
		renderableData->scissorBox = scissorBox;
		BroadcastRenderable(*renderableData);
	}

	void DefaultFramePipeline::UpdateRenderableSkeletonInstance(std::size_t renderableIndex, std::size_t skeletonIndex)
	{
		RenderableData* renderableData = m_renderablePool.RetrieveFromIndex(renderableIndex);
		renderableData->skeletonInstanceIndex = skeletonIndex;
		BroadcastRenderable(*renderableData);
	}

	void DefaultFramePipeline::UpdateViewerRenderOrder(std::size_t viewerIndex, Int32 renderOrder)
	{
		ViewerData* viewerData = m_viewerPool.RetrieveFromIndex(viewerIndex);
		assert(!viewerData->pendingDestruction);
		if (viewerData->renderOrder != renderOrder)
		{
			viewerData->renderOrder = renderOrder;
			m_rebuildFrameGraph = true;
		}
	}

	void DefaultFramePipeline::BroadcastRenderable(const RenderableData& renderableData)
	{
		const SkeletonInstance* skeletonInstance;
		if (renderableData.skeletonInstanceIndex != NoSkeletonInstance)
			skeletonInstance = m_skeletonInstances.RetrieveFromIndex(renderableData.skeletonInstanceIndex)->skeleton.get();
		else
			skeletonInstance = nullptr;

		for (auto& renderQueuePtr : m_renderQueues)
		{
			if (renderQueuePtr)
				renderQueuePtr->RegisterRenderable(renderableData.renderableIndex, renderableData.instanceIndex, *renderableData.renderable, skeletonInstance, renderableData.renderMask, renderableData.scissorBox);
		}
	}

	BakedFrameGraph DefaultFramePipeline::BuildFrameGraph()
	{
		FrameGraph frameGraph;

		std::size_t transferAttachment = InsertTransferPass(frameGraph, {});

		if (!m_shadowAtlasPipelinePass && m_shadowCastingLights.TestAny())
		{
			FramePipelinePass::PassData passData{ nullptr, m_elementRegistry, *this };
			m_shadowAtlasPipelinePass.emplace(passData);

			m_shaderBindingCache.InvalidateSceneBindings();
		}

		std::size_t shadowAtlasIndex = MaxValue();
		if (m_shadowAtlasPipelinePass)
		{
			FramePipelinePass::PassInputData passInputData;
			passInputData.attachmentIndex = transferAttachment;

			FramePass& finalPass = m_shadowAtlasPipelinePass->RegisterToFrameGraph(frameGraph, { .inputAttachments = std::span(&passInputData, 1) });
			shadowAtlasIndex = finalPass.GetDepthStencilOutput();
		}

		// Group every viewer by their render order and RenderTarget
		m_orderedViewers.clear();
		for (auto& viewerData : m_viewerPool)
		{
			if (viewerData.pendingDestruction)
				continue;

			m_orderedViewers.push_back(&viewerData);
		}

		if (m_orderedViewers.empty())
			return frameGraph.Bake();

		std::sort(m_orderedViewers.begin(), m_orderedViewers.end(), [](ViewerData* lhs, ViewerData* rhs)
		{
			// Order by RenderTarget render order and then by viewer render order
			Int32 leftTargetRenderOrder1 = lhs->viewer->GetRenderTarget().GetRenderOrder();
			Int32 rightTargetRenderOrder1 = rhs->viewer->GetRenderTarget().GetRenderOrder();

			if (leftTargetRenderOrder1 != rightTargetRenderOrder1)
				return leftTargetRenderOrder1 < rightTargetRenderOrder1;

			return lhs->renderOrder < rhs->renderOrder;
		});

		StackVector<std::size_t> dependenciesColorAttachments = NazaraStackVector(std::size_t, m_orderedViewers.size());
		std::size_t dependenciesColorAttachmentCount = 0;
		Int32 lastRenderOrder = m_orderedViewers.front()->viewer->GetRenderTarget().GetRenderOrder();

		unsigned int viewerIndex = 0;
		auto HandleRenderTarget = [&](const RenderTarget& renderTarget, std::span<ViewerData*> viewers)
		{
			if (renderTarget.GetRenderOrder() > lastRenderOrder)
			{
				dependenciesColorAttachmentCount = dependenciesColorAttachments.size();
				lastRenderOrder = renderTarget.GetRenderOrder();
			}

			assert(!viewers.empty());

			for (ViewerData* viewerData : viewers)
			{
				std::size_t prepareAttachment = InsertTransferPass(frameGraph, [this, viewerData]
				{
					PipelineViewer* viewer = viewerData->viewer;
					if (m_shadowCastingLights.TestAny())
					{
						assert(m_shadowAtlasPipelinePass);
						ShadowAtlas& shadowAtlas = m_shadowAtlasPipelinePass->GetAtlas();

						for (std::size_t lightIndex : m_shadowCastingLights.IterBits())
						{
							LightData* lightData = m_lightPool.RetrieveFromIndex(lightIndex);
							if (lightData->shadowData && lightData->shadowData->IsPerViewer() && (viewerData->renderMask & lightData->renderMask) != 0)
							{
								switch (lightData->light->GetLightType())
								{
									case SafeCast<int>(BasicLightType::Directional):
										lightData->shadowData->WriteToShader(shadowAtlas, viewer, m_directionalShadowAtlasEntries.AccessEntry(lightData->shadowMappingEntry));
										break;

									case SafeCast<int>(BasicLightType::Point):
										lightData->shadowData->WriteToShader(shadowAtlas, viewer, m_pointShadowAtlasEntries.AccessEntry(lightData->shadowMappingEntry));
										break;

									case SafeCast<int>(BasicLightType::Spot):
										lightData->shadowData->WriteToShader(shadowAtlas, viewer, m_spotShadowAtlasEntries.AccessEntry(lightData->shadowMappingEntry));
										break;
								}
							}
						}
					}
				});

				auto framePassCallback = [&, viewerData = viewerData](std::size_t /*passIndex*/, FramePass& framePass, FramePipelinePassFlags flags)
				{
					framePass.AddInput(prepareAttachment);

					// Inject previous final attachments as inputs for all passes, to force framegraph to order viewers passes relative to each other
					// TODO: Allow the user to define which pass of viewer A uses viewer B rendering
					for (std::size_t i = 0; i < dependenciesColorAttachmentCount; ++i)
						framePass.AddInput(dependenciesColorAttachments[i]);

					if (flags.Test(FramePipelinePassFlag::LightShadowing) && shadowAtlasIndex != MaxValue<std::size_t>())
						framePass.AddInput(shadowAtlasIndex);
				};

				viewerData->finalColorAttachment = viewerData->viewer->RegisterPasses(viewerData->passes, frameGraph, viewerIndex++, framePassCallback);
			}

			std::size_t finalAttachment;
			if (viewers.size() > 1)
			{
				// Multiple viewers on the same target, merge them
				finalAttachment = renderTarget.OnBuildGraph(frameGraph, BuildMergePass(frameGraph, viewers));
			}
			else
			{
				// Single viewer on that target
				const auto& viewer = *viewers.front();
				finalAttachment = renderTarget.OnBuildGraph(frameGraph, viewer.finalColorAttachment);
			}

			if (!renderTarget.IsFrameGraphOutput())
			{
				// Keep track of previous dependencies attachments
				dependenciesColorAttachments.push_back(finalAttachment);
			}
			else
				frameGraph.AddOutput(finalAttachment);
		};

		const RenderTarget* currentTarget = &m_orderedViewers.front()->viewer->GetRenderTarget();
		std::size_t currentTargetIndex = 0;
		for (std::size_t i = 1; i < m_orderedViewers.size(); ++i)
		{
			const RenderTarget* target = &m_orderedViewers[i]->viewer->GetRenderTarget();
			if (currentTarget != target)
			{
				HandleRenderTarget(*currentTarget, std::span(&m_orderedViewers[currentTargetIndex], &m_orderedViewers[i]));
				currentTarget = target;
				currentTargetIndex = i;
			}
		}
		HandleRenderTarget(*currentTarget, std::span(m_orderedViewers.data() + currentTargetIndex, m_orderedViewers.data() + m_orderedViewers.size()));

		return frameGraph.Bake();
	}

	std::size_t DefaultFramePipeline::BuildMergePass(FrameGraph& frameGraph, std::span<ViewerData*> targetViewers)
	{
		FramePass& mergePass = frameGraph.AddPass("Merge pass");

		std::size_t mergedAttachment = frameGraph.AddAttachment({
			"Merged output",
			PixelFormat::RGBA8
		});

		for (const ViewerData* viewerData : targetViewers)
			mergePass.AddInput(viewerData->finalColorAttachment);

		mergePass.AddOutput(mergedAttachment);
		mergePass.SetClearColor(0, Color::Black());

		mergePass.SetRenderCallback([targetViewers](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			Graphics* graphics = Graphics::Instance();
			builder.BindRenderPipeline(*graphics->GetBlitPipeline(false));

			bool first = true;

			for (ViewerData* viewerData : targetViewers)
			{
				Recti renderRect = viewerData->viewer->GetViewport();

				builder.SetScissor(renderRect);
				builder.SetViewport(renderRect);

				if (!viewerData->blitShaderBinding)
				{
					const std::shared_ptr<TextureSampler>& sampler = graphics->GetSamplerCache().Get({});

					viewerData->blitShaderBinding = graphics->GetBlitPipelineLayout()->AllocateShaderBinding(0);
					viewerData->blitShaderBinding->Update({
						{
							0,
							ShaderBinding::SampledTextureBinding {
								env.frameGraph.GetAttachmentTexture(viewerData->finalColorAttachment).get(),
								sampler.get()
							}
						}
					});
				}

				const ShaderBindingPtr& blitShaderBinding = viewerData->blitShaderBinding;

				builder.BindRenderShaderBinding(0, *blitShaderBinding);
				builder.Draw(3);

				if (first)
				{
					builder.BindRenderPipeline(*graphics->GetBlitPipeline(true));
					first = false;
				}
			}
		});

		return mergedAttachment;
	}

	std::size_t DefaultFramePipeline::InsertTransferPass(FrameGraph& frameGraph, std::function<void()> callback)
	{
		std::size_t viewerUploadAttachment = frameGraph.AddDummyAttachment();

		FramePass& framePass = frameGraph.AddPass("CPU to GPU transfers");
		framePass.SetExecutionCallback([this, cb = std::move(callback)]
		{
			if (cb)
				cb();

			return (!m_transferSet.empty()) ? FramePassExecution::UpdateAndExecute : FramePassExecution::Skip;
		});

		framePass.SetCommandCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			builder.MemoryBarrier({ .srcStageMask = PipelineStage::BottomOfPipe, .dstStageMask = PipelineStage::Transfer, .srcAccessMask = {}, .dstAccessMask = MemoryAccess::TransferRead | MemoryAccess::TransferWrite });

			for (TransferInterface* transferInterface : m_transferSet)
				transferInterface->OnTransfer(env.renderResources, builder);
			m_transferSet.clear();

			OnTransfer(this, env.renderResources, builder);

			builder.MemoryBarrier({ .srcStageMask = PipelineStage::Transfer, .dstStageMask = PipelineStage::ComputeShader | PipelineStage::FragmentShader | PipelineStage::VertexShader, .srcAccessMask = MemoryAccess::TransferRead | MemoryAccess::TransferWrite, .dstAccessMask = MemoryAccess::ShaderRead | MemoryAccess::UniformBufferRead });
		});

		framePass.AddOutput(viewerUploadAttachment);

		return viewerUploadAttachment;
	}

	void DefaultFramePipeline::RegisterMaterialInstance(MaterialInstance* materialInstance)
	{
		auto it = m_materialInstances.find(materialInstance);
		if (it == m_materialInstances.end())
		{
			it = m_materialInstances.emplace(materialInstance, MaterialInstanceData{}).first;
			it->second.onTransferRequired.Connect(materialInstance->OnTransferRequired, [this](TransferInterface* transferInterface)
			{
				m_transferSet.insert(transferInterface);
			});
			m_transferSet.insert(materialInstance);
		}

		it->second.usedCount++;
	}

	void DefaultFramePipeline::RegisterShadowCaster(std::size_t lightIndex, LightData* lightData)
	{
		m_shadowCastingLights.UnboundedSet(lightIndex);
		lightData->shadowData = lightData->light->InstanciateShadowData(*this, m_elementRegistry);
		if (lightData->shadowData->IsPerViewer())
		{
			for (auto& viewerData : m_viewerPool)
			{
				if (viewerData.pendingDestruction)
					continue;

				if ((viewerData.renderMask & lightData->renderMask) != 0)
					lightData->shadowData->RegisterViewer(viewerData.viewer);
			}
		}

		switch (lightData->light->GetLightType())
		{
			case SafeCast<int>(BasicLightType::Directional):
			{
				lightData->shadowMappingEntry = m_directionalShadowAtlasEntries.Push();

				if (lightData->shadowMappingEntry >= m_directionalShadowEntriesToIndices.size())
					m_directionalShadowEntriesToIndices.resize(lightData->shadowMappingEntry + 1);

				m_directionalShadowEntriesToIndices[lightData->shadowMappingEntry] = lightIndex;
				break;
			}

			case SafeCast<int>(BasicLightType::Point):
			{
				lightData->shadowMappingEntry = m_pointShadowAtlasEntries.Push();

				if (lightData->shadowMappingEntry >= m_pointShadowEntriesToIndices.size())
					m_pointShadowEntriesToIndices.resize(lightData->shadowMappingEntry + 1);

				m_pointShadowEntriesToIndices[lightData->shadowMappingEntry] = lightIndex;
				break;
			}

			case SafeCast<int>(BasicLightType::Spot):
			{
				lightData->shadowMappingEntry = m_spotShadowAtlasEntries.Push();

				if (lightData->shadowMappingEntry >= m_spotShadowEntriesToIndices.size())
					m_spotShadowEntriesToIndices.resize(lightData->shadowMappingEntry + 1);

				m_spotShadowEntriesToIndices[lightData->shadowMappingEntry] = lightIndex;
				break;
			}
		}

		// If shadow atlas has not been created yet and we require shadow mapping, rebuild the frame graph to add the shadow mapping pass
		if (!m_shadowAtlasPipelinePass)
			m_rebuildFrameGraph = true;
	}

	void DefaultFramePipeline::UnregisterMaterialInstance(MaterialInstance* materialInstance)
	{
		auto it = m_materialInstances.find(materialInstance);
		assert(it != m_materialInstances.end());

		MaterialInstanceData& materialInstanceData = it->second;
		assert(materialInstanceData.usedCount > 0);
		if (--materialInstanceData.usedCount == 0)
		{
			m_materialInstances.erase(it);
			m_transferSet.erase(materialInstance);
		}
	}

	void DefaultFramePipeline::UnregisterShadowCaster(std::size_t lightIndex, LightData* lightData)
	{
		m_shadowCastingLights.Reset(lightIndex);
		lightData->shadowData.reset();

		switch (lightData->light->GetLightType())
		{
			case SafeCast<int>(BasicLightType::Directional):
			{
				UInt32 lightCount = m_directionalShadowAtlasEntries.GetSize();
				UInt32 lastLightIndex = lightCount - 1;
				if (lightData->shadowMappingEntry != lastLightIndex)
				{
					// Swap and pop idiom
					UInt8* lastLight = m_directionalShadowAtlasEntries.AccessEntry(lastLightIndex);
					UInt8* freeSlot = m_directionalShadowAtlasEntries.AccessEntry(lightData->shadowMappingEntry);
					std::memcpy(freeSlot, lastLight, m_directionalShadowAtlasEntries.GetEntrySize());

					// Re-assign the light corresponding to the last entry to the new free slot
					LightData& lastLightData = *m_lightPool.RetrieveFromIndex(m_directionalShadowEntriesToIndices[lastLightIndex]);
					lastLightData.shadowMappingEntry = lightData->shadowMappingEntry;

					m_directionalShadowEntriesToIndices[lightData->shadowMappingEntry] = m_directionalShadowEntriesToIndices[lastLightIndex];
				}
				m_directionalShadowEntriesToIndices[lastLightIndex] = MaxValue();
				m_directionalShadowAtlasEntries.Pop();
				break;
			}

			case SafeCast<int>(BasicLightType::Point):
			{
				UInt32 lightCount = m_pointShadowAtlasEntries.GetSize();
				UInt32 lastLightIndex = lightCount - 1;
				if (lightData->shadowMappingEntry != lastLightIndex)
				{
					// Swap and pop idiom
					UInt8* lastLight = m_pointShadowAtlasEntries.AccessEntry(lastLightIndex);
					UInt8* freeSlot = m_pointShadowAtlasEntries.AccessEntry(lightData->shadowMappingEntry);
					std::memcpy(freeSlot, lastLight, m_pointShadowAtlasEntries.GetEntrySize());

					// Re-assign the light corresponding to the last entry to the new free slot
					LightData& lastLightData = *m_lightPool.RetrieveFromIndex(m_pointShadowEntriesToIndices[lastLightIndex]);
					lastLightData.shadowMappingEntry = lightData->shadowMappingEntry;

					m_pointShadowEntriesToIndices[lightData->shadowMappingEntry] = m_pointShadowEntriesToIndices[lastLightIndex];
				}
				m_pointShadowEntriesToIndices[lastLightIndex] = MaxValue();
				m_pointShadowAtlasEntries.Pop();
				break;
			}

			case SafeCast<int>(BasicLightType::Spot):
			{
				UInt32 lightCount = m_spotShadowAtlasEntries.GetSize();
				UInt32 lastLightIndex = lightCount - 1;
				if (lightData->shadowMappingEntry != lastLightIndex)
				{
					// Swap and pop idiom
					UInt8* lastLight = m_spotShadowAtlasEntries.AccessEntry(lastLightIndex);
					UInt8* freeSlot = m_spotShadowAtlasEntries.AccessEntry(lightData->shadowMappingEntry);
					std::memcpy(freeSlot, lastLight, m_spotShadowAtlasEntries.GetEntrySize());

					// Re-assign the light corresponding to the last entry to the new free slot
					LightData& lastLightData = *m_lightPool.RetrieveFromIndex(m_spotShadowEntriesToIndices[lastLightIndex]);
					lastLightData.shadowMappingEntry = lightData->shadowMappingEntry;

					m_spotShadowEntriesToIndices[lightData->shadowMappingEntry] = m_spotShadowEntriesToIndices[lastLightIndex];
				}
				m_spotShadowEntriesToIndices[lastLightIndex] = MaxValue();
				m_spotShadowAtlasEntries.Pop();
				break;
			}
		}

		lightData->shadowMappingEntry = InvalidShadowMappingEntry;
	}
}
