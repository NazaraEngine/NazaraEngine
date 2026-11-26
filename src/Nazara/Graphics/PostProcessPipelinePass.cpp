// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/PostProcessPipelinePass.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <charconv>

namespace Nz
{
	PostProcessPipelinePass::PostProcessPipelinePass(PassData& /*passData*/, std::string passName, Params params) :
	m_passName(std::move(passName)),
	m_params(std::move(params)),
	m_shader(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, std::move(m_params.shaderName))
	{
		GpuPipelineLayoutInfo layoutInfo;
		for (UInt32 inputIndex = 0; inputIndex < m_params.inputCount; ++inputIndex)
		{
			layoutInfo.bindings.push_back(GpuPipelineLayoutInfo::Binding{
				0, inputIndex, 1,
				ShaderBindingType::Sampler,
				nzsl::ShaderStageType::Fragment
			});
		}

		std::shared_ptr<GpuDevice> renderDevice = Graphics::Instance()->GetGpuDevice();
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
		if (inputOuputs.inputAttachments.size() != m_params.inputCount)
			throw std::runtime_error(Format("{} input(s) expected", m_params.inputCount));

		if (inputOuputs.outputAttachments.size() != 1)
			throw std::runtime_error("one output expected");

		if (inputOuputs.depthStencilInput != InvalidAttachmentIndex)
			throw std::runtime_error("unexpected depth-stencil output");

		if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
			throw std::runtime_error("unexpected depth-stencil output");

		FramePass& postProcess = frameGraph.AddPass(m_passName);

		Nz::HybridVector<std::size_t, 2> inputIndices(m_params.inputCount);
		for (std::size_t i = 0; i < m_params.inputCount; ++i)
		{
			inputIndices[i] = inputOuputs.inputAttachments[i].attachmentIndex;
			postProcess.AddInput(inputIndices[i]);
		}

		postProcess.AddOutput(inputOuputs.outputAttachments[0].attachmentIndex);

		postProcess.SetExecutionCallback([&]
		{
			return (m_rebuildFramePass) ? FramePassExecution::UpdateAndExecute : FramePassExecution::Execute;
		});

		postProcess.SetRenderCallback([this, inputIndices](GpuCommandBufferBuilder& builder, const FramePassEnvironment& env)
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

	auto PostProcessPipelinePass::BuildParams(const ParameterList& parameters) -> Params
	{
		auto DecodeBlendFunc = [&](std::string_view parameterName, BlendFunc defaultBlendFunc) -> BlendFunc
		{
			constexpr auto s_blendFunc = frozen::make_unordered_map<frozen::string, BlendFunc>({
				{ "ConstantColor", BlendFunc::ConstantColor },
				{ "ConstantAlpha", BlendFunc::ConstantAlpha },
				{ "DstAlpha", BlendFunc::DstAlpha },
				{ "DstColor", BlendFunc::DstColor },
				{ "SrcAlpha", BlendFunc::SrcAlpha },
				{ "SrcColor", BlendFunc::SrcColor },
				{ "InvConstantColor", BlendFunc::InvConstantColor },
				{ "InvConstantAlpha", BlendFunc::InvConstantAlpha },
				{ "InvDstAlpha", BlendFunc::InvDstAlpha },
				{ "InvDstColor", BlendFunc::InvDstColor },
				{ "InvSrcAlpha", BlendFunc::InvSrcAlpha },
				{ "InvSrcColor", BlendFunc::InvSrcColor },
				{ "One", BlendFunc::One },
				{ "Zero", BlendFunc::Zero }
			});

			Result<std::string_view, ParameterList::Error> result = parameters.GetStringViewParameter(parameterName);
			if (result.IsOk())
			{
				auto it = s_blendFunc.find(result.GetValue());
				if (it == s_blendFunc.end())
					throw std::runtime_error(Format("PostProcessPipelinePass {} has invalid BlendFunc \"{}\"", parameterName, result.GetValue()));

				return it->second;
			}

			return defaultBlendFunc;
		};

		auto DecodeBlendEquation = [&](std::string_view parameterName, BlendEquation defaultBlendEquation) -> BlendEquation
		{
			constexpr auto s_blendEq = frozen::make_unordered_map<frozen::string, BlendEquation>({
				{ "Add", BlendEquation::Add },
				{ "Max", BlendEquation::Max },
				{ "Min", BlendEquation::Min },
				{ "ReverseSubtract", BlendEquation::ReverseSubtract },
				{ "Subtract", BlendEquation::Subtract }
			});

			Result<std::string_view, ParameterList::Error> result = parameters.GetStringViewParameter(parameterName);
			if (result.IsOk())
			{
				auto it = s_blendEq.find(result.GetValue());
				if (it == s_blendEq.end())
					throw std::runtime_error(Format("PostProcessPipelinePass {} has invalid BlendEquation \"{}\"", parameterName, result.GetValue()));

				return it->second;
			}

			return defaultBlendEquation;
		};

		Params params;
		params.dstAlpha = DecodeBlendFunc("DstAlpha", params.dstAlpha);
		params.dstColor = DecodeBlendFunc("DstColor", params.dstColor);
		params.srcAlpha = DecodeBlendFunc("SrcAlpha", params.srcAlpha);
		params.srcColor = DecodeBlendFunc("SrcColor", params.srcColor);
		params.modeAlpha = DecodeBlendEquation("ModeAlpha", params.modeAlpha);
		params.modeColor = DecodeBlendEquation("ModeColor", params.modeColor);

		Result<bool, ParameterList::Error> blendingResult = parameters.GetBooleanParameter("Blending"); //< will convert
		if (blendingResult.IsOk())
			params.blending = blendingResult.GetValue();

		// InputCount
		Result<std::string_view, ParameterList::Error> inputCountResult = parameters.GetStringViewParameter("InputCount");
		if (inputCountResult.IsOk())
		{
			std::string_view inputCountStr = inputCountResult.GetValue();

			long long inputCount;
			std::from_chars(inputCountStr.data(), inputCountStr.data() + inputCountStr.size(), inputCount);
			if (auto pidParse = std::from_chars(inputCountStr.data(), inputCountStr.data() + inputCountStr.size(), inputCount); pidParse.ec != std::errc())
				throw std::runtime_error("PostProcessPipelinePass InputCount is invalid");

			params.inputCount = SafeCaster(inputCount);
		}

		// Shader
		Result<std::string, ParameterList::Error> shaderResult = parameters.GetStringParameter("Shader");
		if (shaderResult.IsOk())
			params.shaderName = std::move(shaderResult).GetValue();

		return params;
	}

	void PostProcessPipelinePass::BuildPipeline()
	{
		const std::shared_ptr<GpuDevice>& renderDevice = Graphics::Instance()->GetGpuDevice();

		RenderPipelineInfo pipelineInfo;
		pipelineInfo.blending = m_params.blending;
		pipelineInfo.blend.dstAlpha = m_params.dstAlpha;
		pipelineInfo.blend.dstColor = m_params.dstColor;
		pipelineInfo.blend.modeAlpha = m_params.modeAlpha;
		pipelineInfo.blend.modeColor = m_params.modeColor;
		pipelineInfo.blend.srcAlpha = m_params.srcAlpha;
		pipelineInfo.blend.srcColor = m_params.srcColor;
		pipelineInfo.pipelineLayout = m_renderPipelineLayout;
		pipelineInfo.shaderModules.push_back(m_shader.Get({}));

		m_nextRenderPipeline = renderDevice->InstantiateRenderPipeline(pipelineInfo);
	}
}
