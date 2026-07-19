// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Material.hpp>

namespace Nz
{
	ElementRenderer::~ElementRenderer() = default;

	void ElementRenderer::Prepare(const RenderData& /*renderData*/, const SceneData& /*sceneData*/, const AbstractViewer& /*viewer*/, ElementRendererData& /*rendererData*/, GpuResources& /*renderResources*/, std::size_t /*elementCount*/, const Pointer<const RenderElement>* /*elements*/)
	{
	}

	void ElementRenderer::PrepareEnd(ElementRendererData& /*rendererData*/, GpuResources& /*renderResources*/, GpuCommandBufferBuilder& /*commandBuffer*/)
	{
	}

	void ElementRenderer::Reset(ElementRendererData& /*rendererData*/, GpuResources& /*renderResources*/)
	{
	}

	ElementRendererData::~ElementRendererData() = default;
}
