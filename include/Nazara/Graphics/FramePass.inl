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

	inline std::size_t FramePass::AddInputAttachment(std::size_t attachmentId)
	{
		assert(attachmentId != InvalidAttachmentId);

		std::size_t inputIndex = m_attachmentInputs.size();
		auto& input = m_attachmentInputs.emplace_back();
		input.attachmentId = attachmentId;

		return inputIndex;
	}

	inline std::size_t FramePass::AddOutputAttachment(std::size_t attachmentId)
	{
		assert(attachmentId != InvalidAttachmentId);

		std::size_t outputIndex = m_attachmentOutputs.size();
		auto& output = m_attachmentOutputs.emplace_back();
		output.attachmentId = attachmentId;

		return outputIndex;
	}
	template<typename F>
	void FramePass::ForEachAttachment(F&& func, bool singleDSInputOutputCall) const
	{
		for (const auto& input : m_attachmentInputs)
			func(input.attachmentId);

		for (const auto& output : m_attachmentOutputs)
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

	inline auto FramePass::GetAttachmentInputs() const -> const AttachmentInputs&
	{
		return m_attachmentInputs;
	}

	inline auto FramePass::GetAttachmentOutputs() const -> const AttachmentOutputs&
	{
		return m_attachmentOutputs;
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

	inline const std::string& FramePass::GetName() const
	{
		return m_name;
	}

	inline std::size_t FramePass::GetPassId() const
	{
		return m_passId;
	}

	inline void FramePass::SetAttachmentInputAccess(std::size_t inputIndex, TextureLayout layout, PipelineStageFlags stageFlags, MemoryAccessFlags accessFlags)
	{
		assert(inputIndex < m_attachmentInputs.size());
		m_attachmentInputs[inputIndex].accessFlags = accessFlags;
		m_attachmentInputs[inputIndex].layout = layout;
		m_attachmentInputs[inputIndex].stageFlags = stageFlags;
	}

	inline void FramePass::SetAttachmentInputAssumedLayout(std::size_t inputIndex, TextureLayout layout)
	{
		assert(inputIndex < m_attachmentInputs.size());
		m_attachmentInputs[inputIndex].assumedLayout = layout;
	}

	inline void FramePass::SetAttachmentInputUsage(std::size_t inputIndex, TextureUsageFlags usageFlags)
	{
		assert(inputIndex < m_attachmentInputs.size());
		m_attachmentInputs[inputIndex].textureUsageFlags = usageFlags;
	}

	inline void FramePass::SetAttachmentOutputAccess(std::size_t outputIndex, TextureLayout layout, PipelineStageFlags stageFlags, MemoryAccessFlags accessFlags)
	{
		assert(outputIndex < m_attachmentOutputs.size());
		m_attachmentOutputs[outputIndex].accessFlags = accessFlags;
		m_attachmentOutputs[outputIndex].layout = layout;
		m_attachmentOutputs[outputIndex].stageFlags = stageFlags;
	}

	inline void FramePass::SetAttachmentOutputUsage(std::size_t outputIndex, TextureUsageFlags usageFlags)
	{
		assert(outputIndex < m_attachmentOutputs.size());
		m_attachmentOutputs[outputIndex].textureUsageFlags = usageFlags;
	}

	inline void FramePass::SetAttachmentReadInput(std::size_t inputIndex, bool doesRead)
	{
		assert(inputIndex < m_attachmentInputs.size());
		m_attachmentInputs[inputIndex].doesRead = doesRead;
	}

	inline void FramePass::SetCommandCallback(CommandCallback callback)
	{
		m_commandCallback = std::move(callback);
	}

	inline void FramePass::SetClearColor(std::size_t outputIndex, const std::optional<Color>& color)
	{
		assert(outputIndex < m_attachmentOutputs.size());
		m_attachmentOutputs[outputIndex].clearColor = color;
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
