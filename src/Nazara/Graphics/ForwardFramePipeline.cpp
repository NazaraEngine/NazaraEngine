// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardFramePipeline.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
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
	m_rebuildFrameGraph(true),
	m_rebuildForwardPass(false)
	{
	}

	void ForwardFramePipeline::InvalidateViewer(AbstractViewer* viewerInstance)
	{
		m_invalidatedViewerInstances.insert(viewerInstance);
	}

	void ForwardFramePipeline::InvalidateWorldInstance(WorldInstance* worldInstance)
	{
		m_invalidatedWorldInstances.insert(worldInstance);
	}

	void ForwardFramePipeline::RegisterInstancedDrawable(WorldInstance* worldInstance, const InstancedRenderable* instancedRenderable)
	{
		auto& renderableMap = m_renderables[worldInstance];
		if (auto it = renderableMap.find(instancedRenderable); it == renderableMap.end())
		{
			auto& renderableData = renderableMap.emplace(instancedRenderable, RenderableData{}).first->second;
			renderableData.onMaterialInvalidated.Connect(instancedRenderable->OnMaterialInvalidated, [this](InstancedRenderable* instancedRenderable, std::size_t materialIndex, const std::shared_ptr<Material>& newMaterial)
			{
				if (newMaterial)
				{
					if (MaterialPass* pass = newMaterial->GetPass("DepthPass"))
						RegisterMaterialPass(pass);

					if (MaterialPass* pass = newMaterial->GetPass("ForwardPass"))
						RegisterMaterialPass(pass);
				}

				const auto& prevMaterial = instancedRenderable->GetMaterial(materialIndex);
				if (prevMaterial)
				{
					if (MaterialPass* pass = prevMaterial->GetPass("DepthPass"))
						UnregisterMaterialPass(pass);

					if (MaterialPass* pass = prevMaterial->GetPass("ForwardPass"))
						UnregisterMaterialPass(pass);
				}

				m_rebuildForwardPass = true;
			});

			std::size_t matCount = instancedRenderable->GetMaterialCount();
			for (std::size_t i = 0; i < matCount; ++i)
			{
				if (Material* mat = instancedRenderable->GetMaterial(i).get())
				{
					if (MaterialPass* pass = mat->GetPass("DepthPass"))
						RegisterMaterialPass(pass);

					if (MaterialPass* pass = mat->GetPass("ForwardPass"))
						RegisterMaterialPass(pass);
				}
			}

			m_rebuildForwardPass = true;
		}
	}

	void ForwardFramePipeline::RegisterViewer(AbstractViewer* viewerInstance)
	{
		m_viewers.emplace(viewerInstance, ViewerData{});
	}

	void ForwardFramePipeline::Render(RenderFrame& renderFrame)
	{
		Graphics* graphics = Graphics::Instance();

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
						m_rebuildForwardPass = true;
				}
				m_invalidatedMaterials.clear();

				builder.PostTransferBarrier();
			}
			builder.EndDebugRegion();
		}, QueueType::Transfer);

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
		m_rebuildForwardPass = false;
		m_rebuildFrameGraph = false;

		const Vector2ui& frameSize = renderFrame.GetSize();
		for (auto&& [viewer, viewerData] : m_viewers)
		{
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
						builder.BindVertexBuffer(0, graphics->GetFullscreenVertexBuffer().get());
						builder.BindShaderBinding(0, *blitShaderBinding);

						builder.Draw(3);
					}
					builder.EndRenderPass();
				}
				builder.EndDebugRegion();

			}, QueueType::Graphics);
		}
	}

	void ForwardFramePipeline::UnregisterInstancedDrawable(WorldInstance* worldInstance, const InstancedRenderable* instancedRenderable)
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
			m_renderables.erase(worldInstance);

		std::size_t matCount = instancedRenderable->GetMaterialCount();
		for (std::size_t i = 0; i < matCount; ++i)
		{
			if (MaterialPass* pass = instancedRenderable->GetMaterial(i)->GetPass("ForwardPass"))
				UnregisterMaterialPass(pass);
		}

		m_rebuildForwardPass = true;
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

		for (auto&& [viewer, viewerData] : m_viewers)
		{
			FramePass& depthPrepass = frameGraph.AddPass("Depth pre-pass");
			depthPrepass.SetDepthStencilOutput(viewerData.depthStencilAttachment);
			depthPrepass.SetDepthStencilClear(1.f, 0);
			
			depthPrepass.SetExecutionCallback([this]()
			{
				if (m_rebuildForwardPass)
					return FramePassExecution::UpdateAndExecute;
				else
					return FramePassExecution::Execute;
			});
			
			depthPrepass.SetCommandCallback([this, viewer = viewer](CommandBufferBuilder& builder, const Recti& /*renderRect*/)
			{
				Recti viewport = viewer->GetViewport();

				builder.SetScissor(viewport);
				builder.SetViewport(viewport);

				builder.BindShaderBinding(Graphics::ViewerBindingSet, viewer->GetViewerInstance().GetShaderBinding());

				for (const auto& [worldInstance, renderables] : m_renderables)
				{
					builder.BindShaderBinding(Graphics::WorldBindingSet, worldInstance->GetShaderBinding());

					for (const auto& [renderable, renderableData] : renderables)
						renderable->Draw("DepthPass", builder);
				}
			});

			FramePass& forwardPass = frameGraph.AddPass("Forward pass");
			forwardPass.AddOutput(viewerData.colorAttachment);
			forwardPass.SetDepthStencilInput(viewerData.depthStencilAttachment);
			forwardPass.SetDepthStencilOutput(viewerData.depthStencilAttachment);

			forwardPass.SetClearColor(0, Color::Black);
			forwardPass.SetDepthStencilClear(1.f, 0);

			forwardPass.SetExecutionCallback([this]()
			{
				if (m_rebuildForwardPass)
					return FramePassExecution::UpdateAndExecute;
				else
					return FramePassExecution::Execute;
			});

			forwardPass.SetCommandCallback([this, viewer = viewer](CommandBufferBuilder& builder, const Recti& /*renderRect*/)
			{
				Recti viewport = viewer->GetViewport();

				builder.SetScissor(viewport);
				builder.SetViewport(viewport);

				builder.BindShaderBinding(Graphics::ViewerBindingSet, viewer->GetViewerInstance().GetShaderBinding());

				for (const auto& [worldInstance, renderables] : m_renderables)
				{
					builder.BindShaderBinding(Graphics::WorldBindingSet, worldInstance->GetShaderBinding());

					for (const auto& [renderable, renderableData] : renderables)
						renderable->Draw("ForwardPass", builder);
				}
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
