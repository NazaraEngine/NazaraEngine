// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/FramePass.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline FramePass::FramePass(FrameGraph& owner, std::size_t passId, std::string name) :
	m_depthStencilInput(InvalidAttachmentId),
	m_depthStencilOutput(InvalidAttachmentId),
	m_passId(passId),
	m_name(std::move(name)),
	m_owner(owner)
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

	inline auto FramePass::GetCommandCallback() const -> const CommandCallback&
	{
		return m_commandCallback;
	}

	inline auto FramePass::GetDepthStencilClear() const -> const std::optional<DepthStencilClear>&
	{
		return m_depthStencilClear;
	}

	inline std::size_t FramePass::GetDepthStencilInput() const
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

	inline void FramePass::SetExecutionCallback(CommandCallback callback)
	{
		m_commandCallback = std::move(callback);
	}

	inline void FramePass::SetDepthStencilInput(std::size_t attachmentId)
	{
		m_depthStencilInput = attachmentId;
	}

	inline void FramePass::SetDepthStencilOutput(std::size_t attachmentId)
	{
		m_depthStencilOutput = attachmentId;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
