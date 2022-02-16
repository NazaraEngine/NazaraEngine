// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardFramePipeline.hpp>
#include <Nazara/Core/StackArray.hpp>
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
#include <array>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ForwardFramePipeline::ForwardFramePipeline() :
	m_rebuildFrameGraph(true)
	{
		auto& passRegistry = Graphics::Instance()->GetMaterialPassRegistry();
		m_forwardPassIndex = passRegistry.GetPassIndex("ForwardPass");
	}

	ForwardFramePipeline::~ForwardFramePipeline()
	{
		// Force viewer passes to unregister their materials
		m_viewers.clear();
	}

	void ForwardFramePipeline::InvalidateViewer(AbstractViewer* viewerInstance)
	{
		m_invalidatedViewerInstances.insert(viewerInstance);
	}

	void ForwardFramePipeline::InvalidateWorldInstance(WorldInstance* worldInstance)
	{
		m_invalidatedWorldInstances.insert(worldInstance);
	}

	void ForwardFramePipeline::RegisterInstancedDrawable(WorldInstancePtr worldInstance, const InstancedRenderable* instancedRenderable, UInt32 renderMask)
	{
		m_removedWorldInstances.erase(worldInstance);

		auto& renderableMap = m_renderables[worldInstance];
		if (renderableMap.empty())
			InvalidateWorldInstance(worldInstance.get());

		if (auto it = renderableMap.find(instancedRenderable); it == renderableMap.end())
		{
			auto& renderableData = renderableMap.emplace(instancedRenderable, RenderableData{}).first->second;
			renderableData.renderMask = renderMask;
			
			renderableData.onElementInvalidated.Connect(instancedRenderable->OnElementInvalidated, [=](InstancedRenderable* /*instancedRenderable*/)
			{
				// TODO: Invalidate only relevant viewers and passes
				for (auto&& [viewer, viewerData] : m_viewers)
				{
					UInt32 viewerRenderMask = viewer->GetRenderMask();

					if (viewerRenderMask & renderMask)
					{
						if (viewerData.depthPrepass)
							viewerData.depthPrepass->ForceInvalidation();

						viewerData.forwardPass->ForceInvalidation();
					}
				}
			});

			renderableData.onMaterialInvalidated.Connect(instancedRenderable->OnMaterialInvalidated, [this](InstancedRenderable* instancedRenderable, std::size_t materialIndex, const std::shared_ptr<Material>& newMaterial)
			{
				if (newMaterial)
				{
					for (auto&& [viewer, viewerData] : m_viewers)
					{
						if (viewerData.depthPrepass)
							viewerData.depthPrepass->RegisterMaterial(*newMaterial);

						viewerData.forwardPass->RegisterMaterial(*newMaterial);
					}
				}

				const auto& prevMaterial = instancedRenderable->GetMaterial(materialIndex);
				if (prevMaterial)
				{
					for (auto&& [viewer, viewerData] : m_viewers)
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
					for (auto&& [viewer, viewerData] : m_viewers)
					{
						if (viewerData.depthPrepass)
							viewerData.depthPrepass->RegisterMaterial(*mat);

						viewerData.forwardPass->RegisterMaterial(*mat);
					}
				}
			}
		}
	}

	void ForwardFramePipeline::RegisterLight(std::shared_ptr<Light> light, UInt32 renderMask)
	{
		auto& lightData = m_lights[light.get()];
		lightData.light = std::move(light);
		lightData.renderMask = renderMask;
		lightData.onLightInvalidated.Connect(lightData.light->OnLightDataInvalided, [=](Light*)
		{
			//TODO: Switch lights to storage buffers so they can all be part of GPU memory
			for (auto&& [viewer, viewerData] : m_viewers)
			{
				UInt32 viewerRenderMask = viewer->GetRenderMask();

				if (viewerRenderMask & renderMask)
					viewerData.forwardPass->ForceInvalidation();
			}
		});
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

	void ForwardFramePipeline::RegisterViewer(AbstractViewer* viewerInstance, Int32 renderOrder)
	{
		auto& viewerData = m_viewers.emplace(viewerInstance, ViewerData{}).first->second;
		viewerData.renderOrder = renderOrder;
		viewerData.depthPrepass = std::make_unique<DepthPipelinePass>(*this, viewerInstance);
		viewerData.forwardPass = std::make_unique<ForwardPipelinePass>(*this, viewerInstance);

		m_invalidatedViewerInstances.insert(viewerInstance);
		m_rebuildFrameGraph = true;
	}

	void ForwardFramePipeline::Render(RenderFrame& renderFrame)
	{
		m_currentRenderFrame = &renderFrame;

		Graphics* graphics = Graphics::Instance();

		renderFrame.PushForRelease(std::move(m_removedWorldInstances));
		m_removedWorldInstances.clear();

		if (m_rebuildFrameGraph)
		{
			renderFrame.PushForRelease(std::move(m_bakedFrameGraph));
			m_bakedFrameGraph = BuildFrameGraph();
		}

		// Update UBOs and materials
		UploadPool& uploadPool = renderFrame.GetUploadPool();

		renderFrame.Execute([&](CommandBufferBuilder& builder)
		{
			builder.BeginDebugRegion("UBO Update", Color::Yellow);
			{
				builder.PreTransferBarrier();

				for (AbstractViewer* viewer : m_invalidatedViewerInstances)
					viewer->GetViewerInstance().UpdateBuffers(uploadPool, builder);

				m_invalidatedViewerInstances.clear();

				for (WorldInstance* worldInstance : m_invalidatedWorldInstances)
					worldInstance->UpdateBuffers(uploadPool, builder);

				m_invalidatedWorldInstances.clear();

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
		for (auto&& [viewer, data] : m_viewers)
		{
			auto& viewerData = data;

			UInt32 renderMask = viewer->GetRenderMask();

			// Frustum culling
			const Matrix4f& viewProjMatrix = viewer->GetViewerInstance().GetViewProjMatrix();

			Frustumf frustum = Frustumf::Extract(viewProjMatrix);

			std::size_t visibilityHash = 5U;

			m_visibleRenderables.clear();
			for (const auto& [worldInstance, renderables] : m_renderables)
			{
				bool isInstanceVisible = false;

				for (const auto& [renderable, renderableData] : renderables)
				{
					if ((renderMask & renderableData.renderMask) == 0)
						continue;

					// Get global AABB
					BoundingVolumef boundingVolume(renderable->GetAABB());
					boundingVolume.Update(worldInstance->GetWorldMatrix());

					if (!frustum.Contains(boundingVolume))
						continue;

					auto& visibleRenderable = m_visibleRenderables.emplace_back();
					visibleRenderable.instancedRenderable = renderable;
					visibleRenderable.worldInstance = worldInstance.get();

					isInstanceVisible = true;
					visibilityHash = CombineHash(visibilityHash, std::hash<const void*>()(renderable));
				}

				if (isInstanceVisible)
					visibilityHash = CombineHash(visibilityHash, std::hash<const void*>()(worldInstance.get()));
			}

			// Lights update don't trigger a rebuild of the depth pre-pass
			std::size_t depthVisibilityHash = visibilityHash;

			m_visibleLights.clear();
			for (auto&& [light, lightData] : m_lights)
			{
				const BoundingVolumef& boundingVolume = light->GetBoundingVolume();

				// TODO: Use more precise tests for point lights (frustum/sphere is cheap)
				if (renderMask & lightData.renderMask && frustum.Contains(boundingVolume))
				{
					m_visibleLights.push_back(light);
					visibilityHash = CombineHash(visibilityHash, std::hash<const void*>()(light));
				}
			}

			if (viewerData.depthPrepass)
				viewerData.depthPrepass->Prepare(renderFrame, frustum, m_visibleRenderables, depthVisibilityHash);

			viewerData.forwardPass->Prepare(renderFrame, frustum, m_visibleRenderables, m_visibleLights, visibilityHash);
		}

		if (m_bakedFrameGraph.Resize(renderFrame))
		{
			const std::shared_ptr<TextureSampler>& sampler = graphics->GetSamplerCache().Get({});
			for (auto&& [_, viewerData] : m_viewers)
			{
				if (viewerData.blitShaderBinding)
					renderFrame.PushForRelease(std::move(viewerData.blitShaderBinding));

				viewerData.blitShaderBinding = graphics->GetBlitPipelineLayout()->AllocateShaderBinding(0);
				viewerData.blitShaderBinding->Update({
					{
						0,
						ShaderBinding::TextureBinding {
							m_bakedFrameGraph.GetAttachmentTexture(viewerData.colorAttachment).get(),
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
						builder.BindVertexBuffer(0, *graphics->GetFullscreenVertexBuffer());

						builder.BindShaderBinding(0, *data.blitShaderBinding);
						builder.Draw(3);
					}
					builder.EndDebugRegion();
				}
				builder.EndRenderPass();

			}, QueueType::Graphics);
		}
	}

	void ForwardFramePipeline::UnregisterInstancedDrawable(const WorldInstancePtr& worldInstance, const InstancedRenderable* instancedRenderable)
	{
		auto instanceIt = m_renderables.find(worldInstance);
		if (instanceIt == m_renderables.end())
			return;

		auto& instancedRenderables = instanceIt->second;

		auto renderableIt = instancedRenderables.find(instancedRenderable);
		if (renderableIt == instancedRenderables.end())
			return;

		if (instancedRenderables.size() > 1)
			instancedRenderables.erase(renderableIt);
		else
		{
			m_removedWorldInstances.insert(worldInstance);
			m_renderables.erase(instanceIt);
		}

		std::size_t matCount = instancedRenderable->GetMaterialCount();
		for (std::size_t i = 0; i < matCount; ++i)
		{
			for (auto&& [viewer, viewerData] : m_viewers)
			{
				const auto& material = instancedRenderable->GetMaterial(i);
				if (viewerData.depthPrepass)
					viewerData.depthPrepass->UnregisterMaterial(*material);

				viewerData.forwardPass->UnregisterMaterial(*material);
			}
		}
	}

	void ForwardFramePipeline::UnregisterLight(Light* light)
	{
		m_lights.erase(light);
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

	void ForwardFramePipeline::UnregisterViewer(AbstractViewer* viewerInstance)
	{
		m_viewers.erase(viewerInstance);
		m_rebuildFrameGraph = true;
	}

	BakedFrameGraph ForwardFramePipeline::BuildFrameGraph()
	{
		FrameGraph frameGraph;

		for (auto&& [viewer, viewerData] : m_viewers)
		{
			viewerData.colorAttachment = frameGraph.AddAttachment({
				"Color",
				PixelFormat::RGBA8
			});

			viewerData.depthStencilAttachment = frameGraph.AddAttachment({
				"Depth-stencil buffer",
				Graphics::Instance()->GetPreferredDepthStencilFormat()
			});
		}

		for (auto&& [viewer, data] : m_viewers)
		{
			auto& viewerData = data;

			if (viewerData.depthPrepass)
				viewerData.depthPrepass->RegisterToFrameGraph(frameGraph, viewerData.depthStencilAttachment);

			viewerData.forwardPass->RegisterToFrameGraph(frameGraph, viewerData.colorAttachment, viewerData.depthStencilAttachment);
		}

		using ViewerPair = std::pair<const RenderTarget*, const ViewerData*>;

		StackArray<ViewerPair> viewers = NazaraStackArray(ViewerPair, m_viewers.size());
		auto viewerIt = viewers.begin();

		for (auto&& [viewer, viewerData] : m_viewers)
		{
			const RenderTarget& renderTarget = viewer->GetRenderTarget();
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

			FramePass& forwardPass = frameGraph.AddPass("Merge pass");

			renderTargetData.finalAttachment = frameGraph.AddAttachment({
				"Viewer output",
				PixelFormat::RGBA8
			});

			for (const ViewerData* viewerData : targetViewers)
				forwardPass.AddInput(viewerData->colorAttachment);

			forwardPass.AddOutput(renderTargetData.finalAttachment);
			forwardPass.SetClearColor(0, Color::Black);

			forwardPass.SetCommandCallback([&targetViewers](CommandBufferBuilder& builder, const Recti& renderRect)
			{
				builder.SetScissor(renderRect);
				builder.SetViewport(renderRect);

				Graphics* graphics = Graphics::Instance();
				builder.BindPipeline(*graphics->GetBlitPipeline(false));
				builder.BindVertexBuffer(0, *graphics->GetFullscreenVertexBuffer());

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
