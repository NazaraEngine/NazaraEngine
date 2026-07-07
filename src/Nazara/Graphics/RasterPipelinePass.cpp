// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/RasterPipelinePass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>

namespace Nz
{
	FramePass& RasterPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs)
	{
		FramePass& pass = frameGraph.AddPass(m_passName);
		for (auto&& inputData : inputOuputs.inputAttachments)
			pass.AddInput(inputData.attachmentIndex);

		for (auto&& outputData : inputOuputs.outputAttachments)
		{
			std::size_t outputIndex = pass.AddOutput(outputData.attachmentIndex);

			std::visit(Overloaded{
				[](DontClear) {},
				[&](const Color& color)
				{
					pass.SetClearColor(outputIndex, color);
				},
				[&](ViewerClearValue)
				{
					pass.SetClearColor(outputIndex, m_viewer->GetClearColor());
				}
			}, outputData.clearColor);
		}

		if (inputOuputs.depthStencilInput != FramePipelinePass::InvalidAttachmentIndex)
			pass.SetDepthStencilInput(inputOuputs.depthStencilInput);
		else if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
		{
			std::visit(Overloaded{
				[](DontClear) {},
				[&](float depth)
				{
					pass.SetDepthStencilClear(depth, 0);
				},
				[&](ViewerClearValue)
				{
					pass.SetDepthStencilClear(m_viewer->GetClearDepth(), 0);
				}
			}, inputOuputs.clearDepth);
		}

		if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
			pass.SetDepthStencilOutput(inputOuputs.depthStencilOutput);

		pass.SetExecutionCallback([&]
		{
			return FramePassExecution::UpdateAndExecute;
		});

		pass.SetCommandCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
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

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				if (elementType >= m_elementRendererData.size())
					m_elementRendererData.resize(elementType + 1);

				if (!m_elementRendererData[elementType])
					m_elementRendererData[elementType] = elementRenderer.InstanciateData();
			});

			UInt32 renderMask = m_renderMask & m_viewer->GetRenderMask();

			auto& renderQueue = m_pipeline.GetRenderQueue(m_passIndex);
			renderQueue.Process(renderMask, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
			{
				ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);
				elementRenderer.Prepare(renderData, sceneData, *m_viewer, *m_elementRendererData[elementType], env.renderResources, elementCount, elements);
			});

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				elementRenderer.PrepareEnd(*m_elementRendererData[elementType], env.renderResources, builder);
			});
		});

		pass.SetRenderCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			Recti viewport = m_viewer->GetViewport();

			builder.SetScissor(viewport);
			builder.SetViewport(viewport);

			ElementRenderer::RenderData renderData;
			renderData.renderRegion = viewport;
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

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				if (elementType >= m_elementRendererData.size())
					m_elementRendererData.resize(elementType + 1);

				if (!m_elementRendererData[elementType])
					m_elementRendererData[elementType] = elementRenderer.InstanciateData();
			});

			UInt32 renderMask = m_renderMask & m_viewer->GetRenderMask();

			auto& renderQueue = m_pipeline.GetRenderQueue(m_passIndex);
			renderQueue.Process(renderMask, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
			{
				ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);
				elementRenderer.Render(renderData, sceneData, *m_viewer, *m_elementRendererData[elementType], env.renderResources, builder, elementCount, elements);
			});

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				elementRenderer.Reset(*m_elementRendererData[elementType], env.renderResources);
			});
		});

		return pass;
	}

	std::size_t RasterPipelinePass::GetMaterialPassIndex(const ParameterList& parameters)
	{
		Result<long long, ParameterList::Error> passIndexResult = parameters.GetIntegerParameter("MatPassIndex");
		if (passIndexResult.IsOk())
			return passIndexResult.GetValue();
		// TODO: Log error if key is present but not of the right type

		Result<std::string_view, ParameterList::Error> passResult = parameters.GetStringViewParameter("MatPass");
		if (passResult.IsOk())
		{
			auto& materialPassRegistry = Graphics::Instance()->GetMaterialPassRegistry();

			std::string_view passName = passResult.GetValue();
			return materialPassRegistry.GetPassIndex(passName);
		}
		// TODO: Log error if key is present but not of the right type

		throw std::runtime_error("RasterPipelinePass expect either MatPass or MatPassIndex parameter");
	}

	UInt32 RasterPipelinePass::GetRenderMask(const ParameterList& parameters)
	{
		Result<long long, ParameterList::Error> renderMaskResult = parameters.GetIntegerParameter("RenderMask", false);
		if (renderMaskResult.IsOk())
		{
			long long renderMask = renderMaskResult.GetValue();
			if (renderMask < 0 || renderMask >= MaxValue<UInt32>())
				throw std::runtime_error("RasterPipelinePass RenderMask value is out of range");

			return SafeCaster(renderMaskResult.GetValue());
		}

		return MaxValue();
	}
}
