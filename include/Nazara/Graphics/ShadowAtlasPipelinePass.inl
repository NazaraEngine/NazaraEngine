// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline ShadowAtlasPipelinePass::ShadowAtlasPipelinePass(PassData& passData, const ParameterList& parameters) :
	ShadowAtlasPipelinePass(passData, GetRenderQueues(parameters))
	{
	}

	inline ShadowAtlas& ShadowAtlasPipelinePass::GetAtlas()
	{
		return m_shadowAtlas;
	}

	inline const ShadowAtlas& ShadowAtlasPipelinePass::GetAtlas() const
	{
		return m_shadowAtlas;
	}
}
