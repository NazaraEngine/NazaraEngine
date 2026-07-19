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
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <NazaraUtils/MathUtils.hpp>

namespace Nz
{
	RasterPipelinePass::RasterPipelinePass(PassData& passData, std::string passName, const std::vector<std::string_view>& renderQueueNames, UInt32 renderMask) :
	m_renderQueueHash(0),
	m_passName(std::move(passName)),
	m_viewer(passData.viewer),
	m_elementRegistry(passData.elementRegistry),
	m_pipeline(passData.pipeline),
	m_renderMask(renderMask)
	{
		BuildCullingPipeline();

		const NameRegistry& renderQueues = Graphics::Instance()->GetRenderQueueRegistry();
		for (std::string_view renderQueueStr : renderQueueNames)
			m_renderQueues.push_back(&m_pipeline.GetRenderQueue(renderQueues.GetIndex(renderQueueStr)));
	}

	FramePass& RasterPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs)
	{
		std::size_t prepareAttachment = frameGraph.AddDummyAttachment();
		std::size_t cullAttachment = frameGraph.AddDummyAttachment();

		FramePass& preparePass = frameGraph.AddPass(m_passName);
		preparePass.SetExecutionCallback([this]
		{
			std::size_t renderQueueHash = GetRenderQueueHash();
			if (renderQueueHash == m_renderQueueHash)
				return FramePassExecution::Skip;

			return FramePassExecution::UpdateAndExecute;
		});

		preparePass.AddOutput(prepareAttachment);

		preparePass.SetCommandCallback([this](GpuCommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				if (elementType < m_elementRendererData.size() && m_elementRendererData[elementType])
					elementRenderer.Reset(*m_elementRendererData[elementType], env.renderResources);
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

			UInt32 renderMask = m_renderMask & m_viewer->GetRenderMask();

			for (RenderQueue* renderQueue : m_renderQueues)
			{
				renderQueue->Process(renderMask, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
				{
					ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);

					if (elementType >= m_elementRendererData.size())
						m_elementRendererData.resize(elementType + 1);

					if (!m_elementRendererData[elementType])
						m_elementRendererData[elementType] = elementRenderer.InstanciateData();

					elementRenderer.Prepare(renderData, sceneData, *m_viewer, *m_elementRendererData[elementType], env.renderResources, elementCount, elements);
				});
			}

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				if (elementType < m_elementRendererData.size() && m_elementRendererData[elementType])
					elementRenderer.PrepareEnd(*m_elementRendererData[elementType], env.renderResources, builder);
			});
		});

		FramePass& cullPass = frameGraph.AddPass(m_passName);
		cullPass.SetDebugRegionColor(Color::Cyan());
		cullPass.SetExecutionCallback([]
		{
			return FramePassExecution::UpdateAndExecute;
		});
		cullPass.AddInput(prepareAttachment);
		cullPass.AddOutput(cullAttachment);

		cullPass.SetCommandCallback([this](GpuCommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			builder.BindComputePipeline(*m_computePipeline);

			m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				if (elementType >= m_elementRendererData.size() || !m_elementRendererData[elementType])
					return;

				elementRenderer.ForEachIndirectBuffer(*m_elementRendererData[elementType], [&](GpuBuffer& buffer, std::size_t commandCount)
				{
					ShaderBindingPtr computeShaderBinding = m_computePipelineLayout->AllocateShaderBinding(0);
					computeShaderBinding->Update({
						{
							0,
							Nz::ShaderBinding::StorageBufferBinding::WholeBuffer(buffer)
						},
						{
							1,
							Nz::ShaderBinding::StorageBufferBinding::WholeBuffer(*m_pipeline.GetInstanceBuffer())
						},
						{
							2,
							Nz::ShaderBinding::UniformBufferBinding::WholeBuffer(*m_viewer->GetViewerInstance().GetViewerBuffer())
						}
					});

					builder.BindComputeShaderBinding(0, *computeShaderBinding);

					builder.Dispatch(AlignPow2(SafeCast<UInt32>(commandCount), UInt32(256)) / 256, 1, 1);

					env.renderResources.PushForRelease(std::move(computeShaderBinding));
				});
			});

			builder.MemoryBarrier({ .srcStageMask = PipelineStage::ComputeShader, .dstStageMask = PipelineStage::DrawIndirect, .srcAccessMask = MemoryAccess::ShaderWrite, .dstAccessMask = MemoryAccess::IndirectCommandRead });
		});

		FramePass& renderPass = frameGraph.AddPass(m_passName);
		renderPass.SetExecutionCallback([this]
		{
			std::size_t renderQueueHash = GetRenderQueueHash();
			if (renderQueueHash == m_renderQueueHash)
				return FramePassExecution::Execute;

			m_renderQueueHash = renderQueueHash;
			return FramePassExecution::UpdateAndExecute;
		});

		renderPass.AddInput(cullAttachment);
		for (auto&& inputData : inputOuputs.inputAttachments)
			renderPass.AddInput(inputData.attachmentIndex);

		for (auto&& outputData : inputOuputs.outputAttachments)
		{
			std::size_t outputIndex = renderPass.AddOutput(outputData.attachmentIndex);

			std::visit(Overloaded{
				[](DontClear) {},
				[&](const Color& color)
				{
					renderPass.SetClearColor(outputIndex, color);
				},
				[&](ViewerClearValue)
				{
					renderPass.SetClearColor(outputIndex, m_viewer->GetClearColor());
				}
			}, outputData.clearColor);
		}

		if (inputOuputs.depthStencilInput != FramePipelinePass::InvalidAttachmentIndex)
			renderPass.SetDepthStencilInput(inputOuputs.depthStencilInput);
		else if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
		{
			std::visit(Overloaded{
				[](DontClear) {},
				[&](float depth)
				{
					renderPass.SetDepthStencilClear(depth, 0);
				},
				[&](ViewerClearValue)
				{
					renderPass.SetDepthStencilClear(m_viewer->GetClearDepth(), 0);
				}
			}, inputOuputs.clearDepth);
		}

		if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
			renderPass.SetDepthStencilOutput(inputOuputs.depthStencilOutput);

		renderPass.SetExecutionCallback([&]
		{
			std::size_t renderQueueHash = GetRenderQueueHash();
			if (renderQueueHash == m_renderQueueHash)
				return FramePassExecution::Execute;

			m_renderQueueHash = renderQueueHash;
			return FramePassExecution::UpdateAndExecute;
		});

		renderPass.SetRenderCallback([this](GpuCommandBufferBuilder& builder, const FramePassEnvironment& env)
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

			m_elementRegistry.ForEachElementRenderer([this](std::size_t elementType, ElementRenderer& elementRenderer)
			{
				if (elementType >= m_elementRendererData.size())
					m_elementRendererData.resize(elementType + 1);

				if (!m_elementRendererData[elementType])
					m_elementRendererData[elementType] = elementRenderer.InstanciateData();
			});

			UInt32 renderMask = m_renderMask & m_viewer->GetRenderMask();

			for (RenderQueue* renderQueue : m_renderQueues)
			{
				renderQueue->Process(renderMask, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
				{
					ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);
					elementRenderer.Render(renderData, sceneData, *m_viewer, *m_elementRendererData[elementType], env.renderResources, builder, elementCount, elements);
				});
			}
		});

		return renderPass;
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

	std::vector<std::string_view> RasterPipelinePass::GetRenderQueues(const ParameterList& parameters)
	{
		Result<std::string_view, ParameterList::Error> renderQueues = parameters.GetStringViewParameter("RenderQueues");
		if (!renderQueues.IsOk())
			throw std::runtime_error("failed to get RasterPipelinePass RenderQueues parameter");

		std::vector<std::string_view> renderQueueNames;
		SplitStringAny(renderQueues.GetValue(), " +,", [&](std::string_view renderQueue)
		{
			if (renderQueue.empty())
				return true;

			renderQueueNames.push_back(renderQueue);
			return true;
		});

		if (renderQueueNames.empty())
			throw std::runtime_error("no render queues");

		return renderQueueNames;
	}

	void RasterPipelinePass::BuildCullingPipeline()
	{
		Graphics* graphics = Graphics::Instance();
		auto& renderDevice = *graphics->GetGpuDevice();

		m_frustumCullingShader = std::make_shared<UberShader>(nzsl::ShaderStageType::Compute, "Compute.FrustumCulling");

		GpuPipelineLayoutInfo cullingPipelineLayoutInfo;
		cullingPipelineLayoutInfo.bindings.push_back({
			.bindingIndex = 0,
			.type = ShaderBindingType::StorageBuffer,
			.shaderStageFlags = nzsl::ShaderStageType::Compute
		});

		cullingPipelineLayoutInfo.bindings.push_back({
			.bindingIndex = 1,
			.type = ShaderBindingType::StorageBuffer,
			.shaderStageFlags = nzsl::ShaderStageType::Compute
		});

		cullingPipelineLayoutInfo.bindings.push_back({
			.bindingIndex = 2,
			.type = ShaderBindingType::UniformBuffer,
			.shaderStageFlags = nzsl::ShaderStageType::Compute
		});

		m_computePipelineLayout = renderDevice.InstantiateRenderPipelineLayout(std::move(cullingPipelineLayoutInfo));

		m_computePipeline = renderDevice.InstantiateComputePipeline({
			.pipelineLayout = m_computePipelineLayout,
			.shaderModule = m_frustumCullingShader->Get({})
		});
	}

	std::size_t RasterPipelinePass::GetRenderQueueHash() const
	{
		std::size_t renderQueueHash = 0;
		for (RenderQueue* renderQueue : m_renderQueues)
			renderQueueHash ^= renderQueue->GetContentHash() + 0x9e3779b9 + (renderQueueHash << 6) + (renderQueueHash >> 2);

		return renderQueueHash;
	}
}
