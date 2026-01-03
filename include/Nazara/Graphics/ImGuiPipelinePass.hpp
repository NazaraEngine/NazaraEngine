// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_IMGUIPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_IMGUIPIPELINEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <string>

struct ImGuiContext;

namespace Nz
{
	class AbstractViewer;
	class FrameGraph;
	class FramePass;
	class FramePipeline;
	class FramePipelinePassRegistry;
	class ImGuiPlugin;
	class RenderFrame;

	class NAZARA_GRAPHICS_API ImGuiPipelinePass : public FramePipelinePass
	{
		public:
			inline ImGuiPipelinePass(PassData& passData, std::string passName, ImGuiPlugin& imguiPlugin, ImGuiContext* context, const ParameterList& parameters = {});
			ImGuiPipelinePass(const ImGuiPipelinePass&) = delete;
			ImGuiPipelinePass(ImGuiPipelinePass&&) = delete;
			~ImGuiPipelinePass() = default;

			void Prepare(FrameData& frameData) override;

			FramePass& RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs) override;

			ImGuiPipelinePass& operator=(const ImGuiPipelinePass&) = delete;
			ImGuiPipelinePass& operator=(ImGuiPipelinePass&&) = delete;

			static void RegisterPass(FramePipelinePassRegistry& pipelinePassRegistry, ImGuiPlugin& imguiPlugin, ImGuiContext* context);

		private:
			std::string m_passName;
			AbstractViewer* m_viewer;
			FramePipeline& m_pipeline;
			ImGuiContext* m_imguiContext;
			ImGuiPlugin& m_imguiPlugin;
	};
}

#include <Nazara/Graphics/ImGuiPipelinePass.inl>

#endif // NAZARA_GRAPHICS_IMGUIPIPELINEPASS_HPP
