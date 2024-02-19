// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/FramePipelinePass.hpp>

namespace Nz
{
	FramePipelinePass::~FramePipelinePass() = default;

	void FramePipelinePass::InvalidateElements()
	{
	}

	void FramePipelinePass::RegisterMaterialInstance(const MaterialInstance& /*materialInstance*/)
	{
	}

	void FramePipelinePass::UnregisterMaterialInstance(const MaterialInstance& /*materialInstance*/)
	{
	}
}
