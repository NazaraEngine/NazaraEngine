// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DebugDrawPipelinePass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	DebugDrawPipelinePass::DebugDrawPipelinePass(FramePipeline& owner, AbstractViewer* viewer) :
	m_viewer(viewer),
	m_pipeline(owner)
	{
	}

	void DebugDrawPipelinePass::Prepare(RenderFrame& renderFrame)
	{
		DebugDrawer& debugDrawer = m_pipeline.GetDebugDrawer();
		debugDrawer.SetViewerData(m_viewer->GetViewerInstance().GetViewProjMatrix());
		debugDrawer.Prepare(renderFrame);
	}

	void DebugDrawPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, std::size_t inputColorBufferIndex, std::size_t outputColorBufferIndex)
	{
		FramePass& debugDrawPass = frameGraph.AddPass("Debug draw pass");
		debugDrawPass.AddInput(inputColorBufferIndex);
		debugDrawPass.AddOutput(outputColorBufferIndex);

		debugDrawPass.SetExecutionCallback([&]()
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
	}
}
