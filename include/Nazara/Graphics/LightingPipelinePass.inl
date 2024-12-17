// Copyright (C) 2024 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline LightingPipelinePass::~LightingPipelinePass()
	{
		// Destroy shader bindings before releasing the pools
		m_directionalLights.clear();
		m_pointLights.clear();
		m_spotLights.clear();
		m_lightBufferPool.reset();
		m_commonShaderBinding.reset();
	}

	inline void LightingPipelinePass::InvalidateCommandBuffers()
	{
		m_rebuildCommandBuffer = true;
	}

	inline void LightingPipelinePass::InvalidateLights()
	{
		m_rebuildLights = true;
	}
}
