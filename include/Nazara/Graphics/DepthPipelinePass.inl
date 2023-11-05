// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline DepthPipelinePass::DepthPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters) :
	DepthPipelinePass(passData, std::move(passName), GetMaterialPassIndex(parameters))
	{
	}

	inline DepthPipelinePass::DepthPipelinePass(PassData& passData, std::string passName, std::size_t materialPassIndex) :
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

	inline void DepthPipelinePass::InvalidateCommandBuffers()
	{
		m_rebuildCommandBuffer = true;
	}

	inline void DepthPipelinePass::InvalidateElements()
	{
		m_rebuildElements = true;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
