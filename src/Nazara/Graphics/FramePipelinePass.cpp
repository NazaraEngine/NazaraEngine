// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <Nazara/Graphics/Debug.hpp>

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
