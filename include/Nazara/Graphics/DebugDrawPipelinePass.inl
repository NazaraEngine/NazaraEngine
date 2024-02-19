// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline DebugDrawPipelinePass::DebugDrawPipelinePass(PassData& passData, std::string passName, const ParameterList& /*parameters*/) :
	FramePipelinePass({}),
	m_passName(std::move(passName)),
	m_viewer(passData.viewer),
	m_pipeline(passData.pipeline)
	{
	}
}

