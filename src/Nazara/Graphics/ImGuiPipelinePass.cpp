// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ImGuiPipelinePass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipelinePassRegistry.hpp>
#include <Nazara/Renderer/Plugins/ImGuiPlugin.hpp>

namespace Nz
{
	void ImGuiPipelinePass::Prepare(FrameData& frameData)
	{
		m_imguiPlugin.Prepare(m_imguiContext, frameData.renderResources);
	}

	FramePass& ImGuiPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs)
	{
		if (inputOuputs.inputAttachments.size() != 1)
			throw std::runtime_error("one input expected");

		if (inputOuputs.outputAttachments.size() != 1)
			throw std::runtime_error("one output expected");

		FramePass& imguiDrawPass = frameGraph.AddPass("ImGui draw pass");
		imguiDrawPass.AddInput(inputOuputs.inputAttachments[0].attachmentIndex);
		imguiDrawPass.AddOutput(inputOuputs.outputAttachments[0].attachmentIndex);

		if (inputOuputs.depthStencilInput != InvalidAttachmentIndex)
			imguiDrawPass.SetDepthStencilInput(inputOuputs.depthStencilInput);

		if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
			imguiDrawPass.SetDepthStencilOutput(inputOuputs.depthStencilOutput);

		imguiDrawPass.SetExecutionCallback([&]
		{
			return FramePassExecution::UpdateAndExecute;
		});

		imguiDrawPass.SetCommandCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& /*env*/)
		{
			Recti viewport = m_viewer->GetViewport();

			builder.SetScissor(viewport);
			builder.SetViewport(viewport);

			m_imguiPlugin.Draw(m_imguiContext, builder);
		});

		return imguiDrawPass;
	}

	void ImGuiPipelinePass::RegisterPass(FramePipelinePassRegistry& pipelinePassRegistry, ImGuiPlugin& imguiPlugin, ImGuiContext* context)
	{
		pipelinePassRegistry.RegisterPass<ImGuiPipelinePass>("ImGui", { "Input" }, { "Output" }, std::ref(imguiPlugin), context);
	}
}
