// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/PostProcessPipelinePass.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/Graphics.hpp>

namespace Nz
{
	PostProcessPipelinePass::PostProcessPipelinePass(PassData& /*passData*/, std::string passName, std::string shaderName) :
	FramePipelinePass({}),
	m_passName(std::move(passName)),
	m_shader(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, std::move(shaderName))
	{
		RenderPipelineLayoutInfo layoutInfo;
		layoutInfo.bindings.assign({
			{
				0, 0, 1,
				ShaderBindingType::Sampler,
				nzsl::ShaderStageType::Fragment
			}
		});

		std::shared_ptr<RenderDevice> renderDevice = Graphics::Instance()->GetRenderDevice();
		m_renderPipelineLayout = renderDevice->InstantiateRenderPipelineLayout(std::move(layoutInfo));
		if (!m_renderPipelineLayout)
			throw std::runtime_error("failed to instantiate postprocess RenderPipelineLayout");

		m_onShaderUpdated.Connect(m_shader.OnShaderUpdated, [this](UberShader*)
		{
			BuildPipeline();
		});
		BuildPipeline();
	}

	void PostProcessPipelinePass::Prepare(FrameData& frameData)
	{
		if (m_nextRenderPipeline)
		{
			if (m_renderPipeline)
				frameData.renderResources.PushForRelease(std::move(m_renderPipeline));

			m_renderPipeline = std::move(m_nextRenderPipeline);
			m_rebuildFramePass = true;
		}
	}

	FramePass& PostProcessPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs)
	{
		if (inputOuputs.inputAttachments.size() != 1)
			throw std::runtime_error("one input expected");

		if (inputOuputs.outputAttachments.size() != 1)
			throw std::runtime_error("one output expected");

		if (inputOuputs.depthStencilInput != InvalidAttachmentIndex)
			throw std::runtime_error("unexpected depth-stencil output");

		if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
			throw std::runtime_error("unexpected depth-stencil output");

		std::size_t inputColorBufferIndex = inputOuputs.inputAttachments[0].attachmentIndex;

		FramePass& postProcess = frameGraph.AddPass(m_passName);
		postProcess.AddInput(inputColorBufferIndex);
		postProcess.AddOutput(inputOuputs.outputAttachments[0].attachmentIndex);

		postProcess.SetExecutionCallback([&]
		{
			return (m_rebuildFramePass) ? FramePassExecution::UpdateAndExecute : FramePassExecution::Execute;
		});

		postProcess.SetCommandCallback([this, inputColorBufferIndex](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			if (m_shaderBinding)
				env.renderResources.PushForRelease(std::move(m_shaderBinding));

			auto& samplerCache = Graphics::Instance()->GetSamplerCache();

			const auto& sourceTexture = env.frameGraph.GetAttachmentTexture(inputColorBufferIndex);
			const auto& sampler = samplerCache.Get({});

			m_shaderBinding = m_renderPipelineLayout->AllocateShaderBinding(0);
			m_shaderBinding->Update({
				{
					0,
					ShaderBinding::SampledTextureBinding {
						sourceTexture.get(), sampler.get()
					}
				}
			});

			builder.SetScissor(env.renderRect);
			builder.SetViewport(env.renderRect);

			builder.BindRenderPipeline(*m_renderPipeline);
			builder.BindRenderShaderBinding(0, *m_shaderBinding);

			builder.Draw(3);

			m_rebuildFramePass = false;
		});

		return postProcess;
	}

	std::string PostProcessPipelinePass::GetShaderName(const ParameterList& parameters)
	{
		Result<std::string, ParameterList::Error> shaderResult = parameters.GetStringParameter("Shader");
		if (shaderResult.IsOk())
			return std::move(shaderResult).GetValue();
		// TODO: Log error if key is present but not of the right

		throw std::runtime_error("PostProcessPipelinePass expect a Shader parameter");
	}

	void PostProcessPipelinePass::BuildPipeline()
	{
		std::shared_ptr<RenderDevice> renderDevice = Graphics::Instance()->GetRenderDevice();

		RenderPipelineInfo pipelineInfo;
		pipelineInfo.pipelineLayout = m_renderPipelineLayout;
		pipelineInfo.shaderModules.push_back(m_shader.Get({}));

		m_nextRenderPipeline = renderDevice->InstantiateRenderPipeline(pipelineInfo);
	}
}
