// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ForwardFramePipeline.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/PipelineViewer.hpp>
#include <Nazara/Graphics/RenderTarget.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <NazaraUtils/StackVector.hpp>

namespace Nz
{
	ForwardFramePipeline::ForwardFramePipeline(ElementRendererRegistry& elementRegistry) :
	m_elementRegistry(elementRegistry),
	m_renderablePool(4096),
	m_lightPool(64),
	m_skeletonInstances(1024),
	m_viewerPool(8),
	m_worldInstances(2048),
	m_generationCounter(0),
	m_rebuildFrameGraph(true)
	{
	}

	ForwardFramePipeline::~ForwardFramePipeline()
	{
		// Force viewer passes to unregister their materials
		m_viewerPool.Clear();
	}

	const std::vector<Nz::FramePipelinePass::VisibleRenderable>& ForwardFramePipeline::FrustumCull(const Frustumf& frustum, UInt32 mask, std::size_t& visibilityHash) const
	{
		auto CombineHash = [](std::size_t currentHash, std::size_t newHash)
		{
			return currentHash * 23 + newHash;
		};

		m_visibleRenderables.clear();
		for (const RenderableData& renderableData : m_renderablePool)
		{
			if ((mask & renderableData.renderMask) == 0)
				continue;

			const WorldInstancePtr& worldInstance = m_worldInstances.RetrieveFromIndex(renderableData.worldInstanceIndex)->worldInstance;

			// Get global AABB
			BoundingVolumef boundingVolume(renderableData.renderable->GetAABB());
			boundingVolume.Update(worldInstance->GetWorldMatrix());

			if (frustum.Intersect(boundingVolume) == IntersectionSide::Outside)
				continue;

			auto& visibleRenderable = m_visibleRenderables.emplace_back();
			visibleRenderable.instancedRenderable = renderableData.renderable;
			visibleRenderable.scissorBox = renderableData.scissorBox;
			visibleRenderable.worldInstance = worldInstance.get();

			if (renderableData.skeletonInstanceIndex != NoSkeletonInstance)
				visibleRenderable.skeletonInstance = m_skeletonInstances.RetrieveFromIndex(renderableData.skeletonInstanceIndex)->skeleton.get();
			else
				visibleRenderable.skeletonInstance = nullptr;

			visibilityHash = CombineHash(visibilityHash, std::hash<const void*>()(&renderableData) + renderableData.generation);
		}

		return m_visibleRenderables;
	}

	void ForwardFramePipeline::ForEachRegisteredMaterialInstance(FunctionRef<void(const MaterialInstance& materialInstance)> callback)
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

	void ForwardFramePipeline::QueueTransfer(TransferInterface* transfer)
	{
		m_transferSet.insert(transfer);
	}

	std::size_t ForwardFramePipeline::RegisterLight(const Light* light, UInt32 renderMask)
	{
		std::size_t lightIndex;
		LightData* lightData = m_lightPool.Allocate(lightIndex);
		lightData->light = light;
		lightData->renderMask = renderMask;
		lightData->onLightInvalidated.Connect(lightData->light->OnLightDataInvalided, [](Light*)
		{
			//TODO: Switch lights to storage buffers so they can all be part of GPU memory
		});

		lightData->onLightShadowCastingChanged.Connect(lightData->light->OnLightShadowCastingChanged, [this, lightData, lightIndex](Light* light, bool isCastingShadows)
		{
			if (isCastingShadows)
			{
				m_shadowCastingLights.UnboundedSet(lightIndex);
				lightData->shadowData = light->InstanciateShadowData(*this, m_elementRegistry);
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
			}
			else
			{
				m_shadowCastingLights.Reset(lightIndex);
				lightData->shadowData.reset();
			}

			m_rebuildFrameGraph = true;
		});

		if (lightData->light->IsShadowCaster())
		{
			m_shadowCastingLights.UnboundedSet(lightIndex);
			lightData->shadowData = light->InstanciateShadowData(*this, m_elementRegistry);
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

			m_rebuildFrameGraph = true;
		}

		return lightIndex;
	}

	std::size_t ForwardFramePipeline::RegisterRenderable(std::size_t worldInstanceIndex, std::size_t skeletonInstanceIndex, const InstancedRenderable* instancedRenderable, UInt32 renderMask, const Recti& scissorBox)
	{
		std::size_t renderableIndex;
		RenderableData* renderableData = m_renderablePool.Allocate(renderableIndex);
		renderableData->generation = m_generationCounter++;
		renderableData->renderable = instancedRenderable;
		renderableData->renderMask = renderMask;
		renderableData->scissorBox = scissorBox;
		renderableData->skeletonInstanceIndex = skeletonInstanceIndex;
		renderableData->worldInstanceIndex = worldInstanceIndex;

		renderableData->onElementInvalidated.Connect(instancedRenderable->OnElementInvalidated, [=](InstancedRenderable* /*instancedRenderable*/)
		{
			// TODO: Invalidate only relevant viewers and passes
			for (auto& viewerData : m_viewerPool)
			{
				if (viewerData.pendingDestruction)
					continue;

				if (viewerData.renderMask & renderMask)
				{
					for (auto& passPtr : viewerData.passes)
					{
						if (passPtr->ShouldNotify(FramePipelineNotification::ElementInvalidation))
							passPtr->InvalidateElements();
					}
				}
			}
		});

		renderableData->onMaterialInvalidated.Connect(instancedRenderable->OnMaterialInvalidated, [this](InstancedRenderable* instancedRenderable, std::size_t materialIndex, const std::shared_ptr<MaterialInstance>& newMaterial)
		{
			if (newMaterial)
			{
				RegisterMaterialInstance(newMaterial.get());

				for (auto& viewerData : m_viewerPool)
				{
					if (viewerData.pendingDestruction)
						continue;

					for (auto& passPtr : viewerData.passes)
					{
						if (passPtr->ShouldNotify(FramePipelineNotification::MaterialInstanceRegistration))
							passPtr->RegisterMaterialInstance(*newMaterial);
					}
				}
			}

			const auto& prevMaterial = instancedRenderable->GetMaterial(materialIndex);
			if (prevMaterial)
			{
				UnregisterMaterialInstance(prevMaterial.get());

				for (auto& viewerData : m_viewerPool)
				{
					if (viewerData.pendingDestruction)
						continue;

					for (auto& passPtr : viewerData.passes)
					{
						if (passPtr->ShouldNotify(FramePipelineNotification::MaterialInstanceRegistration))
							passPtr->UnregisterMaterialInstance(*prevMaterial);
					}
				}
			}
		});

		std::size_t matCount = instancedRenderable->GetMaterialCount();
		for (std::size_t i = 0; i < matCount; ++i)
		{
			if (MaterialInstance* mat = instancedRenderable->GetMaterial(i).get())
			{
				RegisterMaterialInstance(mat);

				for (auto& viewerData : m_viewerPool)
				{
					if (viewerData.pendingDestruction)
						continue;

					for (auto& passPtr : viewerData.passes)
					{
						if (passPtr->ShouldNotify(FramePipelineNotification::MaterialInstanceRegistration))
							passPtr->RegisterMaterialInstance(*mat);
					}
				}
			}
		}

		return renderableIndex;
	}

	std::size_t ForwardFramePipeline::RegisterSkeleton(SkeletonInstancePtr skeletonInstance)
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

	std::size_t ForwardFramePipeline::RegisterViewer(PipelineViewer* viewerInstance, Int32 renderOrder)
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

	std::size_t ForwardFramePipeline::RegisterWorldInstance(WorldInstancePtr worldInstance)
	{
		std::size_t worldInstanceIndex;
		WorldInstanceData& worldInstanceData = *m_worldInstances.Allocate(worldInstanceIndex);
		worldInstanceData.worldInstance = std::move(worldInstance);
		worldInstanceData.onTransferRequired.Connect(worldInstanceData.worldInstance->OnTransferRequired, [this](TransferInterface* transferInterface)
		{
			m_transferSet.insert(transferInterface);
		});

		m_transferSet.insert(worldInstanceData.worldInstance.get());

		return worldInstanceIndex;
	}

	const Light* ForwardFramePipeline::RetrieveLight(std::size_t lightIndex) const
	{
		return m_lightPool.RetrieveFromIndex(lightIndex)->light;
	}

	const LightShadowData* ForwardFramePipeline::RetrieveLightShadowData(std::size_t lightIndex) const
	{
		if (!m_shadowCastingLights.UnboundedTest(lightIndex))
			return nullptr;

		return m_lightPool.RetrieveFromIndex(lightIndex)->shadowData.get();
	}

	const Texture* ForwardFramePipeline::RetrieveLightShadowmap(std::size_t lightIndex, const AbstractViewer* viewer) const
	{
		const LightShadowData* lightShadowData = RetrieveLightShadowData(lightIndex);
		if (!lightShadowData)
			return nullptr;

		return lightShadowData->RetrieveLightShadowmap(m_bakedFrameGraph, viewer);
	}

	void ForwardFramePipeline::Render(RenderResources& renderResources)
	{
		// Destroy instances at the end of the frame

		for (std::size_t lightIndex : m_removedLightInstances.IterBits())
		{
			renderResources.PushForRelease(std::move(*m_lightPool.RetrieveFromIndex(lightIndex)));
			m_lightPool.Free(lightIndex);
		}
		m_removedLightInstances.Clear();

		for (std::size_t skeletonInstanceIndex : m_removedSkeletonInstances.IterBits())
		{
			renderResources.PushForRelease(std::move(*m_skeletonInstances.RetrieveFromIndex(skeletonInstanceIndex)));
			m_skeletonInstances.Free(skeletonInstanceIndex);
		}
		m_removedSkeletonInstances.Clear();

		for (std::size_t viewerIndex : m_removedViewerInstances.IterBits())
		{
			auto& viewerData = *m_viewerPool.RetrieveFromIndex(viewerIndex);
			renderResources.PushForRelease(std::move(viewerData));
			m_viewerPool.Free(viewerIndex);
		}
		m_removedViewerInstances.Clear();

		for (std::size_t worldInstanceIndex : m_removedWorldInstances.IterBits())
		{
			renderResources.PushForRelease(std::move(*m_worldInstances.RetrieveFromIndex(worldInstanceIndex)));
			m_worldInstances.Free(worldInstanceIndex);
		}
		m_removedWorldInstances.Clear();

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
			viewerData->frame.frustum = Frustumf::Extract(viewProjMatrix);

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
		for (std::size_t i : m_visibleShadowCastingLights.IterBits())
		{
			LightData* lightData = m_lightPool.RetrieveFromIndex(i);
			if (!lightData->shadowData->IsPerViewer())
				lightData->shadowData->PrepareRendering(renderResources, nullptr);
		}

		// Viewer handling (second pass)
		for (ViewerData* viewerData : m_orderedViewers)
		{
			// Per-viewer shadow map handling
			for (std::size_t lightIndex : viewerData->frame.visibleLights.IterBits())
			{
				LightData* lightData = m_lightPool.RetrieveFromIndex(lightIndex);
				if (lightData->shadowData && lightData->shadowData->IsPerViewer() && (viewerData->renderMask & lightData->renderMask) != 0)
					lightData->shadowData->PrepareRendering(renderResources, viewerData->viewer);
			}

			// Frustum culling
			std::size_t visibilityHash = 5;
			const auto& visibleRenderables = FrustumCull(viewerData->frame.frustum, viewerData->renderMask, visibilityHash);

			FramePipelinePass::FrameData passData = {
				&viewerData->frame.visibleLights,
				viewerData->frame.frustum,
				renderResources,
				visibleRenderables,
				visibilityHash
			};

			for (auto& passPtr : viewerData->passes)
				passPtr->Prepare(passData);
		}

		// Update UBOs and materials
		renderResources.Execute([&](CommandBufferBuilder& builder)
		{
			builder.BeginDebugRegion("CPU to GPU transfers", Color::Yellow());
			{
				builder.PreTransferBarrier();

				for (TransferInterface* transferInterface : m_transferSet)
					transferInterface->OnTransfer(renderResources, builder);
				m_transferSet.clear();

				OnTransfer(this, renderResources, builder);

				builder.PostTransferBarrier();
			}
			builder.EndDebugRegion();
		}, QueueType::Transfer);

		m_bakedFrameGraph.Execute(renderResources);
		m_rebuildFrameGraph = false;

		// reset at the end instead of the beginning so debug draw can be used before calling this method
		DebugDrawer& debugDrawer = GetDebugDrawer();
		debugDrawer.Reset(renderResources);
	}

	void ForwardFramePipeline::UnregisterLight(std::size_t lightIndex)
	{
		m_removedLightInstances.UnboundedSet(lightIndex);

		if (m_shadowCastingLights.UnboundedTest(lightIndex))
		{
			m_shadowCastingLights.Reset(lightIndex);
			m_rebuildFrameGraph = true;
		}
	}

	void ForwardFramePipeline::UnregisterRenderable(std::size_t renderableIndex)
	{
		RenderableData& renderable = *m_renderablePool.RetrieveFromIndex(renderableIndex);

		std::size_t matCount = renderable.renderable->GetMaterialCount();
		for (std::size_t i = 0; i < matCount; ++i)
		{
			const auto& material = renderable.renderable->GetMaterial(i);
			UnregisterMaterialInstance(material.get());

			for (auto& viewerData : m_viewerPool)
			{
				if (viewerData.pendingDestruction)
					continue;

				for (auto& passPtr : viewerData.passes)
				{
					if (passPtr->ShouldNotify(FramePipelineNotification::MaterialInstanceRegistration))
						passPtr->UnregisterMaterialInstance(*material);
				}
			}
		}

		m_renderablePool.Free(renderableIndex);
	}

	void ForwardFramePipeline::UnregisterSkeleton(std::size_t skeletonIndex)
	{
		// Defer instance release
		m_removedSkeletonInstances.UnboundedSet(skeletonIndex);
	}

	void ForwardFramePipeline::UnregisterViewer(std::size_t viewerIndex)
	{
		auto& viewerData = *m_viewerPool.RetrieveFromIndex(viewerIndex);
		viewerData.pendingDestruction = true;

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

	void ForwardFramePipeline::UnregisterWorldInstance(std::size_t worldInstance)
	{
		// Defer instance release
		m_removedWorldInstances.UnboundedSet(worldInstance);
	}

	void ForwardFramePipeline::UpdateLightRenderMask(std::size_t lightIndex, UInt32 renderMask)
	{
		LightData* lightData = m_lightPool.RetrieveFromIndex(lightIndex);
		lightData->renderMask = renderMask;
	}

	void ForwardFramePipeline::UpdateRenderableRenderMask(std::size_t renderableIndex, UInt32 renderMask)
	{
		RenderableData* renderableData = m_renderablePool.RetrieveFromIndex(renderableIndex);
		renderableData->renderMask = renderMask;
	}

	void ForwardFramePipeline::UpdateRenderableScissorBox(std::size_t renderableIndex, const Recti& scissorBox)
	{
		RenderableData* renderableData = m_renderablePool.RetrieveFromIndex(renderableIndex);
		renderableData->scissorBox = scissorBox;

		// TODO: Invalidate only relevant viewers and passes
		for (auto& viewerData : m_viewerPool)
		{
			if (viewerData.pendingDestruction)
				continue;

			if (viewerData.renderMask & renderableData->renderMask)
			{
				for (auto& passPtr : viewerData.passes)
				{
					if (passPtr->ShouldNotify(FramePipelineNotification::ElementInvalidation))
						passPtr->InvalidateElements();
				}
			}
		}
	}

	void ForwardFramePipeline::UpdateRenderableSkeletonInstance(std::size_t renderableIndex, std::size_t skeletonIndex)
	{
		RenderableData* renderableData = m_renderablePool.RetrieveFromIndex(renderableIndex);
		renderableData->skeletonInstanceIndex = skeletonIndex;

		// TODO: Invalidate only relevant viewers and passes
		for (auto& viewerData : m_viewerPool)
		{
			if (viewerData.pendingDestruction)
				continue;

			if (viewerData.renderMask & renderableData->renderMask)
			{
				for (auto& passPtr : viewerData.passes)
				{
					if (passPtr->ShouldNotify(FramePipelineNotification::ElementInvalidation))
						passPtr->InvalidateElements();
				}
			}
		}
	}

	void ForwardFramePipeline::UpdateViewerRenderOrder(std::size_t viewerIndex, Int32 renderOrder)
	{
		ViewerData* viewerData = m_viewerPool.RetrieveFromIndex(viewerIndex);
		assert(!viewerData->pendingDestruction);
		if (viewerData->renderOrder != renderOrder)
		{
			viewerData->renderOrder = renderOrder;
			m_rebuildFrameGraph = true;
		}
	}

	BakedFrameGraph ForwardFramePipeline::BuildFrameGraph()
	{
		FrameGraph frameGraph;

		// Register viewer-independent passes
		for (std::size_t i : m_shadowCastingLights.IterBits())
		{
			LightData* lightData = m_lightPool.RetrieveFromIndex(i);
			if (!lightData->shadowData->IsPerViewer())
				lightData->shadowData->RegisterToFrameGraph(frameGraph, nullptr);
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
				for (std::size_t i : m_shadowCastingLights.IterBits())
				{
					LightData* lightData = m_lightPool.RetrieveFromIndex(i);
					if (lightData->shadowData->IsPerViewer() && (viewerData->renderMask & lightData->renderMask) != 0)
						lightData->shadowData->RegisterToFrameGraph(frameGraph, viewerData->viewer);
				}

				auto framePassCallback = [&, viewerData = viewerData](std::size_t /*passIndex*/, FramePass& framePass, FramePipelinePassFlags flags)
				{
					// Inject previous final attachments as inputs for all passes, to force framegraph to order viewers passes relative to each other
					// TODO: Allow the user to define which pass of viewer A uses viewer B rendering
					for (std::size_t i = 0; i < dependenciesColorAttachmentCount; ++i)
						framePass.AddInput(dependenciesColorAttachments[i]);

					if (flags.Test(FramePipelinePassFlag::LightShadowing))
					{
						for (std::size_t i : m_shadowCastingLights.IterBits())
						{
							LightData* lightData = m_lightPool.RetrieveFromIndex(i);
							if ((viewerData->renderMask & lightData->renderMask) != 0)
								lightData->shadowData->RegisterPassInputs(framePass, (lightData->shadowData->IsPerViewer()) ? viewerData->viewer : nullptr);
						}
					}
				};

				viewerData->finalColorAttachment = viewerData->viewer->RegisterPasses(viewerData->passes, frameGraph, viewerIndex++, framePassCallback);
			}

			std::size_t finalAttachment;
			if (viewers.size() > 1)
			{
				// Multiple viewers on the same targets, merge them
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

	std::size_t ForwardFramePipeline::BuildMergePass(FrameGraph& frameGraph, std::span<ViewerData*> targetViewers)
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

		mergePass.SetCommandCallback([targetViewers](CommandBufferBuilder& builder, const FramePassEnvironment& env)
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

	void ForwardFramePipeline::RegisterMaterialInstance(MaterialInstance* materialInstance)
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

	void ForwardFramePipeline::UnregisterMaterialInstance(MaterialInstance* materialInstance)
	{
		auto it = m_materialInstances.find(materialInstance);
		assert(it != m_materialInstances.end());

		MaterialInstanceData& materialInstanceData = it->second;
		assert(materialInstanceData.usedCount > 0);
		if (--materialInstanceData.usedCount == 0)
			m_materialInstances.erase(it);
	}
}
