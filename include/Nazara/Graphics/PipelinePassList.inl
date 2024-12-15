// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz
{
	inline std::size_t PipelinePassList::AddAttachment(FramePassAttachment attachment)
	{
		std::size_t index = m_attachments.size();
		m_attachments.emplace_back(std::move(attachment));

		return index;
	}

	inline std::size_t PipelinePassList::AddAttachmentProxy(std::string name, std::size_t attachmentIndex)
	{
		std::size_t index = m_attachments.size();
		m_attachments.emplace_back(AttachmentProxy{ std::move(name), attachmentIndex });

		return index;
	}

	inline std::size_t PipelinePassList::AddPass(std::string name, std::size_t implIndex, ParameterList parameterList)
	{
		std::size_t index = m_passes.size();
		auto& pass = m_passes.emplace_back();
		pass.name = std::move(name);
		pass.implIndex = implIndex;
		pass.parameterList = std::move(parameterList);

		return index;
	}

	inline void PipelinePassList::EnablePassFlags(std::size_t passIndex, FramePipelinePassFlags flags)
	{
		assert(passIndex < m_passes.size());
		auto& pass = m_passes[passIndex];
		pass.flags = flags;
	}

	inline void PipelinePassList::SetFinalOutput(std::size_t attachmentIndex)
	{
		m_finalOutputAttachment = attachmentIndex;
	}

	inline void PipelinePassList::SetPassDepthClearValue(std::size_t passIndex, float clearDepth)
	{
		assert(passIndex < m_passes.size());
		auto& pass = m_passes[passIndex];
		pass.clearDepth = clearDepth;
	}

	inline void PipelinePassList::SetPassDepthClearValue(std::size_t passIndex, FramePipelinePass::ViewerClearValue)
	{
		assert(passIndex < m_passes.size());
		auto& pass = m_passes[passIndex];
		pass.clearDepth = FramePipelinePass::ViewerClearValue{};
	}

	inline void PipelinePassList::SetPassDepthStencilInput(std::size_t passIndex, std::size_t attachmentIndex)
	{
		assert(passIndex < m_passes.size());
		auto& pass = m_passes[passIndex];
		pass.depthStencilInput = attachmentIndex;
	}

	inline void PipelinePassList::SetPassDepthStencilOutput(std::size_t passIndex, std::size_t attachmentIndex)
	{
		assert(passIndex < m_passes.size());
		auto& pass = m_passes[passIndex];
		pass.depthStencilOutput = attachmentIndex;
	}
	inline void PipelinePassList::SetPassInput(std::size_t passIndex, std::size_t inputIndex, std::size_t attachmentIndex)
	{
		assert(passIndex < m_passes.size());
		auto& pass = m_passes[passIndex];
		assert(inputIndex < MaxPassAttachment);

		if (inputIndex >= pass.inputs.size())
			pass.inputs.resize(inputIndex + 1, NoAttachment);

		pass.inputs[inputIndex] = attachmentIndex;
	}

	inline void PipelinePassList::SetPassOutput(std::size_t passIndex, std::size_t outputIndex, std::size_t attachmentIndex)
	{
		assert(passIndex < m_passes.size());
		auto& pass = m_passes[passIndex];
		assert(outputIndex < MaxPassAttachment);

		if (outputIndex >= pass.outputs.size())
			pass.outputs.resize(outputIndex + 1);

		pass.outputs[outputIndex].attachmentIndex = attachmentIndex;
	}

	inline void PipelinePassList::SetPassOutputClearColor(std::size_t passIndex, std::size_t outputIndex, const Color& clearColor)
	{
		assert(passIndex < m_passes.size());
		auto& pass = m_passes[passIndex];
		assert(outputIndex < MaxPassAttachment);
		pass.outputs[outputIndex].clearColor = clearColor;
	}

	inline void PipelinePassList::SetPassOutputClearColor(std::size_t passIndex, std::size_t outputIndex, FramePipelinePass::ViewerClearValue)
	{
		assert(passIndex < m_passes.size());
		auto& pass = m_passes[passIndex];
		assert(outputIndex < MaxPassAttachment);
		pass.outputs[outputIndex].clearColor = FramePipelinePass::ViewerClearValue{};
	}
}

