// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_DEBUGDRAWPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_DEBUGDRAWPIPELINEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <string>

namespace Nz
{
	class AbstractViewer;
	class FrameGraph;
	class FramePass;
	class FramePipeline;
	class RenderFrame;

	class NAZARA_GRAPHICS_API DebugDrawPipelinePass : public FramePipelinePass
	{
		public:
			DebugDrawPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters = {});
			DebugDrawPipelinePass(const DebugDrawPipelinePass&) = delete;
			DebugDrawPipelinePass(DebugDrawPipelinePass&&) = delete;
			~DebugDrawPipelinePass() = default;

			void Prepare(FrameData& frameData) override;

			FramePass& RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs) override;

			DebugDrawPipelinePass& operator=(const DebugDrawPipelinePass&) = delete;
			DebugDrawPipelinePass& operator=(DebugDrawPipelinePass&&) = delete;

		private:
			std::string m_passName;
			AbstractViewer* m_viewer;
			FramePipeline& m_pipeline;
	};
}

#include <Nazara/Graphics/DebugDrawPipelinePass.inl>

#endif // NAZARA_GRAPHICS_DEBUGDRAWPIPELINEPASS_HPP
