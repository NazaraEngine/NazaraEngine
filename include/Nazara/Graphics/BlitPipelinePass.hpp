// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_BLITPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_BLITPIPELINEPASS_HPP

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

	class NAZARA_GRAPHICS_API BlitPipelinePass : public FramePipelinePass
	{
		public:
			BlitPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters = {});
			BlitPipelinePass(const BlitPipelinePass&) = delete;
			BlitPipelinePass(BlitPipelinePass&&) = delete;
			~BlitPipelinePass() = default;

			void Prepare(FrameData& frameData) override;

			FramePass& RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs) override;

			BlitPipelinePass& operator=(const BlitPipelinePass&) = delete;
			BlitPipelinePass& operator=(BlitPipelinePass&&) = delete;

		private:
			std::string m_passName;
			AbstractViewer* m_viewer;
			FramePipeline& m_pipeline;
	};
}

#include <Nazara/Graphics/BlitPipelinePass.inl>

#endif // NAZARA_GRAPHICS_BLITPIPELINEPASS_HPP
