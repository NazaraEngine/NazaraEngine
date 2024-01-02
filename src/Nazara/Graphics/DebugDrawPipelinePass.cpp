// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DebugDrawPipelinePass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	void DebugDrawPipelinePass::Prepare(FrameData& frameData)
	{
		DebugDrawer& debugDrawer = m_pipeline.GetDebugDrawer();
		debugDrawer.SetViewerData(m_viewer->GetViewerInstance().GetViewProjMatrix());
		debugDrawer.Prepare(frameData.renderResources);
	}

	FramePass& DebugDrawPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs)
	{
		if (inputOuputs.inputAttachments.size() != 1)
			throw std::runtime_error("one input expected");

		if (inputOuputs.outputAttachments.size() != 1)
			throw std::runtime_error("one output expected");

		FramePass& debugDrawPass = frameGraph.AddPass("Debug draw pass");
		debugDrawPass.AddInput(inputOuputs.inputAttachments[0]);
		debugDrawPass.AddOutput(inputOuputs.outputAttachments[0]);
		
		if (inputOuputs.depthStencilInput != InvalidAttachmentIndex)
			debugDrawPass.SetDepthStencilInput(inputOuputs.depthStencilInput);

		if (inputOuputs.depthStencilOutput != InvalidAttachmentIndex)
			debugDrawPass.SetDepthStencilOutput(inputOuputs.depthStencilOutput);

		debugDrawPass.SetExecutionCallback([&]
		{
			return FramePassExecution::UpdateAndExecute;
		});

		debugDrawPass.SetCommandCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& /*env*/)
		{
			Recti viewport = m_viewer->GetViewport();

			builder.SetScissor(viewport);
			builder.SetViewport(viewport);

			DebugDrawer& debugDrawer = m_pipeline.GetDebugDrawer();
			debugDrawer.Draw(builder);
		});

		return debugDrawPass;
	}
}
