// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ShadowAtlasPipelinePass.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/LightShadowData.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/ShadowViewer.hpp>

namespace Nz
{
	ShadowAtlasPipelinePass::ShadowAtlasPipelinePass(PassData& passData) :
	FramePipelinePass(FramePipelineNotification::ElementInvalidation | FramePipelineNotification::MaterialInstanceRegistration),
	m_lastVisibilityHash(0),
	m_shadowAtlas(*Graphics::Instance()->GetRenderDevice(), 8192),
	m_elementRegistry(passData.elementRegistry),
	m_pipeline(passData.pipeline),
	m_rebuildElements(false)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssert(graphics);

		m_passIndex = graphics->GetMaterialPassRegistry().GetPassIndex("ShadowPass");
	}

	void ShadowAtlasPipelinePass::Prepare(FrameData& frameData)
	{
		if (m_lastVisibilityHash != frameData.visibilityHash || m_rebuildElements) //< FIXME
		{
			frameData.renderResources.PushForRelease(std::move(m_renderElements));
			m_renderElements.clear();

			for (const auto& renderableData : frameData.visibleRenderables)
			{
				InstancedRenderable::ElementData elementData{
					&renderableData.scissorBox,
					renderableData.skeletonInstance,
					renderableData.worldInstance
				};

				renderableData.instancedRenderable->BuildElement(m_elementRegistry, elementData, m_passIndex, m_renderElements);
			}

			m_renderQueueRegistry.Clear();
			m_renderQueue.Clear();

			for (const auto& renderElement : m_renderElements)
			{
				renderElement->Register(m_renderQueueRegistry);
				m_renderQueue.Insert(renderElement.GetElement());
			}

			m_renderQueueRegistry.Finalize();

			m_lastVisibilityHash = frameData.visibilityHash;
			m_rebuildElements = false;
		}

		// TODO: Don't sort every frame if no material pass requires distance sorting
		m_renderQueue.Sort([&](const RenderElement* element)
		{
			return element->ComputeSortingScore(frameData.frustum, m_renderQueueRegistry);
		});

		m_shadowAtlas.Clear();
		m_pipeline.ForEachShadowCastingLight([&](std::size_t /*lightIndex*/, const Light* /*light*/, LightShadowData* shadowData)
		{
			shadowData->RegisterToAtlas(m_shadowAtlas);
		});

		m_shadowAtlas.Pack();
	}

	void ShadowAtlasPipelinePass::RegisterMaterialInstance(const MaterialInstance& materialInstance)
	{
		if (!materialInstance.HasPass(m_passIndex))
			return;

		auto it = m_materialInstances.find(&materialInstance);
		if (it == m_materialInstances.end())
		{
			auto& matPassEntry = m_materialInstances[&materialInstance];
			matPassEntry.onMaterialInstancePipelineInvalidated.Connect(materialInstance.OnMaterialInstancePipelineInvalidated, [this](const MaterialInstance*, std::size_t passIndex)
			{
				if (passIndex != m_passIndex)
					return;

				m_rebuildElements = true;
			});
		}
		else
			it->second.usedCount++;
	}

	FramePass& ShadowAtlasPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs)
	{
		NazaraAssertMsg(inputOuputs.outputAttachments.empty(), "no outputs expected");
		NazaraAssertMsg(inputOuputs.depthStencilInput == FramePipelinePass::InvalidAttachmentIndex, "no depth-stencil input expected");
		NazaraAssertMsg(inputOuputs.depthStencilOutput == FramePipelinePass::InvalidAttachmentIndex, "no depth-stencil output expected");

		const std::shared_ptr<Texture>& shadowAtlasTexture = m_shadowAtlas.GetTexture();
		const Vector3ui& size = shadowAtlasTexture->GetSize();

		std::size_t shadowAtlasIndex = frameGraph.AddAttachment({
			.name = "Shadow atlas",
			.format = PixelFormat::Depth16,
			.size = FramePassAttachmentSize::Fixed,
			.width = size.x,
			.height = size.y
		});
		frameGraph.BindExternalTexture(shadowAtlasIndex, shadowAtlasTexture);

		FramePass& pass = frameGraph.AddPass("Shadow atlas");
		for (auto&& inputData : inputOuputs.inputAttachments)
			pass.AddInput(inputData.attachmentIndex);

		pass.SetDepthStencilClear(1.0f, 0);
		pass.SetDepthStencilOutput(shadowAtlasIndex);

		pass.SetExecutionCallback([&]
		{
			return (!m_shadowAtlas.IsEmpty()) ? FramePassExecution::UpdateAndExecute : FramePassExecution::Skip;
		});

		pass.SetCommandCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			std::size_t shadowViewerIndex = 0;
			m_pipeline.ForEachShadowCastingLight([&](std::size_t lightIndex, const Light* /*light*/, LightShadowData* shadowData)
			{
				LightData& lightData = m_lightData[lightIndex];

				std::size_t viewIndex = 0;
				shadowData->ForEachView([&](std::size_t shadowAtlasEntry, ShadowViewer& shadowViewer)
				{
					std::optional<Rectui32> viewport = m_shadowAtlas.GetRect(shadowAtlasEntry);
					if (!viewport)
						return;

					ElementRenderer::RenderData renderData;
					renderData.renderRegion = Recti(*viewport);
					renderData.shaderBindingCache = m_pipeline.GetShaderBindingCache();

					ElementRenderer::SceneData sceneData;
					sceneData.directionalLights = m_pipeline.GetDirectionalLightBuffer();
					sceneData.directionalLightAtlasMapping = m_pipeline.GetDirectionalShadowMappingBuffer();
					sceneData.pointLights = m_pipeline.GetPointLightBuffer();
					sceneData.pointLightAtlasMapping = m_pipeline.GetPointShadowMappingBuffer();
					sceneData.shadowAtlas = m_pipeline.GetShadowAtlasTexture();
					sceneData.spotLights = m_pipeline.GetSpotLightBuffer();
					sceneData.spotLightAtlasMapping = m_pipeline.GetSpotShadowMappingBuffer();

					m_elementRegistry.ProcessRenderQueue(m_renderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
					{
						ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);

						if (elementType >= lightData.elementRendererData.size())
							lightData.elementRendererData.resize(elementType + 1);

						if (viewIndex >= lightData.elementRendererData[elementType].size())
							lightData.elementRendererData[elementType].resize(viewIndex + 1);

						if (!lightData.elementRendererData[elementType][viewIndex])
							lightData.elementRendererData[elementType][viewIndex] = elementRenderer.InstanciateData();

						elementRenderer.Prepare(renderData, sceneData, shadowViewer, *lightData.elementRendererData[elementType][viewIndex], env.renderResources, elementCount, elements);
					});

					m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
					{
						if (elementType >= lightData.elementRendererData.size() || viewIndex >= lightData.elementRendererData[elementType].size())
							return;

						elementRenderer.PrepareEnd(*lightData.elementRendererData[elementType][viewIndex], env.renderResources, builder);
					});

					viewIndex++;
				});
			});
		});

		pass.SetRenderCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			ElementRenderer::RenderData renderData;
			renderData.shaderBindingCache = m_pipeline.GetShaderBindingCache();

			ElementRenderer::SceneData sceneData;
			sceneData.directionalLights = m_pipeline.GetDirectionalLightBuffer();
			sceneData.directionalLightAtlasMapping = m_pipeline.GetDirectionalShadowMappingBuffer();
			sceneData.pointLights = m_pipeline.GetPointLightBuffer();
			sceneData.pointLightAtlasMapping = m_pipeline.GetPointShadowMappingBuffer();
			sceneData.shadowAtlas = m_pipeline.GetShadowAtlasTexture();
			sceneData.spotLights = m_pipeline.GetSpotLightBuffer();
			sceneData.spotLightAtlasMapping = m_pipeline.GetSpotShadowMappingBuffer();

			std::size_t shadowViewerIndex = 0;
			m_pipeline.ForEachShadowCastingLight([&](std::size_t lightIndex, const Light* /*light*/, LightShadowData* shadowData)
			{
				LightData& lightData = m_lightData[lightIndex];

				std::size_t viewIndex = 0;
				shadowData->ForEachView([&](std::size_t shadowAtlasEntry, ShadowViewer& shadowViewer)
				{
					std::optional<Rectui32> viewport = m_shadowAtlas.GetRect(shadowAtlasEntry);
					if (!viewport)
						return;

					Recti shadowAtlasViewport(*viewport);
					shadowViewer.UpdateViewport(shadowAtlasViewport);

					builder.SetScissor(shadowAtlasViewport);
					builder.SetViewport(shadowAtlasViewport);

					renderData.renderRegion = Recti(*viewport);

					m_elementRegistry.ProcessRenderQueue(m_renderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
					{
						ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);
						elementRenderer.Render(renderData, sceneData, shadowViewer, *lightData.elementRendererData[elementType][viewIndex], env.renderResources, builder, elementCount, elements);
					});

					m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
					{
						if (elementType >= lightData.elementRendererData.size() || viewIndex >= lightData.elementRendererData[elementType].size())
							return;

						if (lightData.elementRendererData[elementType][viewIndex])
							elementRenderer.Reset(*lightData.elementRendererData[elementType][viewIndex], env.renderResources);
					});

					viewIndex++;
				});
			});
		});

		return pass;
	}

	void ShadowAtlasPipelinePass::UnregisterMaterialInstance(const MaterialInstance& materialInstance)
	{
		auto it = m_materialInstances.find(&materialInstance);
		if (it != m_materialInstances.end())
		{
			if (--it->second.usedCount == 0)
				m_materialInstances.erase(it);
		}
	}
}
