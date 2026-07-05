// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FORWARDPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_FORWARDPIPELINEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Graphics/BaseElementRenderPipelinePass.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API ForwardPipelinePass : public BaseElementRenderPipelinePass
	{
		public:
			ForwardPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters = {});
			ForwardPipelinePass(const ForwardPipelinePass&) = delete;
			ForwardPipelinePass(ForwardPipelinePass&&) = delete;
			~ForwardPipelinePass() = default;

			FramePass& RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs) override;

			ForwardPipelinePass& operator=(const ForwardPipelinePass&) = delete;
			ForwardPipelinePass& operator=(ForwardPipelinePass&&) = delete;

		private:
			std::string m_passName;
			AbstractViewer* m_viewer;
			FramePipeline& m_pipeline;
	};
}

#include <Nazara/Graphics/ForwardPipelinePass.inl>

#endif // NAZARA_GRAPHICS_FORWARDPIPELINEPASS_HPP
