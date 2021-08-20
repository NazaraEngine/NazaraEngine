// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardFramePipeline.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/SubmeshRenderer.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
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
		m_depthPassIndex = passRegistry.GetPassIndex("DepthPass");
		m_forwardPassIndex = passRegistry.GetPassIndex("ForwardPass");

		m_elementRenderers.resize(1);
		m_elementRenderers[UnderlyingCast(BasicRenderElement::Submesh)] = std::make_unique<SubmeshRenderer>();
	}

	void ForwardFramePipeline::InvalidateViewer(AbstractViewer* viewerInstance)
	{
		m_invalidatedViewerInstances.insert(viewerInstance);
	}

	void ForwardFramePipeline::InvalidateWorldInstance(WorldInstance* worldInstance)
	{
		m_invalidatedWorldInstances.insert(worldInstance);
	}

	void ForwardFramePipeline::RegisterInstancedDrawable(WorldInstancePtr worldInstance, const InstancedRenderable* instancedRenderable)
	{
		m_removedWorldInstances.erase(worldInstance);

		auto& renderableMap = m_renderables[worldInstance];
		if (renderableMap.empty())
			InvalidateWorldInstance(worldInstance.get());

		if (auto it = renderableMap.find(instancedRenderable); it == renderableMap.end())
		{
			auto& renderableData = renderableMap.emplace(instancedRenderable, RenderableData{}).first->second;
			renderableData.onMaterialInvalidated.Connect(instancedRenderable->OnMaterialInvalidated, [this](InstancedRenderable* instancedRenderable, std::size_t materialIndex, const std::shared_ptr<Material>& newMaterial)
			{
				if (newMaterial)
				{
					if (MaterialPass* pass = newMaterial->GetPass(m_depthPassIndex))
						RegisterMaterialPass(pass);

					if (MaterialPass* pass = newMaterial->GetPass(m_forwardPassIndex))
						RegisterMaterialPass(pass);
				}

				const auto& prevMaterial = instancedRenderable->GetMaterial(materialIndex);
				if (prevMaterial)
				{
					if (MaterialPass* pass = prevMaterial->GetPass(m_depthPassIndex))
						UnregisterMaterialPass(pass);

					if (MaterialPass* pass = prevMaterial->GetPass(m_forwardPassIndex))
						UnregisterMaterialPass(pass);
				}

				for (auto&& [viewer, viewerData] : m_viewers)
				{
					viewerData.rebuildDepthPrepass = true;
					viewerData.rebuildForwardPass = true;
				}
			});

			std::size_t matCount = instancedRenderable->GetMaterialCount();
			for (std::size_t i = 0; i < matCount; ++i)
			{
				if (Material* mat = instancedRenderable->GetMaterial(i).get())
				{
					if (MaterialPass* pass = mat->GetPass(m_depthPassIndex))
						RegisterMaterialPass(pass);

					if (MaterialPass* pass = mat->GetPass(m_forwardPassIndex))
						RegisterMaterialPass(pass);
				}
			}

			for (auto&& [viewer, viewerData] : m_viewers)
			{
				viewerData.rebuildDepthPrepass = true;
				viewerData.rebuildForwardPass = true;
			}
		}
	}

	void ForwardFramePipeline::RegisterViewer(AbstractViewer* viewerInstance)
	{
		m_viewers.emplace(viewerInstance, ViewerData{});
		m_invalidatedViewerInstances.insert(viewerInstance);
		m_rebuildFrameGraph = true;
	}

	void ForwardFramePipeline::Render(RenderFrame& renderFrame)
	{
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

				for (MaterialPass* material : m_invalidatedMaterials)
				{
					if (material->Update(renderFrame, builder))
					{
						for (auto&& [viewer, viewerData] : m_viewers)
						{
							viewerData.rebuildDepthPrepass = true;
							viewerData.rebuildForwardPass = true;
						}
					}
				}
				m_invalidatedMaterials.clear();

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

			// Frustum culling
			const Matrix4f& viewProjMatrix = viewer->GetViewerInstance().GetViewProjMatrix();

			Frustumf frustum;
			frustum.Extract(viewProjMatrix);

			std::size_t visibilityHash = 5U;

			m_visibleRenderables.clear();
			for (const auto& [worldInstance, renderables] : m_renderables)
			{
				bool isInstanceVisible = false;

				for (const auto& [renderable, renderableData] : renderables)
				{
					// Get global AABB
					BoundingVolumef boundingVolume(renderable->GetAABB());
					boundingVolume.Update(worldInstance->GetWorldMatrix());

					if (!frustum.Contains(boundingVolume.aabb))
						continue;

					auto& renderableData = m_visibleRenderables.emplace_back();
					renderableData.instancedRenderable = renderable;
					renderableData.worldInstance = worldInstance.get();

					isInstanceVisible = true;
					visibilityHash = CombineHash(visibilityHash, std::hash<const void*>()(renderable));
				}

				if (isInstanceVisible)
					visibilityHash = CombineHash(visibilityHash, std::hash<const void*>()(worldInstance.get()));
			}

			if (viewerData.visibilityHash != visibilityHash)
			{
				viewerData.rebuildDepthPrepass = true;
				viewerData.rebuildForwardPass = true;

				viewerData.visibilityHash = visibilityHash;
			}

			if (viewerData.rebuildDepthPrepass)
			{
				viewerData.depthPrepassRenderElements.clear();

				for (const auto& renderableData : m_visibleRenderables)
					renderableData.instancedRenderable->BuildElement(m_depthPassIndex, *renderableData.worldInstance, viewerData.depthPrepassRenderElements);

				viewerData.depthPrepassRegistry.Clear();
				viewerData.depthPrepassRenderQueue.Clear();

				for (const auto& renderElement : viewerData.depthPrepassRenderElements)
				{
					renderElement->Register(viewerData.depthPrepassRegistry);
					viewerData.depthPrepassRenderQueue.Insert(renderElement.get());
				}
			}

			viewerData.depthPrepassRenderQueue.Sort([&](const RenderElement* element)
			{
				return element->ComputeSortingScore(viewerData.depthPrepassRegistry);
			});

			if (viewerData.rebuildForwardPass)
			{
				viewerData.forwardRenderElements.clear();

				for (const auto& renderableData : m_visibleRenderables)
					renderableData.instancedRenderable->BuildElement(m_forwardPassIndex, *renderableData.worldInstance, viewerData.forwardRenderElements);

				viewerData.forwardRegistry.Clear();
				viewerData.forwardRenderQueue.Clear();
				for (const auto& renderElement : viewerData.forwardRenderElements)
				{
					renderElement->Register(viewerData.forwardRegistry);
					viewerData.forwardRenderQueue.Insert(renderElement.get());
				}
			}

			viewerData.forwardRenderQueue.Sort([&](const RenderElement* element)
			{
				return element->ComputeSortingScore(viewerData.forwardRegistry);
			});
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
		}

		m_bakedFrameGraph.Execute(renderFrame);
		m_rebuildFrameGraph = false;

		const Vector2ui& frameSize = renderFrame.GetSize();
		for (auto&& [viewer, viewerData] : m_viewers)
		{
			viewerData.rebuildForwardPass = false;
			viewerData.rebuildDepthPrepass = false;

			const RenderTarget& renderTarget = viewer->GetRenderTarget();
			Recti renderRegion(0, 0, frameSize.x, frameSize.y);
			const ShaderBindingPtr& blitShaderBinding = viewerData.blitShaderBinding;
			const std::shared_ptr<Texture>& sourceTexture = m_bakedFrameGraph.GetAttachmentTexture(viewerData.colorAttachment);

			renderFrame.Execute([&](CommandBufferBuilder& builder)
			{
				builder.TextureBarrier(PipelineStage::ColorOutput, PipelineStage::FragmentShader, MemoryAccess::ColorWrite, MemoryAccess::ShaderRead, TextureLayout::ColorOutput, TextureLayout::ColorInput, *sourceTexture);

				std::array<CommandBufferBuilder::ClearValues, 2> clearValues;
				clearValues[0].color = Color::Black;
				clearValues[1].depth = 1.f;
				clearValues[1].stencil = 0;

				builder.BeginDebugRegion("Main window rendering", Color::Green);
				{
					builder.BeginRenderPass(renderTarget.GetFramebuffer(renderFrame.GetFramebufferIndex()), renderTarget.GetRenderPass(), renderRegion, { clearValues[0], clearValues[1] });
					{
						builder.SetScissor(renderRegion);
						builder.SetViewport(renderRegion);

						builder.BindPipeline(*graphics->GetBlitPipeline());
						builder.BindVertexBuffer(0, *graphics->GetFullscreenVertexBuffer());
						builder.BindShaderBinding(0, *blitShaderBinding);

						builder.Draw(3);
					}
					builder.EndRenderPass();
				}
				builder.EndDebugRegion();

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
			m_renderables.erase(instanceIt);;
		}

		std::size_t matCount = instancedRenderable->GetMaterialCount();
		for (std::size_t i = 0; i < matCount; ++i)
		{
			if (MaterialPass* pass = instancedRenderable->GetMaterial(i)->GetPass(m_depthPassIndex))
				UnregisterMaterialPass(pass);

			if (MaterialPass* pass = instancedRenderable->GetMaterial(i)->GetPass(m_forwardPassIndex))
				UnregisterMaterialPass(pass);
		}

		for (auto&& [viewer, viewerData] : m_viewers)
		{
			viewerData.rebuildDepthPrepass = true;
			viewerData.rebuildForwardPass = true;
		}
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

			FramePass& depthPrepass = frameGraph.AddPass("Depth pre-pass");
			depthPrepass.SetDepthStencilOutput(viewerData.depthStencilAttachment);
			depthPrepass.SetDepthStencilClear(1.f, 0);
			
			depthPrepass.SetExecutionCallback([&]()
			{
				if (viewerData.rebuildDepthPrepass)
					return FramePassExecution::UpdateAndExecute;
				else
					return FramePassExecution::Execute;
			});
			
			depthPrepass.SetCommandCallback([this, viewer = viewer, &viewerData](CommandBufferBuilder& builder, const Recti& /*renderRect*/)
			{
				Recti viewport = viewer->GetViewport();

				builder.SetScissor(viewport);
				builder.SetViewport(viewport);

				builder.BindShaderBinding(Graphics::ViewerBindingSet, viewer->GetViewerInstance().GetShaderBinding());

				ProcessRenderQueue(builder, viewerData.depthPrepassRenderQueue);
			});

			FramePass& forwardPass = frameGraph.AddPass("Forward pass");
			forwardPass.AddOutput(viewerData.colorAttachment);
			forwardPass.SetDepthStencilInput(viewerData.depthStencilAttachment);
			//forwardPass.SetDepthStencilOutput(viewerData.depthStencilAttachment);

			forwardPass.SetClearColor(0, Color::Black);
			forwardPass.SetDepthStencilClear(1.f, 0);

			forwardPass.SetExecutionCallback([&]()
			{
				if (viewerData.rebuildForwardPass)
					return FramePassExecution::UpdateAndExecute;
				else
					return FramePassExecution::Execute;
			});

			forwardPass.SetCommandCallback([this, viewer = viewer, &viewerData](CommandBufferBuilder& builder, const Recti& /*renderRect*/)
			{
				Recti viewport = viewer->GetViewport();

				builder.SetScissor(viewport);
				builder.SetViewport(viewport);

				builder.BindShaderBinding(Graphics::ViewerBindingSet, viewer->GetViewerInstance().GetShaderBinding());

				ProcessRenderQueue(builder, viewerData.forwardRenderQueue);
			});
		}

		//FIXME: This doesn't handle multiple window viewers
		for (auto&& [viewer, viewerData] : m_viewers)
			frameGraph.SetBackbufferOutput(viewerData.colorAttachment);

		return frameGraph.Bake();
	}

	void ForwardFramePipeline::RegisterMaterialPass(MaterialPass* material)
	{
		auto it = m_materials.find(material);
		if (it == m_materials.end())
		{
			it = m_materials.emplace(material, MaterialData{}).first;
			it->second.onMaterialInvalided.Connect(material->OnMaterialInvalidated, [this, material](const MaterialPass* /*material*/)
			{
				m_invalidatedMaterials.insert(material);
			});

			m_invalidatedMaterials.insert(material);
		}

		it->second.usedCount++;
	}

	void ForwardFramePipeline::ProcessRenderQueue(CommandBufferBuilder& builder, const RenderQueue<RenderElement*>& renderQueue)
	{
		if (renderQueue.empty())
			return;

		auto it = renderQueue.begin();
		auto itEnd = renderQueue.end();
		while (it != itEnd)
		{
			const RenderElement* element = *it;
			UInt8 elementType = element->GetElementType();

			const Pointer<RenderElement>* first = it;

			++it;
			while (it != itEnd && (*it)->GetElementType() == elementType)
				++it;

			std::size_t count = it - first;
			
			if (elementType >= m_elementRenderers.size() || !m_elementRenderers[elementType])
				continue;

			ElementRenderer& elementRenderer = *m_elementRenderers[elementType];
			elementRenderer.Render(builder, first, count);
		}
	}

	void ForwardFramePipeline::UnregisterMaterialPass(MaterialPass* material)
	{
		auto it = m_materials.find(material);
		assert(it != m_materials.end());

		MaterialData& materialData = it->second;
		assert(materialData.usedCount > 0);
		if (--materialData.usedCount == 0)
			m_materials.erase(material);
	}
}
