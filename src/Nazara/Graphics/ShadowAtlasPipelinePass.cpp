// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ShadowAtlasPipelinePass.hpp>
#include <NazaraUtils/MathUtils.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/LightShadowData.hpp>
#include <Nazara/Graphics/ShadowViewer.hpp>
#include <fmt/format.h>

namespace Nz
{
	ShadowAtlasPipelinePass::ShadowAtlasPipelinePass(PassData& passData) :
	m_renderQueueHash(0),
	m_shadowAtlas(*Graphics::Instance()->GetRenderDevice(), 8192),
	m_elementRegistry(passData.elementRegistry),
	m_pipeline(passData.pipeline),
	m_renderMask(MaxValue())
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssert(graphics);

		m_passIndex = graphics->GetMaterialPassRegistry().GetPassIndex("ShadowPass");

		BuildCullingPipeline();
	}

	void ShadowAtlasPipelinePass::Prepare(FrameData& frameData)
	{
		m_shadowAtlas.Clear();
		m_pipeline.ForEachShadowCastingLight([&](std::size_t /*lightIndex*/, const Light* /*light*/, LightShadowData* shadowData)
		{
			shadowData->RegisterToAtlas(m_shadowAtlas);
		});

		m_shadowAtlas.Pack();
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
			.format = shadowAtlasTexture->GetFormat(),
			.size = FramePassAttachmentSize::Fixed,
			.width = size.x,
			.height = size.y
		});
		frameGraph.BindExternalTexture(shadowAtlasIndex, shadowAtlasTexture);

		std::size_t prepareAttachment = frameGraph.AddDummyAttachment();
		std::size_t cullAttachment = frameGraph.AddDummyAttachment();
		
		FramePass& preparePass = frameGraph.AddPass("Shadow atlas prepare");
		preparePass.SetExecutionCallback([&]
		{
			auto& renderQueue = m_pipeline.GetRenderQueue(m_passIndex);
			renderQueue.UpdateRenderQueue();

			if (renderQueue.GetContentHash() == m_renderQueueHash)
				return FramePassExecution::Execute;

			//m_renderQueueHash = renderQueue.GetContentHash();
			return FramePassExecution::UpdateAndExecute;
		});

		preparePass.AddOutput(prepareAttachment);

		preparePass.SetCommandCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			m_pipeline.ForEachShadowCastingLight([&](std::size_t lightIndex, const Light* /*light*/, LightShadowData* shadowData)
			{
				LightData& lightData = m_lightData[lightIndex];

				std::size_t viewIndex = 0;
				shadowData->ForEachView([&](std::size_t shadowAtlasEntry, ShadowViewer& shadowViewer)
				{
					m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
					{
						if (elementType >= lightData.elementRendererData.size() || viewIndex >= lightData.elementRendererData[elementType].size())
							return;

						if (lightData.elementRendererData[elementType][viewIndex])
							elementRenderer.Reset(*lightData.elementRendererData[elementType][viewIndex], env.renderResources);
					});

					std::optional<Rectui32> viewport = m_shadowAtlas.GetRect(shadowAtlasEntry);
					if (!viewport)
					{
						viewIndex++;
						return;
					}

					ElementRenderer::RenderData renderData;
					renderData.renderRegion = Recti(*viewport);
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

					UInt32 renderMask = m_renderMask & shadowViewer.GetRenderMask();

					auto& renderQueue = m_pipeline.GetRenderQueue(m_passIndex);
					renderQueue.Process(renderMask, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
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

		FramePass& cullPass = frameGraph.AddPass("Shadow atlas culling");
		cullPass.SetDebugRegionColor(Color::Cyan());
		cullPass.SetExecutionCallback([&]
		{
			return FramePassExecution::UpdateAndExecute;
		});
		cullPass.AddInput(prepareAttachment);
		cullPass.AddOutput(cullAttachment);

		cullPass.SetCommandCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			m_pipeline.ForEachShadowCastingLight([&](std::size_t lightIndex, const Light* /*light*/, LightShadowData* shadowData)
			{
				LightData& lightData = m_lightData[lightIndex];

				std::size_t viewIndex = 0;
				shadowData->ForEachView([&](std::size_t /*shadowAtlasEntry*/, ShadowViewer& shadowViewer)
				{
					builder.BindComputePipeline(*m_computePipeline);

					m_elementRegistry.ForEachElementRenderer([&](std::size_t elementType, ElementRenderer& elementRenderer)
					{
						if (elementType >= lightData.elementRendererData.size() || viewIndex >= lightData.elementRendererData[elementType].size())
							return;

						elementRenderer.ForEachIndirectBuffer(*lightData.elementRendererData[elementType][viewIndex], [&](RenderBuffer& buffer, std::size_t commandCount)
						{
							ShaderBindingPtr computeShaderBinding = m_computePipelineLayout->AllocateShaderBinding(0);
							computeShaderBinding->Update({
								{
									0,
									ShaderBinding::StorageBufferBinding::WholeBuffer(buffer)
								},
								{
									1,
									ShaderBinding::StorageBufferBinding::WholeBuffer(*m_pipeline.GetInstanceBuffer())
								},
								{
									2,
									ShaderBinding::UniformBufferBinding::WholeBuffer(*shadowViewer.GetViewerInstance().GetViewerBuffer())
								}
							});

							builder.BindComputeShaderBinding(0, *computeShaderBinding);

							builder.Dispatch(AlignPow2(SafeCast<UInt32>(commandCount), UInt32(256)) / 256, 1, 1);

							env.renderResources.PushForRelease(std::move(computeShaderBinding));
						});
					});

					viewIndex++;
				});
			});
			builder.MemoryBarrier({ .srcStageMask = PipelineStage::ComputeShader, .dstStageMask = PipelineStage::DrawIndirect, .srcAccessMask = MemoryAccess::ShaderWrite, .dstAccessMask = MemoryAccess::IndirectCommandRead });
		});

		FramePass& renderPass = frameGraph.AddPass("Shadow atlas render");
		renderPass.SetExecutionCallback([&]
		{
			if (m_shadowAtlas.IsEmpty())
				return FramePassExecution::Skip;

			auto& renderQueue = m_pipeline.GetRenderQueue(m_passIndex);
			if (renderQueue.GetContentHash() == m_renderQueueHash)
				return FramePassExecution::Execute;

			m_renderQueueHash = renderQueue.GetContentHash();
			return FramePassExecution::UpdateAndExecute;
		});

		renderPass.AddInput(cullAttachment);
		for (auto&& inputData : inputOuputs.inputAttachments)
			renderPass.AddInput(inputData.attachmentIndex);

		renderPass.SetDepthStencilClear(1.0f, 0);
		renderPass.SetDepthStencilOutput(shadowAtlasIndex);

		renderPass.SetRenderCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			ElementRenderer::RenderData renderData;
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

					UInt32 renderMask = m_renderMask & shadowViewer.GetRenderMask();

					auto& renderQueue = m_pipeline.GetRenderQueue(m_passIndex);
					renderQueue.Process(renderMask, [&](std::size_t elementType, const Pointer<const RenderElement>* elements, std::size_t elementCount)
					{
						ElementRenderer& elementRenderer = m_elementRegistry.GetElementRenderer(elementType);
						elementRenderer.Render(renderData, sceneData, shadowViewer, *lightData.elementRendererData[elementType][viewIndex], env.renderResources, builder, elementCount, elements);
					});

					viewIndex++;
				});
			});
		});

		return renderPass;
	}

	void ShadowAtlasPipelinePass::BuildCullingPipeline()
	{
		Graphics* graphics = Graphics::Instance();
		auto& renderDevice = *graphics->GetRenderDevice();

		m_frustumCullingShader = std::make_shared<UberShader>(nzsl::ShaderStageType::Compute, "Compute.FrustumCulling");

		RenderPipelineLayoutInfo cullingPipelineLayoutInfo;
		cullingPipelineLayoutInfo.pushConstantSize = 6 * sizeof(Nz::Planef);
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
}
