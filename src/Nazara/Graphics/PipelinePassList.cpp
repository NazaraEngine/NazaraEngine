// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PipelinePassList.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipelinePassRegistry.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	bool PipelinePassListParams::IsValid() const
	{
		return true;
	}

	std::vector<std::unique_ptr<FramePipelinePass>> PipelinePassList::BuildPasses(FramePipelinePass::PassData& passData) const
	{
		auto& passRegistry = Graphics::Instance()->GetFramePipelinePassRegistry();

		std::vector<std::unique_ptr<FramePipelinePass>> passes;
		for (const Pass& pass : m_passes)
			passes.emplace_back(passRegistry.BuildPass(pass.implIndex, passData, pass.name, pass.parameterList));

		return passes;
	}

	std::size_t PipelinePassList::RegisterPasses(const std::vector<std::unique_ptr<FramePipelinePass>>& passes, FrameGraph& frameGraph, std::optional<unsigned int> viewerIndex, const FunctionRef<void(std::size_t passIndex, FramePass& framePass, FramePipelinePassFlags flags)>& passCallback) const
	{
		NazaraAssert(m_passes.size() == passes.size(), "pass vector size doesn't match passlist size");

		StackArray<std::size_t> attachmentIndices = NazaraStackArrayNoInit(std::size_t, m_attachments.size());
		auto GetAttachmentIndex = [&](std::size_t attachmentIndex)
		{
			if (attachmentIndex == NoAttachment)
				return NoAttachment;

			assert(attachmentIndex < m_attachments.size());
			return attachmentIndices[attachmentIndex];
		};

		for (std::size_t i = 0; i < m_attachments.size(); ++i)
		{
			attachmentIndices[i] = std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, FramePassAttachment>)
				{
					if (arg.size == FramePassAttachmentSize::ViewerTargetFactor)
					{
						if (!viewerIndex)
							throw std::runtime_error(Format("no viewer index but attachment {} depends on viewer target size", arg.name));

						FramePassAttachment attachment = arg;
						attachment.viewerIndex = *viewerIndex;
						return frameGraph.AddAttachment(attachment);
					}
					else
						return frameGraph.AddAttachment(arg);
				}
				else if constexpr (std::is_same_v<T, AttachmentProxy>)
					return frameGraph.AddAttachmentProxy(arg.name, GetAttachmentIndex(arg.attachmentIndex));
				else
					static_assert(AlwaysFalse<T>(), "unhandled case");

			}, m_attachments[i]);
		}

		for (std::size_t passIndex = 0; passIndex < passes.size(); ++passIndex)
		{
			const Pass& passData = m_passes[passIndex];

			std::array<std::size_t, MaxPassAttachment> inputs;
			for (std::size_t i = 0; i < passData.inputs.size(); ++i)
				inputs[i] = GetAttachmentIndex(passData.inputs[i]);

			std::array<std::size_t, MaxPassAttachment> outputs;
			for (std::size_t i = 0; i < passData.outputs.size(); ++i)
				outputs[i] = GetAttachmentIndex(passData.outputs[i]);

			FramePipelinePass::PassInputOuputs passInputOuputs;
			passInputOuputs.depthStencilInput = GetAttachmentIndex(passData.depthStencilInput);
			passInputOuputs.depthStencilOutput = GetAttachmentIndex(passData.depthStencilOutput);
			passInputOuputs.inputAttachments = std::span(inputs.data(), passData.inputs.size());
			passInputOuputs.outputAttachments = std::span(outputs.data(), passData.outputs.size());

			FramePass& framePass = passes[passIndex]->RegisterToFrameGraph(frameGraph, passInputOuputs);
			if (passCallback)
				passCallback(passIndex, framePass, passData.flags);
		}

		return GetAttachmentIndex(m_finalOutputAttachment);
	}

	std::shared_ptr<PipelinePassList> PipelinePassList::LoadFromFile(const std::filesystem::path& filePath, const PipelinePassListParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssert(graphics, "Graphics module has not been initialized");

		return graphics->GetPipelinePassListLoader().LoadFromFile(filePath, params);
	}

	std::shared_ptr<PipelinePassList> PipelinePassList::LoadFromMemory(const void* data, std::size_t size, const PipelinePassListParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssert(graphics, "Graphics module has not been initialized");

		return graphics->GetPipelinePassListLoader().LoadFromMemory(data, size, params);
	}

	std::shared_ptr<PipelinePassList> PipelinePassList::LoadFromStream(Stream& stream, const PipelinePassListParams& params)
	{
		Graphics* graphics = Graphics::Instance();
		NazaraAssert(graphics, "Graphics module has not been initialized");

		return graphics->GetPipelinePassListLoader().LoadFromStream(stream, params);
	}
}
