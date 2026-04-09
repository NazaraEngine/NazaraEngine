// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/PostProcessPipelinePass.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <NazaraUtils/StackArray.hpp>

namespace Nz
{
	PostProcessPipelinePass::PostProcessPipelinePass(PassData& /*passData*/, std::string passName, std::string shaderName, UInt32 inputCount) :
	FramePipelinePass({}),
	m_passName(std::move(passName)),
	m_shader(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, std::move(shaderName)),
	m_inputCount(inputCount)
	{
		RenderPipelineLayoutInfo layoutInfo;
		for (UInt32 inputIndex = 0; inputIndex < m_inputCount; ++inputIndex)
		{
			layoutInfo.bindings.push_back(RenderPipelineLayoutInfo::Binding{
				0, inputIndex, 1,
				ShaderBindingType::Sampler,
				nzsl::ShaderStageType::Fragment
			});
		}

		std::shared_ptr<RenderDevice> renderDevice = Graphics::Instance()->GetRenderDevice();
		m_renderPipelineLayout = renderDevice->InstantiateRenderPipelineLayout(std::move(layoutInfo));
		if (!m_renderPipelineLayout)
			throw std::runtime_error("failed to instantiate post-process RenderPipelineLayout");

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
		if (inputOuputs.inputAttachments.size() != m_inputCount)
			throw std::runtime_error("one input expected");

		if (inputOuputs.outputAttachments.size() != 1)
			throw std::runtime_error("one output expected");

		if (inputOuputs.depthStencilInput != InvalidAttachmentIndex)
			throw std::runtime_error("unexpected depth-stencil output");

		if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
			throw std::runtime_error("unexpected depth-stencil output");

		FramePass& postProcess = frameGraph.AddPass(m_passName);

		Nz::HybridVector<std::size_t, 2> inputIndices(m_inputCount);
		for (std::size_t i = 0; i < m_inputCount; ++i)
		{
			inputIndices[i] = inputOuputs.inputAttachments[i].attachmentIndex;
			postProcess.AddInput(inputIndices[i]);
		}

		postProcess.AddOutput(inputOuputs.outputAttachments[0].attachmentIndex);

		postProcess.SetExecutionCallback([&]
		{
			return (m_rebuildFramePass) ? FramePassExecution::UpdateAndExecute : FramePassExecution::Execute;
		});

		postProcess.SetCommandCallback([this, inputIndices](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			if (m_shaderBinding)
				env.renderResources.PushForRelease(std::move(m_shaderBinding));

			auto& samplerCache = Graphics::Instance()->GetSamplerCache();

			const auto& sampler = samplerCache.Get({});

			StackArray bindings = NazaraStackArray(Nz::ShaderBinding::Binding, inputIndices.size());
			for (UInt32 i = 0; i < inputIndices.size(); ++i)
			{
				const auto& inputTexture = env.frameGraph.GetAttachmentTexture(inputIndices[i]);

				bindings[i] = {
					i,
					ShaderBinding::SampledTextureBinding {
						inputTexture.get(), sampler.get()
					}
				};
			}

			m_shaderBinding = m_renderPipelineLayout->AllocateShaderBinding(0);
			m_shaderBinding->Update(bindings.data(), bindings.size());

			builder.SetScissor(env.renderRect);
			builder.SetViewport(env.renderRect);

			builder.BindRenderPipeline(*m_renderPipeline);
			builder.BindRenderShaderBinding(0, *m_shaderBinding);

			builder.Draw(3);

			m_rebuildFramePass = false;
		});

		return postProcess;
	}

	UInt32 PostProcessPipelinePass::GetInputCount(const ParameterList& parameters)
	{
		Result<std::string_view, ParameterList::Error> inputCountResult = parameters.GetStringViewParameter("InputCount");
		if (inputCountResult.IsOk())
		{
			std::string_view inputCountStr = inputCountResult.GetValue();

			long long inputCount;
			std::from_chars(inputCountStr.data(), inputCountStr.data() + inputCountStr.size(), inputCount);
			if (auto pidParse = std::from_chars(inputCountStr.data(), inputCountStr.data() + inputCountStr.size(), inputCount); pidParse.ec != std::errc())
				throw std::runtime_error("PostProcessPipelinePass InputCount is invalid");

			return SafeCaster(inputCount);
		}

		// TODO: Log error if key is present but not of the right type
		return 1;
	}

	std::string PostProcessPipelinePass::GetShaderName(const ParameterList& parameters)
	{
		Result<std::string, ParameterList::Error> shaderResult = parameters.GetStringParameter("Shader");
		if (shaderResult.IsOk())
			return std::move(shaderResult).GetValue();
		// TODO: Log error if key is present but not of the right type

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
