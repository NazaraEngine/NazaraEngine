// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline ImGuiPipelinePass::ImGuiPipelinePass(PassData& passData, std::string passName, ImGuiPlugin& imguiPlugin, ImGuiContext* context, const ParameterList& /*parameters*/) :
	FramePipelinePass({}),
	m_passName(std::move(passName)),
	m_viewer(passData.viewer),
	m_pipeline(passData.pipeline),
	m_imguiContext(context),
	m_imguiPlugin(imguiPlugin)
	{
	}
}
