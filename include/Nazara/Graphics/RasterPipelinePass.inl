// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline RasterPipelinePass::RasterPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters) :
	RasterPipelinePass(passData, std::move(passName), GetMaterialPassIndex(parameters))
	{
	}

	inline RasterPipelinePass::RasterPipelinePass(PassData& passData, std::string passName, std::size_t materialPassIndex) :
	FramePipelinePass(FramePipelineNotification::ElementInvalidation | FramePipelineNotification::MaterialInstanceRegistration),
	m_passIndex(materialPassIndex),
	m_lastVisibilityHash(0),
	m_passName(std::move(passName)),
	m_viewer(passData.viewer),
	m_elementRegistry(passData.elementRegistry),
	m_pipeline(passData.pipeline),
	m_rebuildCommandBuffer(false),
	m_rebuildElements(false)
	{
	}

	inline void RasterPipelinePass::InvalidateCommandBuffers()
	{
		m_rebuildCommandBuffer = true;
	}

	inline void RasterPipelinePass::InvalidateElements()
	{
		m_rebuildElements = true;
	}
}

