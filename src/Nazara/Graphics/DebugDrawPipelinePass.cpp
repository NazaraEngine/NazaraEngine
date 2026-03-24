// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/DebugDrawPipelinePass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/Renderer/RenderResources.hpp>

namespace Nz
{
	void DebugDrawPipelinePass::Prepare(FrameData& frameData)
	{
		DebugDrawer* debugDrawer = m_viewer->GetDebugDrawer();
		if (!debugDrawer)
			return;

		debugDrawer->SetViewerData(m_viewer->GetViewerInstance().GetViewProjMatrix());
		debugDrawer->Prepare(frameData.renderResources);

		frameData.renderResources.Execute([&](CommandBufferBuilder& builder)
		{
			debugDrawer->Upload(builder, frameData.renderResources);
		}, Nz::QueueType::Graphics);
	}

	FramePass& DebugDrawPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs)
	{
		if (inputOuputs.inputResources.size() != 1)
			throw std::runtime_error("one input expected");

		if (inputOuputs.outputResources.size() != 1)
			throw std::runtime_error("one output expected");

		FramePass& debugDrawPass = frameGraph.AddPass("Debug draw pass");
		debugDrawPass.AddInputAttachment(inputOuputs.inputResources[0].resourceIndex);
		debugDrawPass.AddOutputAttachment(inputOuputs.outputResources[0].resourceIndex);

		if (inputOuputs.depthStencilInput != InvalidResourceIndex)
			debugDrawPass.SetDepthStencilInput(inputOuputs.depthStencilInput);

		if (inputOuputs.depthStencilOutput != InvalidResourceIndex)
			debugDrawPass.SetDepthStencilOutput(inputOuputs.depthStencilOutput);

		debugDrawPass.SetExecutionCallback([&]
		{
			return (m_viewer->GetDebugDrawer()) ? FramePassExecution::UpdateAndExecute : FramePassExecution::Skip;
		});

		debugDrawPass.SetCommandCallback([this](CommandBufferBuilder& builder, const FramePassEnvironment& /*env*/)
		{
			Recti viewport = m_viewer->GetViewport();

			builder.SetScissor(viewport);
			builder.SetViewport(viewport);

			DebugDrawer* debugDrawer = m_viewer->GetDebugDrawer();
			NazaraAssert(debugDrawer);

			debugDrawer->Draw(builder);
			debugDrawer->Reset();
		});

		return debugDrawPass;
	}
}
