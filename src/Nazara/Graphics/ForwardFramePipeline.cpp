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
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/SpriteChainRenderer.hpp>
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

		m_elementRenderers.resize(BasicRenderElementCount);
		m_elementRenderers[UnderlyingCast(BasicRenderElement::SpriteChain)] = std::make_unique<SpriteChainRenderer>(*Graphics::Instance()->GetRenderDevice());
		m_elementRenderers[UnderlyingCast(BasicRenderElement::Submesh)] = std::make_unique<SubmeshRenderer>();

		auto lightOffset = PredefinedLightData::GetOffsets();

		m_lightDataBuffer = Graphics::Instance()->GetRenderDevice()->InstantiateBuffer(BufferType::Uniform);
		if (!m_lightDataBuffer->Initialize(lightOffset.totalSize, BufferUsage::DeviceLocal))
			throw std::runtime_error("failed to create light data buffer");

		std::vector<UInt8> staticLightData(lightOffset.totalSize);
		AccessByOffset<UInt32&>(staticLightData.data(), lightOffset.lightCountOffset) = 1;
		AccessByOffset<UInt32&>(staticLightData.data(), lightOffset.lightsOffset + lightOffset.lightMemberOffsets.type) = 0;
		AccessByOffset<Vector4f&>(staticLightData.data(), lightOffset.lightsOffset + lightOffset.lightMemberOffsets.color) = Vector4f(1.f, 1.f, 1.f, 1.f);
		AccessByOffset<Vector2f&>(staticLightData.data(), lightOffset.lightsOffset + lightOffset.lightMemberOffsets.factor) = Vector2f(0.2f, 1.f);
		AccessByOffset<Vector4f&>(staticLightData.data(), lightOffset.lightsOffset + lightOffset.lightMemberOffsets.parameter1) = Vector4f(0.f, 0.f, -1.f, 1.f);
		AccessByOffset<UInt8&>(staticLightData.data(), lightOffset.lightsOffset + lightOffset.lightMemberOffsets.shadowMappingFlag) = 0;

		if (!m_lightDataBuffer->Fill(staticLightData.data(), 0, staticLightData.size()))
			throw std::runtime_error("failed to fill light data buffer");
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

			renderableData.onElementInvalidated.Connect(instancedRenderable->OnElementInvalidated, [this](InstancedRenderable* /*instancedRenderable*/)
			{
				// TODO: Invalidate only relevant viewers
				for (auto&& [viewer, viewerData] : m_viewers)
				{
					viewerData.rebuildDepthPrepass = true;
					viewerData.rebuildForwardPass = true;
					viewerData.prepare = true;
				}
			});

			renderableData.onMaterialInvalidated.Connect(instancedRenderable->OnMaterialInvalidated, [this](InstancedRenderable* instancedRenderable, std::size_t materialIndex, const std::shared_ptr<Material>& newMaterial)
			{
				if (newMaterial)
				{
					if (const auto& pass = newMaterial->GetPass(m_depthPassIndex))
						RegisterMaterialPass(pass.get());

					if (const auto& pass = newMaterial->GetPass(m_forwardPassIndex))
						RegisterMaterialPass(pass.get());
				}

				const auto& prevMaterial = instancedRenderable->GetMaterial(materialIndex);
				if (prevMaterial)
				{
					if (const auto& pass = prevMaterial->GetPass(m_depthPassIndex))
						UnregisterMaterialPass(pass.get());

					if (const auto& pass = prevMaterial->GetPass(m_forwardPassIndex))
						UnregisterMaterialPass(pass.get());
				}

				// TODO: Invalidate only relevant viewers
				for (auto&& [viewer, viewerData] : m_viewers)
				{
					viewerData.rebuildDepthPrepass = true;
					viewerData.rebuildForwardPass = true;
					viewerData.prepare = true;
				}
			});

			std::size_t matCount = instancedRenderable->GetMaterialCount();
			for (std::size_t i = 0; i < matCount; ++i)
			{
				if (Material* mat = instancedRenderable->GetMaterial(i).get())
				{
					if (const auto& pass = mat->GetPass(m_depthPassIndex))
						RegisterMaterialPass(pass.get());

					if (const auto& pass = mat->GetPass(m_forwardPassIndex))
						RegisterMaterialPass(pass.get());
				}
			}

			for (auto&& [viewer, viewerData] : m_viewers)
			{
				if (viewer->GetRenderMask() & renderMask)
				{
					viewerData.rebuildDepthPrepass = true;
					viewerData.rebuildForwardPass = true;
					viewerData.prepare = true;
				}
			}
		}
	}

	void ForwardFramePipeline::RegisterViewer(AbstractViewer* viewerInstance, Int32 renderOrder)
	{
		auto& viewerData = m_viewers.emplace(viewerInstance, ViewerData{}).first->second;
		viewerData.renderOrder = renderOrder;

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

				for (MaterialPass* material : m_invalidatedMaterials)
				{
					if (material->Update(renderFrame, builder))
					{
						for (auto&& [viewer, viewerData] : m_viewers)
						{
							viewerData.rebuildDepthPrepass = true;
							viewerData.rebuildForwardPass = true;
							viewerData.prepare = true;
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

					if (!frustum.Contains(boundingVolume.aabb))
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

			if (viewerData.visibilityHash != visibilityHash)
			{
				viewerData.rebuildDepthPrepass = true;
				viewerData.rebuildForwardPass = true;
				viewerData.prepare = true;

				viewerData.visibilityHash = visibilityHash;
			}

			if (viewerData.rebuildDepthPrepass)
			{
				renderFrame.PushForRelease(std::move(viewerData.depthPrepassRenderElements));
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

				viewerData.depthPrepassRegistry.Finalize();
			}

			viewerData.depthPrepassRenderQueue.Sort([&](const RenderElement* element)
			{
				return element->ComputeSortingScore(frustum, viewerData.depthPrepassRegistry);
			});

			if (viewerData.rebuildForwardPass)
			{
				renderFrame.PushForRelease(std::move(viewerData.forwardRenderElements));
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

				viewerData.forwardRegistry.Finalize();
			}

			viewerData.forwardRenderQueue.Sort([&](const RenderElement* element)
			{
				return element->ComputeSortingScore(frustum, viewerData.forwardRegistry);
			});
		}

		for (auto&& [viewer, viewerData] : m_viewers)
		{
			if (!viewerData.prepare)
				continue;

			for (std::size_t i = 0; i < m_elementRenderers.size(); ++i)
			{
				auto& elementRendererPtr = m_elementRenderers[i];

				if (i >= viewerData.elementRendererData.size() || !viewerData.elementRendererData[i])
				{
					if (i >= viewerData.elementRendererData.size())
						viewerData.elementRendererData.resize(i + 1);

					viewerData.elementRendererData[i] = elementRendererPtr->InstanciateData();
				}

				if (elementRendererPtr)
					elementRendererPtr->Reset(*viewerData.elementRendererData[i], renderFrame);
			}

			auto& rendererData = viewerData.elementRendererData;

			const auto& viewerInstance = viewer->GetViewerInstance();

			ElementRenderer::RenderStates renderStates;
			renderStates.lightData = m_lightDataBuffer;

			ProcessRenderQueue(viewerData.depthPrepassRenderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
			{
				ElementRenderer& elementRenderer = *m_elementRenderers[elementType];
				elementRenderer.Prepare(viewerInstance, *rendererData[elementType], renderFrame, renderStates, elements, elementCount);
			});

			ProcessRenderQueue(viewerData.forwardRenderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
			{
				ElementRenderer& elementRenderer = *m_elementRenderers[elementType];
				elementRenderer.Prepare(viewerInstance, *rendererData[elementType], renderFrame, renderStates, elements, elementCount);
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

		for (auto&& [viewer, viewerData] : m_viewers)
		{
			viewerData.rebuildForwardPass = false;
			viewerData.rebuildDepthPrepass = false;
			viewerData.prepare = false;
		}

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
			if (const auto& pass = instancedRenderable->GetMaterial(i)->GetPass(m_depthPassIndex))
				UnregisterMaterialPass(pass.get());

			if (const auto& pass = instancedRenderable->GetMaterial(i)->GetPass(m_forwardPassIndex))
				UnregisterMaterialPass(pass.get());
		}

		for (auto&& [viewer, viewerData] : m_viewers)
		{
			viewerData.rebuildDepthPrepass = true;
			viewerData.rebuildForwardPass = true;
			viewerData.prepare = true;
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

				const auto& viewerInstance = viewer->GetViewerInstance();

				ProcessRenderQueue(viewerData.depthPrepassRenderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
				{
					ElementRenderer& elementRenderer = *m_elementRenderers[elementType];
					elementRenderer.Render(viewerInstance, *viewerData.elementRendererData[elementType], builder, elements, elementCount);
				});
			});

			FramePass& forwardPass = frameGraph.AddPass("Forward pass");
			forwardPass.AddOutput(viewerData.colorAttachment);
			forwardPass.SetDepthStencilInput(viewerData.depthStencilAttachment);
			//forwardPass.SetDepthStencilOutput(viewerData.depthStencilAttachment);

			forwardPass.SetClearColor(0, viewer->GetClearColor());
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

				const auto& viewerInstance = viewer->GetViewerInstance();

				ProcessRenderQueue(viewerData.forwardRenderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
				{
					ElementRenderer& elementRenderer = *m_elementRenderers[elementType];
					elementRenderer.Render(viewerInstance , *viewerData.elementRendererData[elementType], builder, elements, elementCount);
				});
			});
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

			forwardPass.SetCommandCallback([this, &targetViewers](CommandBufferBuilder& builder, const Recti& renderRect)
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

	template<typename F>
	void ForwardFramePipeline::ProcessRenderQueue(const RenderQueue<RenderElement*>& renderQueue, F&& callback)
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

			callback(elementType, first, count);
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
