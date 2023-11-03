// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardFramePipeline.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/PipelineViewer.hpp>
#include <Nazara/Graphics/PointLight.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/SpotLight.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Math/Angle.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/Framebuffer.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <array>
#include <Nazara/Graphics/Debug.hpp>

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
		lightData->onLightInvalidated.Connect(lightData->light->OnLightDataInvalided, [=](Light*)
		{
			//TODO: Switch lights to storage buffers so they can all be part of GPU memory
		});

		lightData->onLightShadowCastingChanged.Connect(lightData->light->OnLightShadowCastingChanged, [=](Light* light, bool isCastingShadows)
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

						if ((viewerData.viewer->GetRenderMask() & lightData->renderMask) != 0)
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

					if ((viewerData.viewer->GetRenderMask() & lightData->renderMask) != 0)
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

				UInt32 viewerRenderMask = viewerData.viewer->GetRenderMask();

				if (viewerRenderMask & renderMask)
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

		FramePipelinePass::PassData passData = {
			viewerInstance,
			m_elementRegistry,
			*this
		};

		viewerData.passes = viewerInstance->BuildPasses(passData);

		m_transferSet.insert(&viewerInstance->GetViewerInstance());

		UInt32 renderMask = viewerInstance->GetRenderMask();
		for (std::size_t i : m_shadowCastingLights.IterBits())
		{
			LightData* lightData = m_lightPool.RetrieveFromIndex(i);
			if (lightData->shadowData->IsPerViewer() && (renderMask & lightData->renderMask) != 0)
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

	void ForwardFramePipeline::Render(RenderFrame& renderFrame)
	{
		m_currentRenderFrame = &renderFrame;

		Graphics* graphics = Graphics::Instance();

		// Destroy instances at the end of the frame
		for (std::size_t skeletonInstanceIndex : m_removedSkeletonInstances.IterBits())
		{
			renderFrame.PushForRelease(std::move(*m_skeletonInstances.RetrieveFromIndex(skeletonInstanceIndex)));
			m_skeletonInstances.Free(skeletonInstanceIndex);
		}
		m_removedSkeletonInstances.Clear();

		for (std::size_t viewerIndex : m_removedViewerInstances.IterBits())
		{
			renderFrame.PushForRelease(std::move(*m_viewerPool.RetrieveFromIndex(viewerIndex)));
			m_viewerPool.Free(viewerIndex);
		}
		m_removedViewerInstances.Clear();

		for (std::size_t worldInstanceIndex : m_removedWorldInstances.IterBits())
		{
			renderFrame.PushForRelease(std::move(*m_worldInstances.RetrieveFromIndex(worldInstanceIndex)));
			m_worldInstances.Free(worldInstanceIndex);
		}
		m_removedWorldInstances.Clear();

		bool frameGraphInvalidated;
		if (m_rebuildFrameGraph)
		{
			renderFrame.PushForRelease(std::move(m_bakedFrameGraph));
			m_bakedFrameGraph = BuildFrameGraph();
			m_bakedFrameGraph.Resize(renderFrame);
			frameGraphInvalidated = true;
		}
		else
			frameGraphInvalidated = m_bakedFrameGraph.Resize(renderFrame);

		// Find active lights (i.e. visible in any frustum)
		m_activeLights.Clear();
		for (auto& viewerData : m_viewerPool)
		{
			if (viewerData.pendingDestruction)
				continue;

			UInt32 renderMask = viewerData.viewer->GetRenderMask();

			// Extract frustum from viewproj matrix
			const Matrix4f& viewProjMatrix = viewerData.viewer->GetViewerInstance().GetViewProjMatrix();
			viewerData.frame.frustum = Frustumf::Extract(viewProjMatrix);

			viewerData.frame.visibleLights.Clear();
			for (auto it = m_lightPool.begin(); it != m_lightPool.end(); ++it)
			{
				const LightData& lightData = *it;
				std::size_t lightIndex = it.GetIndex();

				if ((lightData.renderMask & renderMask) == 0)
					continue;

				m_activeLights.UnboundedSet(lightIndex);
				viewerData.frame.visibleLights.UnboundedSet(lightIndex);
			}
		}

		m_visibleShadowCastingLights.PerformsAND(m_activeLights, m_shadowCastingLights);

		// Shadow map handling (for active lights)
		for (std::size_t i : m_visibleShadowCastingLights.IterBits())
		{
			LightData* lightData = m_lightPool.RetrieveFromIndex(i);
			if (!lightData->shadowData->IsPerViewer())
				lightData->shadowData->PrepareRendering(renderFrame, nullptr);
		}

		// Viewer handling (second pass)
		for (auto& viewerData : m_viewerPool)
		{
			if (viewerData.pendingDestruction)
				continue;

			UInt32 renderMask = viewerData.viewer->GetRenderMask();

			// Per-viewer shadow map handling
			for (std::size_t lightIndex : viewerData.frame.visibleLights.IterBits())
			{
				LightData* lightData = m_lightPool.RetrieveFromIndex(lightIndex);
				if (lightData->shadowData && lightData->shadowData->IsPerViewer() && (renderMask & lightData->renderMask) != 0)
					lightData->shadowData->PrepareRendering(renderFrame, viewerData.viewer);
			}

			// Frustum culling
			std::size_t visibilityHash = 5;
			const auto& visibleRenderables = FrustumCull(viewerData.frame.frustum, renderMask, visibilityHash);

			FramePipelinePass::FrameData passData = {
				&viewerData.frame.visibleLights,
				viewerData.frame.frustum,
				renderFrame,
				visibleRenderables,
				visibilityHash
			};

			for (auto& passPtr : viewerData.passes)
				passPtr->Prepare(passData);
		}

		if (frameGraphInvalidated)
		{
			const std::shared_ptr<TextureSampler>& sampler = graphics->GetSamplerCache().Get({});
			for (auto& viewerData : m_viewerPool)
			{
				if (viewerData.pendingDestruction)
					continue;

				if (viewerData.blitShaderBinding)
					renderFrame.PushForRelease(std::move(viewerData.blitShaderBinding));

				viewerData.blitShaderBinding = graphics->GetBlitPipelineLayout()->AllocateShaderBinding(0);
				viewerData.blitShaderBinding->Update({
					{
						0,
						ShaderBinding::SampledTextureBinding {
							m_bakedFrameGraph.GetAttachmentTexture(viewerData.finalColorAttachment).get(),
							sampler.get()
						}
					}
				});
			}

			for (auto&& [_, renderTargetData] : m_renderTargets)
			{
				if (renderTargetData.blitShaderBinding)
					renderFrame.PushForRelease(std::move(renderTargetData.blitShaderBinding));

				renderTargetData.blitShaderBinding = graphics->GetBlitPipelineLayout()->AllocateShaderBinding(0);
				renderTargetData.blitShaderBinding->Update({
					{
						0,
						ShaderBinding::SampledTextureBinding {
							m_bakedFrameGraph.GetAttachmentTexture(renderTargetData.finalAttachment).get(),
							sampler.get()
						}
					}
				});
			}
		}

		// Update UBOs and materials
		renderFrame.Execute([&](CommandBufferBuilder& builder)
		{
			builder.BeginDebugRegion("CPU to GPU transfers", Color::Yellow());
			{
				builder.PreTransferBarrier();

				for (TransferInterface* transferInterface : m_transferSet)
					transferInterface->OnTransfer(renderFrame, builder);
				m_transferSet.clear();

				OnTransfer(this, renderFrame, builder);

				builder.PostTransferBarrier();
			}
			builder.EndDebugRegion();
		}, QueueType::Transfer);

		m_bakedFrameGraph.Execute(renderFrame);
		m_rebuildFrameGraph = false;

		// Final blit (TODO: Make part of frame graph)
		const Vector2ui& frameSize = renderFrame.GetSize();
		for (auto&& [renderTargetPtr, renderTargetData] : m_renderTargets)
		{
			Recti renderRegion(0, 0, frameSize.x, frameSize.y);

			const RenderTarget& renderTarget = *renderTargetPtr;
			const auto& data = renderTargetData;
			renderFrame.Execute([&](CommandBufferBuilder& builder)
			{
				const std::shared_ptr<Texture>& sourceTexture = m_bakedFrameGraph.GetAttachmentTexture(data.finalAttachment);

				builder.TextureBarrier(PipelineStage::ColorOutput, PipelineStage::FragmentShader, MemoryAccess::ColorWrite, MemoryAccess::ShaderRead, TextureLayout::ColorOutput, TextureLayout::ColorInput, *sourceTexture);

				std::array<CommandBufferBuilder::ClearValues, 2> clearValues;
				clearValues[0].color = Color::Black();
				clearValues[1].depth = 1.f;
				clearValues[1].stencil = 0;

				builder.BeginRenderPass(renderTarget.GetFramebuffer(renderFrame.GetFramebufferIndex()), renderTarget.GetRenderPass(), renderRegion, { clearValues[0], clearValues[1] });
				{
					builder.BeginDebugRegion("Main window rendering", Color::Green());
					{
						builder.SetScissor(renderRegion);
						builder.SetViewport(renderRegion);
						builder.BindRenderPipeline(*graphics->GetBlitPipeline(false));

						builder.BindRenderShaderBinding(0, *data.blitShaderBinding);
						builder.Draw(3);
					}
					builder.EndDebugRegion();
				}
				builder.EndRenderPass();

			}, QueueType::Graphics);
		}

		// reset at the end instead of the beginning so debug draw can be used before calling this method
		DebugDrawer& debugDrawer = GetDebugDrawer();
		debugDrawer.Reset(renderFrame);
	}

	void ForwardFramePipeline::UnregisterLight(std::size_t lightIndex)
	{
		m_lightPool.Free(lightIndex);
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

		UInt32 renderMask = viewerData.viewer->GetRenderMask();
		for (std::size_t i : m_shadowCastingLights.IterBits())
		{
			LightData* lightData = m_lightPool.RetrieveFromIndex(i);
			if (lightData->shadowData->IsPerViewer() && (renderMask & lightData->renderMask) != 0)
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

			UInt32 viewerRenderMask = viewerData.viewer->GetRenderMask();

			if (viewerRenderMask & renderableData->renderMask)
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

			UInt32 viewerRenderMask = viewerData.viewer->GetRenderMask();

			if (viewerRenderMask & renderableData->renderMask)
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

		for (std::size_t i : m_shadowCastingLights.IterBits())
		{
			LightData* lightData = m_lightPool.RetrieveFromIndex(i);
			if (!lightData->shadowData->IsPerViewer())
				lightData->shadowData->RegisterToFrameGraph(frameGraph, nullptr);
		}

		for (auto& viewerData : m_viewerPool)
		{
			if (viewerData.pendingDestruction)
				continue;

			UInt32 renderMask = viewerData.viewer->GetRenderMask();
			for (std::size_t i : m_shadowCastingLights.IterBits())
			{
				LightData* lightData = m_lightPool.RetrieveFromIndex(i);
				if (lightData->shadowData->IsPerViewer() && (renderMask & lightData->renderMask) != 0)
					lightData->shadowData->RegisterToFrameGraph(frameGraph, viewerData.viewer);
			}

			auto framePassCallback = [this, &viewerData, renderMask](std::size_t /*passIndex*/, FramePass& framePass, FramePipelinePassFlags flags)
			{
				if (flags.Test(FramePipelinePassFlag::LightShadowing))
				{
					for (std::size_t i : m_shadowCastingLights.IterBits())
					{
						LightData* lightData = m_lightPool.RetrieveFromIndex(i);
						if ((renderMask & lightData->renderMask) != 0)
							lightData->shadowData->RegisterPassInputs(framePass, (lightData->shadowData->IsPerViewer()) ? viewerData.viewer : nullptr);
					}
				}
			};

			viewerData.finalColorAttachment = viewerData.viewer->RegisterPasses(viewerData.passes, frameGraph, framePassCallback);
		}

		using ViewerPair = std::pair<const RenderTarget*, const ViewerData*>;

		StackArray<ViewerPair> viewers = NazaraStackArray(ViewerPair, m_viewerPool.size());
		auto viewerIt = viewers.begin();

		for (auto& viewerData : m_viewerPool)
		{
			if (viewerData.pendingDestruction)
				continue;

			const RenderTarget& renderTarget = viewerData.viewer->GetRenderTarget();
			*viewerIt++ = std::make_pair(&renderTarget, &viewerData);
		}

		std::sort(viewers.begin(), viewers.end(), [](const ViewerPair& lhs, const ViewerPair& rhs)
		{
			return lhs.second->renderOrder < rhs.second->renderOrder;
		});

		m_renderTargets.clear();
		for (auto&& [renderTarget, viewerData] : viewers)
		{
			auto& renderTargetData = m_renderTargets[renderTarget];
			renderTargetData.viewers.push_back(viewerData);
		}

		for (auto&& [renderTarget, renderTargetData] : m_renderTargets)
		{
			const auto& targetViewers = renderTargetData.viewers;

			FramePass& mergePass = frameGraph.AddPass("Merge pass");

			renderTargetData.finalAttachment = frameGraph.AddAttachment({
				"Viewer output",
				PixelFormat::RGBA8
			});

			for (const ViewerData* viewerData : targetViewers)
				mergePass.AddInput(viewerData->finalColorAttachment);

			mergePass.AddOutput(renderTargetData.finalAttachment);
			mergePass.SetClearColor(0, Color::Black());

			mergePass.SetCommandCallback([&targetViewers](CommandBufferBuilder& builder, const FramePassEnvironment& /*env*/)
			{
				Graphics* graphics = Graphics::Instance();
				builder.BindRenderPipeline(*graphics->GetBlitPipeline(false));

				bool first = true;

				for (const ViewerData* viewerData : targetViewers)
				{
					Recti renderRect = viewerData->viewer->GetViewport();

					builder.SetScissor(renderRect);
					builder.SetViewport(renderRect);

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

			frameGraph.AddBackbufferOutput(renderTargetData.finalAttachment);
		}

		return frameGraph.Bake();
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
