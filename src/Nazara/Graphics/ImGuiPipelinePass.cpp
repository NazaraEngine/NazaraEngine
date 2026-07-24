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
		if (inputOuputs.outputAttachments.size() != 1)
			throw std::runtime_error("one output expected");

		FramePass& imguiDrawPass = frameGraph.AddPass("ImGui draw pass");
		for (auto&& inputData : inputOuputs.inputAttachments)
			imguiDrawPass.AddInput(inputData.attachmentIndex);

		for (auto&& outputData : inputOuputs.outputAttachments)
		{
			std::size_t outputIndex = imguiDrawPass.AddOutput(outputData.attachmentIndex);

			std::visit(Overloaded{
				[](DontClear) {},
				[&](const Color& color)
				{
					imguiDrawPass.SetClearColor(outputIndex, color);
				},
				[&](ViewerClearValue)
				{
					imguiDrawPass.SetClearColor(outputIndex, m_viewer->GetClearColor());
				}
			}, outputData.clearColor);
		}

		if (inputOuputs.depthStencilInput != FramePipelinePass::InvalidAttachmentIndex)
			imguiDrawPass.SetDepthStencilInput(inputOuputs.depthStencilInput);
		else if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
		{
			std::visit(Overloaded{
				[](DontClear) {},
				[&](float depth)
				{
					imguiDrawPass.SetDepthStencilClear(depth, 0);
				},
				[&](ViewerClearValue)
				{
					imguiDrawPass.SetDepthStencilClear(m_viewer->GetClearDepth(), 0);
				}
			}, inputOuputs.clearDepth);
		}

		if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
			imguiDrawPass.SetDepthStencilOutput(inputOuputs.depthStencilOutput);

		if (inputOuputs.depthStencilInput != InvalidAttachmentIndex)
			imguiDrawPass.SetDepthStencilInput(inputOuputs.depthStencilInput);

		imguiDrawPass.SetExecutionCallback([&]
		{
			return FramePassExecution::UpdateAndExecute;
		});

		imguiDrawPass.SetRenderCallback([this](GpuCommandBufferBuilder& builder, const FramePassEnvironment& /*env*/)
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
