// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_DEBUGDRAWPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_DEBUGDRAWPIPELINEPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Math/Frustum.hpp>

namespace Nz
{
	class AbstractViewer;
	class FrameGraph;
	class FramePass;
	class FramePipeline;
	class Material;

	class NAZARA_GRAPHICS_API DebugDrawPipelinePass : public FramePipelinePass
	{
		public:
			DebugDrawPipelinePass(FramePipeline& owner, AbstractViewer* viewer);
			DebugDrawPipelinePass(const DebugDrawPipelinePass&) = delete;
			DebugDrawPipelinePass(DebugDrawPipelinePass&&) = delete;
			~DebugDrawPipelinePass() = default;

			void Prepare(RenderFrame& renderFrame);

			FramePass& RegisterToFrameGraph(FrameGraph& frameGraph, std::size_t inputColorBufferIndex, std::size_t outputColorBufferIndex);

			DebugDrawPipelinePass& operator=(const DebugDrawPipelinePass&) = delete;
			DebugDrawPipelinePass& operator=(DebugDrawPipelinePass&&) = delete;

		private:
			AbstractViewer* m_viewer;
			FramePipeline& m_pipeline;
	};
}

#include <Nazara/Graphics/DebugDrawPipelinePass.inl>

#endif // NAZARA_GRAPHICS_DEBUGDRAWPIPELINEPASS_HPP
