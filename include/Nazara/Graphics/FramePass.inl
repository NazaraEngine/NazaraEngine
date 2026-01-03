// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz
{
	inline FramePass::FramePass(FrameGraph& /*owner*/, std::size_t passId, std::string name) :
	m_depthStencilOutput(InvalidAttachmentId),
	m_passId(passId),
	m_name(std::move(name))
	{
	}

	inline std::size_t FramePass::AddInput(std::size_t attachmentId)
	{
		assert(attachmentId != InvalidAttachmentId);

		std::size_t inputIndex = m_inputs.size();
		auto& input = m_inputs.emplace_back();
		input.attachmentId = attachmentId;

		return inputIndex;
	}

	inline std::size_t FramePass::AddOutput(std::size_t attachmentId)
	{
		assert(attachmentId != InvalidAttachmentId);

		std::size_t outputIndex = m_outputs.size();
		auto& output = m_outputs.emplace_back();
		output.attachmentId = attachmentId;

		return outputIndex;
	}
	template<typename F>
	void FramePass::ForEachAttachment(F&& func, bool singleDSInputOutputCall) const
	{
		for (const auto& input : m_inputs)
			func(input.attachmentId);

		for (const auto& output : m_outputs)
			func(output.attachmentId);

		if (m_depthStencilInput)
		{
			func(m_depthStencilInput->attachmentId);

			if (m_depthStencilOutput != FramePass::InvalidAttachmentId && m_depthStencilOutput != m_depthStencilInput->attachmentId)
			{
				if (!singleDSInputOutputCall || m_depthStencilOutput != m_depthStencilInput->attachmentId)
					func(m_depthStencilOutput);
			}
		}
		else if (m_depthStencilOutput != FramePass::InvalidAttachmentId)
			func(m_depthStencilOutput);
	}

	inline auto FramePass::GetCommandCallback() const -> const CommandCallback&
	{
		return m_commandCallback;
	}

	inline auto FramePass::GetDepthStencilClear() const -> const std::optional<DepthStencilClear>&
	{
		return m_depthStencilClear;
	}

	inline auto FramePass::GetDepthStencilInput() const -> const std::optional<DepthStencilInput>&
	{
		return m_depthStencilInput;
	}

	inline std::size_t FramePass::GetDepthStencilOutput() const
	{
		return m_depthStencilOutput;
	}

	inline auto FramePass::GetExecutionCallback() const -> const ExecutionCallback&
	{
		return m_executionCallback;
	}

	inline auto FramePass::GetInputs() const -> const std::vector<Input>&
	{
		return m_inputs;
	}

	inline const std::string& FramePass::GetName() const
	{
		return m_name;
	}

	inline auto FramePass::GetOutputs() const -> const std::vector<Output>&
	{
		return m_outputs;
	}

	inline std::size_t FramePass::GetPassId() const
	{
		return m_passId;
	}

	inline void FramePass::SetCommandCallback(CommandCallback callback)
	{
		m_commandCallback = std::move(callback);
	}

	inline void FramePass::SetClearColor(std::size_t outputIndex, const std::optional<Color>& color)
	{
		assert(outputIndex < m_outputs.size());
		m_outputs[outputIndex].clearColor = color;
	}

	inline void FramePass::SetDepthStencilClear(float depth, UInt32 stencil)
	{
		auto& dsClear = m_depthStencilClear.emplace();
		dsClear.depth = depth;
		dsClear.stencil = stencil;
	}

	inline void FramePass::SetExecutionCallback(ExecutionCallback callback)
	{
		m_executionCallback = std::move(callback);
	}

	inline void FramePass::SetInputAccess(std::size_t inputIndex, TextureLayout layout, PipelineStageFlags stageFlags, MemoryAccessFlags accessFlags)
	{
		assert(inputIndex < m_inputs.size());
		m_inputs[inputIndex].accessFlags = accessFlags;
		m_inputs[inputIndex].layout = layout;
		m_inputs[inputIndex].stageFlags = stageFlags;
	}

	inline void FramePass::SetInputAssumedLayout(std::size_t inputIndex, TextureLayout layout)
	{
		assert(inputIndex < m_inputs.size());
		m_inputs[inputIndex].assumedLayout = layout;
	}

	inline void FramePass::SetInputUsage(std::size_t inputIndex, TextureUsageFlags usageFlags)
	{
		assert(inputIndex < m_inputs.size());
		m_inputs[inputIndex].textureUsageFlags = usageFlags;
	}

	inline void FramePass::SetOutputAccess(std::size_t outputIndex, TextureLayout layout, PipelineStageFlags stageFlags, MemoryAccessFlags accessFlags)
	{
		assert(outputIndex < m_outputs.size());
		m_outputs[outputIndex].accessFlags = accessFlags;
		m_outputs[outputIndex].layout = layout;
		m_outputs[outputIndex].stageFlags = stageFlags;
	}

	inline void FramePass::SetOutputUsage(std::size_t outputIndex, TextureUsageFlags usageFlags)
	{
		assert(outputIndex < m_outputs.size());
		m_outputs[outputIndex].textureUsageFlags = usageFlags;
	}

	inline void FramePass::SetReadInput(std::size_t inputIndex, bool doesRead)
	{
		assert(inputIndex < m_inputs.size());
		m_inputs[inputIndex].doesRead = doesRead;
	}

	inline void FramePass::SetDepthStencilInput(std::size_t attachmentId, TextureUsage attachmentUsage)
	{
		NazaraAssertMsg(attachmentUsage == TextureUsage::DepthStencilAttachment || attachmentUsage == TextureUsage::ShaderSampling, "unexpected usage, depth-stencil input can be used as attachment or as sampled texture");
		m_depthStencilInput = DepthStencilInput{ attachmentId, attachmentUsage };
	}

	inline void FramePass::SetDepthStencilOutput(std::size_t attachmentId)
	{
		m_depthStencilOutput = attachmentId;
	}
}
