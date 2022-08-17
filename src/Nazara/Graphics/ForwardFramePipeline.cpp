// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardFramePipeline.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/PointLight.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Math/Angle.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/Framebuffer.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/Utils/StackArray.hpp>
#include <array>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ForwardFramePipeline::ForwardFramePipeline() :
	m_renderablePool(4096),
	m_lightPool(64),
	m_viewerPool(8),
	m_worldInstances(2048),
	m_rebuildFrameGraph(true)
	{
	}

	ForwardFramePipeline::~ForwardFramePipeline()
	{
		// Force viewer passes to unregister their materials
		m_viewerPool.Clear();
	}

	void ForwardFramePipeline::InvalidateViewer(std::size_t viewerIndex)
	{
		m_invalidatedViewerInstances.Set(viewerIndex);
	}

	void ForwardFramePipeline::InvalidateWorldInstance(std::size_t worldInstanceIndex)
	{
		m_invalidatedWorldInstances.Set(worldInstanceIndex);
	}

	std::size_t ForwardFramePipeline::RegisterLight(std::shared_ptr<Light> light, UInt32 renderMask)
	{
		std::size_t lightIndex;
		LightData* lightData = m_lightPool.Allocate(lightIndex);
		lightData->light = std::move(light);
		lightData->renderMask = renderMask;
		lightData->onLightInvalidated.Connect(lightData->light->OnLightDataInvalided, [=](Light*)
		{
			//TODO: Switch lights to storage buffers so they can all be part of GPU memory
			for (auto& viewerData : m_viewerPool)
			{
				UInt32 viewerRenderMask = viewerData.viewer->GetRenderMask();

				if (viewerRenderMask & renderMask)
					viewerData.forwardPass->InvalidateElements();
			}
		});

		return lightIndex;
	}

	void ForwardFramePipeline::RegisterMaterialPass(MaterialPass* materialPass)
	{
		auto it = m_activeMaterialPasses.find(materialPass);
		if (it == m_activeMaterialPasses.end())
		{
			it = m_activeMaterialPasses.emplace(materialPass, MaterialPassData{}).first;
			it->second.onMaterialPassInvalided.Connect(materialPass->OnMaterialPassInvalidated, [=](const MaterialPass* /*material*/)
			{
				m_invalidatedMaterialPasses.insert(materialPass);
			});

			m_invalidatedMaterialPasses.insert(materialPass);
		}

		it->second.usedCount++;
	}
	
	std::size_t ForwardFramePipeline::RegisterRenderable(std::size_t worldInstanceIndex, const InstancedRenderable* instancedRenderable, UInt32 renderMask, const Recti& scissorBox)
	{
		std::size_t renderableIndex;
		RenderableData* renderableData = m_renderablePool.Allocate(renderableIndex);
		renderableData->renderable = instancedRenderable;
		renderableData->renderMask = renderMask;
		renderableData->scissorBox = scissorBox;
		renderableData->worldInstanceIndex = worldInstanceIndex;

		renderableData->onElementInvalidated.Connect(instancedRenderable->OnElementInvalidated, [=](InstancedRenderable* /*instancedRenderable*/)
		{
			// TODO: Invalidate only relevant viewers and passes
			for (auto& viewerData : m_viewerPool)
			{
				UInt32 viewerRenderMask = viewerData.viewer->GetRenderMask();

				if (viewerRenderMask & renderMask)
				{
					if (viewerData.depthPrepass)
						viewerData.depthPrepass->InvalidateElements();

					viewerData.forwardPass->InvalidateElements();
				}
			}
		});

		renderableData->onMaterialInvalidated.Connect(instancedRenderable->OnMaterialInvalidated, [this](InstancedRenderable* instancedRenderable, std::size_t materialIndex, const std::shared_ptr<Material>& newMaterial)
		{
			if (newMaterial)
			{
				for (auto& viewerData : m_viewerPool)
				{
					if (viewerData.depthPrepass)
						viewerData.depthPrepass->RegisterMaterial(*newMaterial);

					viewerData.forwardPass->RegisterMaterial(*newMaterial);
				}
			}

			const auto& prevMaterial = instancedRenderable->GetMaterial(materialIndex);
			if (prevMaterial)
			{
				for (auto& viewerData : m_viewerPool)
				{
					if (viewerData.depthPrepass)
						viewerData.depthPrepass->UnregisterMaterial(*prevMaterial);

					viewerData.forwardPass->UnregisterMaterial(*prevMaterial);
				}
			}
		});

		std::size_t matCount = instancedRenderable->GetMaterialCount();
		for (std::size_t i = 0; i < matCount; ++i)
		{
			if (Material* mat = instancedRenderable->GetMaterial(i).get())
			{
				for (auto& viewerData : m_viewerPool)
				{
					if (viewerData.depthPrepass)
						viewerData.depthPrepass->RegisterMaterial(*mat);

					viewerData.forwardPass->RegisterMaterial(*mat);
				}
			}
		}

		return renderableIndex;
	}

	std::size_t ForwardFramePipeline::RegisterViewer(AbstractViewer* viewerInstance, Int32 renderOrder)
	{
		std::size_t viewerIndex;
		auto& viewerData = *m_viewerPool.Allocate(viewerIndex);
		viewerData.renderOrder = renderOrder;
		viewerData.debugDrawPass = std::make_unique<DebugDrawPipelinePass>(*this, viewerInstance);
		viewerData.depthPrepass = std::make_unique<DepthPipelinePass>(*this, viewerInstance);
		viewerData.forwardPass = std::make_unique<ForwardPipelinePass>(*this, viewerInstance);
		viewerData.viewer = viewerInstance;

		m_invalidatedViewerInstances.UnboundedSet(viewerIndex);
		m_rebuildFrameGraph = true;

		return viewerIndex;
	}

	std::size_t ForwardFramePipeline::RegisterWorldInstance(WorldInstancePtr worldInstance)
	{
		std::size_t worldInstanceIndex;
		m_worldInstances.Allocate(worldInstanceIndex, std::move(worldInstance));

		m_invalidatedWorldInstances.UnboundedSet(worldInstanceIndex);

		return worldInstanceIndex;
	}

	void ForwardFramePipeline::Render(RenderFrame& renderFrame)
	{
		m_currentRenderFrame = &renderFrame;

		Graphics* graphics = Graphics::Instance();

		// Destroy world instances at the end of the frame
		for (std::size_t worldInstanceIndex = m_removedWorldInstances.FindFirst(); worldInstanceIndex != m_removedWorldInstances.npos; worldInstanceIndex = m_removedWorldInstances.FindNext(worldInstanceIndex))
		{
			renderFrame.PushForRelease(*m_worldInstances.RetrieveFromIndex(worldInstanceIndex));
			m_worldInstances.Free(worldInstanceIndex);
		}
		m_removedWorldInstances.Clear();

		if (m_rebuildFrameGraph)
		{
			renderFrame.PushForRelease(std::move(m_bakedFrameGraph));
			m_bakedFrameGraph = BuildFrameGraph();
		}

		// Update UBOs and materials
		UploadPool& uploadPool = renderFrame.GetUploadPool();

		renderFrame.Execute([&](CommandBufferBuilder& builder)
		{
			builder.BeginDebugRegion("CPU to GPU transfers", Color::Yellow);
			{
				builder.PreTransferBarrier();

				OnTransfer(this, renderFrame, builder);

				for (std::size_t viewerIndex = m_invalidatedViewerInstances.FindFirst(); viewerIndex != m_invalidatedViewerInstances.npos; viewerIndex = m_invalidatedViewerInstances.FindNext(viewerIndex))
				{
					ViewerData* viewerData = m_viewerPool.RetrieveFromIndex(viewerIndex);
					viewerData->viewer->GetViewerInstance().UpdateBuffers(uploadPool, builder);
				}
				m_invalidatedViewerInstances.Reset();

				for (std::size_t worldInstanceIndex = m_invalidatedWorldInstances.FindFirst(); worldInstanceIndex != m_invalidatedWorldInstances.npos; worldInstanceIndex = m_invalidatedWorldInstances.FindNext(worldInstanceIndex))
				{
					WorldInstancePtr& worldInstance = *m_worldInstances.RetrieveFromIndex(worldInstanceIndex);
					worldInstance->UpdateBuffers(uploadPool, builder);
				}
				m_invalidatedWorldInstances.Reset();

				for (MaterialPass* materialPass : m_invalidatedMaterialPasses)
					materialPass->Update(renderFrame, builder);

				m_invalidatedMaterialPasses.clear();

				builder.PostTransferBarrier();
			}
			builder.EndDebugRegion();
		}, QueueType::Transfer);

		auto CombineHash = [](std::size_t currentHash, std::size_t newHash)
		{
			return currentHash * 23 + newHash;
		};

		// Render queues handling
		for (auto& viewerData : m_viewerPool)
		{
			UInt32 renderMask = viewerData.viewer->GetRenderMask();

			// Frustum culling
			const Matrix4f& viewProjMatrix = viewerData.viewer->GetViewerInstance().GetViewProjMatrix();

			Frustumf frustum = Frustumf::Extract(viewProjMatrix);

			std::size_t visibilityHash = 5U;

			m_visibleRenderables.clear();
			for (const RenderableData& renderableData : m_renderablePool)
			{
				if ((renderMask & renderableData.renderMask) == 0)
					continue;

				WorldInstancePtr& worldInstance = *m_worldInstances.RetrieveFromIndex(renderableData.worldInstanceIndex);

				// Get global AABB
				BoundingVolumef boundingVolume(renderableData.renderable->GetAABB());
				boundingVolume.Update(worldInstance->GetWorldMatrix());

				if (!frustum.Contains(boundingVolume))
					continue;

				auto& visibleRenderable = m_visibleRenderables.emplace_back();
				visibleRenderable.instancedRenderable = renderableData.renderable;
				visibleRenderable.scissorBox = renderableData.scissorBox;
				visibleRenderable.worldInstance = worldInstance.get();

				visibilityHash = CombineHash(visibilityHash, std::hash<const void*>()(&renderableData));
			}

			// Lights update don't trigger a rebuild of the depth pre-pass
			std::size_t depthVisibilityHash = visibilityHash;

			m_visibleLights.clear();
			for (const LightData& lightData : m_lightPool)
			{
				const BoundingVolumef& boundingVolume = lightData.light->GetBoundingVolume();

				// TODO: Use more precise tests for point lights (frustum/sphere is cheap)
				if (renderMask & lightData.renderMask && frustum.Contains(boundingVolume))
				{
					m_visibleLights.push_back(lightData.light.get());
					visibilityHash = CombineHash(visibilityHash, std::hash<const void*>()(lightData.light.get()));
				}
			}

			if (viewerData.depthPrepass)
				viewerData.depthPrepass->Prepare(renderFrame, frustum, m_visibleRenderables, depthVisibilityHash);

			viewerData.forwardPass->Prepare(renderFrame, frustum, m_visibleRenderables, m_visibleLights, visibilityHash);

			viewerData.debugDrawPass->Prepare(renderFrame);
		}

		if (m_bakedFrameGraph.Resize(renderFrame))
		{
			const std::shared_ptr<TextureSampler>& sampler = graphics->GetSamplerCache().Get({});
			for (auto& viewerData : m_viewerPool)
			{
				if (viewerData.blitShaderBinding)
					renderFrame.PushForRelease(std::move(viewerData.blitShaderBinding));

				viewerData.blitShaderBinding = graphics->GetBlitPipelineLayout()->AllocateShaderBinding(0);
				viewerData.blitShaderBinding->Update({
					{
						0,
						ShaderBinding::TextureBinding {
							m_bakedFrameGraph.GetAttachmentTexture(viewerData.debugColorAttachment).get(),
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
						ShaderBinding::TextureBinding {
							m_bakedFrameGraph.GetAttachmentTexture(renderTargetData.finalAttachment).get(),
							sampler.get()
						}
					}
				});
			}
		}

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
				clearValues[0].color = Color::Black;
				clearValues[1].depth = 1.f;
				clearValues[1].stencil = 0;

				builder.BeginRenderPass(renderTarget.GetFramebuffer(renderFrame.GetFramebufferIndex()), renderTarget.GetRenderPass(), renderRegion, { clearValues[0], clearValues[1] });
				{
					builder.BeginDebugRegion("Main window rendering", Color::Green);
					{
						builder.SetScissor(renderRegion);
						builder.SetViewport(renderRegion);
						builder.BindPipeline(*graphics->GetBlitPipeline(false));

						builder.BindShaderBinding(0, *data.blitShaderBinding);
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
	}

	void ForwardFramePipeline::UnregisterMaterialPass(MaterialPass* materialPass)
	{
		auto it = m_activeMaterialPasses.find(materialPass);
		assert(it != m_activeMaterialPasses.end());

		MaterialPassData& materialData = it->second;
		assert(materialData.usedCount > 0);
		if (--materialData.usedCount == 0)
			m_activeMaterialPasses.erase(materialPass);
	}

	void ForwardFramePipeline::UnregisterRenderable(std::size_t renderableIndex)
	{
		RenderableData& renderable = *m_renderablePool.RetrieveFromIndex(renderableIndex);

		std::size_t matCount = renderable.renderable->GetMaterialCount();
		for (std::size_t i = 0; i < matCount; ++i)
		{
			for (auto& viewerData : m_viewerPool)
			{
				const auto& material = renderable.renderable->GetMaterial(i);
				if (viewerData.depthPrepass)
					viewerData.depthPrepass->UnregisterMaterial(*material);

				viewerData.forwardPass->UnregisterMaterial(*material);
			}
		}

		m_renderablePool.Free(renderableIndex);
	}

	void ForwardFramePipeline::UnregisterViewer(std::size_t viewerIndex)
	{
		m_viewerPool.Free(viewerIndex);
		m_invalidatedViewerInstances.Reset(viewerIndex);
		m_rebuildFrameGraph = true;
	}

	void ForwardFramePipeline::UnregisterWorldInstance(std::size_t worldInstance)
	{
		// Defer world instance release
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
			UInt32 viewerRenderMask = viewerData.viewer->GetRenderMask();

			if (viewerRenderMask & renderableData->renderMask)
			{
				if (viewerData.depthPrepass)
					viewerData.depthPrepass->InvalidateElements();

				viewerData.forwardPass->InvalidateElements();
			}
		}
	}

	void ForwardFramePipeline::UpdateViewerRenderMask(std::size_t viewerIndex, Int32 renderOrder)
	{
		ViewerData* viewerData = m_viewerPool.RetrieveFromIndex(viewerIndex);
		if (viewerData->renderOrder != renderOrder)
		{
			viewerData->renderOrder = renderOrder;
			m_rebuildFrameGraph = true;
		}
	}

	BakedFrameGraph ForwardFramePipeline::BuildFrameGraph()
	{
		FrameGraph frameGraph;

		for (auto& viewerData : m_viewerPool)
		{
			viewerData.forwardColorAttachment = frameGraph.AddAttachment({
				"Forward output",
				PixelFormat::RGBA8
			});
			
			viewerData.debugColorAttachment = frameGraph.AddAttachmentProxy("Debug draw output", viewerData.forwardColorAttachment);

			viewerData.depthStencilAttachment = frameGraph.AddAttachment({
				"Depth-stencil buffer",
				Graphics::Instance()->GetPreferredDepthStencilFormat()
			});

			if (viewerData.depthPrepass)
				viewerData.depthPrepass->RegisterToFrameGraph(frameGraph, viewerData.depthStencilAttachment);

			viewerData.forwardPass->RegisterToFrameGraph(frameGraph, viewerData.forwardColorAttachment, viewerData.depthStencilAttachment, viewerData.depthPrepass != nullptr);

			viewerData.debugDrawPass->RegisterToFrameGraph(frameGraph, viewerData.forwardColorAttachment, viewerData.debugColorAttachment);
		}

		using ViewerPair = std::pair<const RenderTarget*, const ViewerData*>;

		StackArray<ViewerPair> viewers = NazaraStackArray(ViewerPair, m_viewerPool.size());
		auto viewerIt = viewers.begin();

		for (auto& viewerData : m_viewerPool)
		{
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
				mergePass.AddInput(viewerData->debugColorAttachment);

			mergePass.AddOutput(renderTargetData.finalAttachment);
			mergePass.SetClearColor(0, Color::Black);

			mergePass.SetCommandCallback([&targetViewers](CommandBufferBuilder& builder, const Nz::FramePassEnvironment& env)
			{
				builder.SetScissor(env.renderRect);
				builder.SetViewport(env.renderRect);

				Graphics* graphics = Graphics::Instance();
				builder.BindPipeline(*graphics->GetBlitPipeline(false));

				bool first = true;

				for (const ViewerData* viewerData : targetViewers)
				{
					const ShaderBindingPtr& blitShaderBinding = viewerData->blitShaderBinding;

					builder.BindShaderBinding(0, *blitShaderBinding);
					builder.Draw(3);

					if (first)
					{
						builder.BindPipeline(*graphics->GetBlitPipeline(true));
						first = false;
					}
				}
			});

			frameGraph.AddBackbufferOutput(renderTargetData.finalAttachment);
		}

		return frameGraph.Bake();
	}
}
