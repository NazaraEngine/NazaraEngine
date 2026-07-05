// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ForwardPipelinePass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>

namespace Nz
{
	ForwardPipelinePass::ForwardPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters) :
	BaseElementRenderPipelinePass(passData),
	m_passName(std::move(passName)),
	m_viewer(passData.viewer),
	m_pipeline(passData.pipeline)
	{
		if (auto result = parameters.GetIntegerParameter("RenderMask", false); result && result.GetValue() >= 0 && result.GetValue() < MaxValue<UInt32>())
			m_renderMask = SafeCaster(result.GetValue());
		else if (result.GetError() != Nz::ParameterList::Error::MissingValue)
			throw std::runtime_error("parameter RenderMask has incorrect value");

		Graphics* graphics = Graphics::Instance();
		m_passIndex = graphics->GetMaterialPassRegistry().GetPassIndex("ForwardPass");
	}

	FramePass& ForwardPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs)
	{
		FramePass& forwardPass = frameGraph.AddPass(m_passName);
		for (auto&& inputData : inputOuputs.inputAttachments)
			forwardPass.AddInput(inputData.attachmentIndex);

		for (auto&& outputData : inputOuputs.outputAttachments)
		{
			std::size_t outputIndex = forwardPass.AddOutput(outputData.attachmentIndex);

			std::visit(Overloaded{
				[](DontClear) {},
				[&](const Color& color)
				{
					forwardPass.SetClearColor(outputIndex, color);
				},
				[&](ViewerClearValue)
				{
					forwardPass.SetClearColor(outputIndex, m_viewer->GetClearColor());
				}
			}, outputData.clearColor);
		}

		if (inputOuputs.depthStencilInput != FramePipelinePass::InvalidAttachmentIndex)
			forwardPass.SetDepthStencilInput(inputOuputs.depthStencilInput);
		else if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
		{
			std::visit(Overloaded{
				[](DontClear) {},
				[&](float depth)
				{
					forwardPass.SetDepthStencilClear(depth, 0);
				},
				[&](ViewerClearValue)
				{
					forwardPass.SetDepthStencilClear(m_viewer->GetClearDepth(), 0);
				}
			}, inputOuputs.clearDepth);
		}

		if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
			forwardPass.SetDepthStencilOutput(inputOuputs.depthStencilOutput);

		forwardPass.SetExecutionCallback([&]()
		{
			return FramePassExecution::UpdateAndExecute;
		});

		forwardPass.SetCommandCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				if (elementType >= m_elementRendererData.size())
					m_elementRendererData.resize(elementType + 1);

				if (!m_elementRendererData[elementType])
					m_elementRendererData[elementType] = elementRenderer.InstanciateData();
			});

			ElementRenderer::RenderData renderData;
			renderData.renderRegion = env.renderRect;
			renderData.shaderBindingCache = m_pipeline.GetShaderBindingCache();

			ElementRenderer::SceneData sceneData;
			sceneData.directionalLights = m_pipeline.GetDirectionalLightBuffer();
			sceneData.directionalLightAtlasMapping = m_pipeline.GetDirectionalShadowMappingBuffer();
			sceneData.instanceBuffer = m_pipeline.GetInstanceBuffer();
			sceneData.pointLights = m_pipeline.GetPointLightBuffer();
			sceneData.pointLightAtlasMapping = m_pipeline.GetPointShadowMappingBuffer();
			sceneData.shadowAtlas = m_pipeline.GetShadowAtlasTexture();
			sceneData.spotLights = m_pipeline.GetSpotLightBuffer();
			sceneData.spotLightAtlasMapping = m_pipeline.GetSpotShadowMappingBuffer();

			m_elementRegistry.ProcessRenderQueue(m_renderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
			{
				ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);
				elementRenderer.Prepare(renderData, sceneData, *m_viewer, *m_elementRendererData[elementType], env.renderResources, elementCount, elements);
			});

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				elementRenderer.PrepareEnd(*m_elementRendererData[elementType], env.renderResources, builder);
			});
		});

		forwardPass.SetRenderCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			Recti viewport = m_viewer->GetViewport();

			builder.SetScissor(viewport);
			builder.SetViewport(viewport);

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				if (elementType >= m_elementRendererData.size())
					m_elementRendererData.resize(elementType + 1);

				if (!m_elementRendererData[elementType])
					m_elementRendererData[elementType] = elementRenderer.InstanciateData();
			});

			ElementRenderer::RenderData renderData;
			renderData.renderRegion = env.renderRect;
			renderData.shaderBindingCache = m_pipeline.GetShaderBindingCache();

			ElementRenderer::SceneData sceneData;
			sceneData.directionalLights = m_pipeline.GetDirectionalLightBuffer();
			sceneData.directionalLightAtlasMapping = m_pipeline.GetDirectionalShadowMappingBuffer();
			sceneData.instanceBuffer = m_pipeline.GetInstanceBuffer();
			sceneData.pointLights = m_pipeline.GetPointLightBuffer();
			sceneData.pointLightAtlasMapping = m_pipeline.GetPointShadowMappingBuffer();
			sceneData.shadowAtlas = m_pipeline.GetShadowAtlasTexture();
			sceneData.spotLights = m_pipeline.GetSpotLightBuffer();
			sceneData.spotLightAtlasMapping = m_pipeline.GetSpotShadowMappingBuffer();

			m_elementRegistry.ProcessRenderQueue(m_renderQueue, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
			{
				ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);
				elementRenderer.Render(renderData, sceneData, *m_viewer, *m_elementRendererData[elementType], env.renderResources, builder, elementCount, elements);
			});

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				elementRenderer.Reset(*m_elementRendererData[elementType], env.renderResources);
			});
		});

		return forwardPass;
	}
}
