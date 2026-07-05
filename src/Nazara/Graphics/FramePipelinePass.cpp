// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/FramePipelinePass.hpp>

namespace Nz
{
	FramePipelinePass::~FramePipelinePass() = default;

	void FramePipelinePass::ClearRenderables()
	{
	}

	void FramePipelinePass::InvalidateElements()
	{
	}

	void FramePipelinePass::RegisterMaterialInstance(const MaterialInstance& /*materialInstance*/)
	{
	}

	void FramePipelinePass::RegisterRenderable(std::size_t /*renderableIndex*/, UInt32 /*instanceIndex*/, const InstancedRenderable& /*instancedRenderable*/, const SkeletonInstance* /*skeletonInstance*/, UInt32 /*renderMask*/, const Recti& /*scissorBox*/)
	{
	}

	void FramePipelinePass::UnregisterMaterialInstance(const MaterialInstance& /*materialInstance*/)
	{
	}

	void FramePipelinePass::UnregisterRenderable(std::size_t /*renderableIndex*/)
	{
	}
}
