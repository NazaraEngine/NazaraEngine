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
	std::vector<std::unique_ptr<FramePipelinePass>> PipelinePassList::BuildPasses(FramePipelinePass::PassData& passData) const
	{
		auto& passRegistry = Graphics::Instance()->GetFramePipelinePassRegistry();

		std::vector<std::unique_ptr<FramePipelinePass>> passes;
		for (const Pass& pass : m_passes)
			passes.emplace_back(passRegistry.BuildPass(pass.implIndex, passData, pass.name, pass.parameterList));

		return passes;
	}

	std::size_t PipelinePassList::RegisterPasses(const std::vector<std::unique_ptr<FramePipelinePass>>& passes, FrameGraph& frameGraph) const
	{
		NazaraAssert(m_passes.size() == passes.size(), "pass vector size doesn't match passlist size");

		StackArray<std::size_t> attachmentIndices = NazaraStackArrayNoInit(std::size_t, m_attachments.size());
		for (std::size_t i = 0; i < m_attachments.size(); ++i)
			attachmentIndices[i] = frameGraph.AddAttachment(m_attachments[i]);

		auto GetAttachmentIndex = [&](std::size_t attachmentIndex)
		{
			if (attachmentIndex == NoAttachment)
				return NoAttachment;

			assert(attachmentIndex < m_attachments.size());
			return attachmentIndices[attachmentIndex];
		};

		for (std::size_t i = 0; i < passes.size(); ++i)
		{
			const Pass& passData = m_passes[i];

			std::array<std::size_t, MaxPassAttachment> inputs;
			for (std::size_t j = 0; j < passData.inputs.size(); ++j)
				inputs[j] = GetAttachmentIndex(passData.inputs[j]);

			std::array<std::size_t, MaxPassAttachment> outputs;
			for (std::size_t j = 0; j < passData.outputs.size(); ++j)
				outputs[j] = GetAttachmentIndex(passData.outputs[j]);

			FramePipelinePass::PassInputOuputs passInputOuputs;
			passInputOuputs.depthStencilInput = GetAttachmentIndex(passData.depthStencilInput);
			passInputOuputs.depthStencilOutput = GetAttachmentIndex(passData.depthStencilOutput);
			passInputOuputs.inputAttachments = inputs.data();
			passInputOuputs.inputCount = passData.inputs.size();
			passInputOuputs.outputAttachments = outputs.data();
			passInputOuputs.outputCount = passData.outputs.size();

			passes[i]->RegisterToFrameGraph(frameGraph, passInputOuputs);
		}

		return GetAttachmentIndex(m_finalOutputAttachment);
	}
}
