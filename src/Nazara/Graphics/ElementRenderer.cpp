// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ElementRenderer::~ElementRenderer() = default;

	void ElementRenderer::Prepare(const ViewerInstance& /*viewerInstance*/, ElementRendererData& /*rendererData*/, RenderResources& /*renderResources*/, std::size_t /*elementCount*/, const Pointer<const RenderElement>* /*elements*/, SparsePtr<const RenderStates> /*renderStates*/)
	{
	}

	void ElementRenderer::PrepareEnd(RenderResources& /*renderResources*/, ElementRendererData& /*rendererData*/)
	{
	}

	void ElementRenderer::Reset(ElementRendererData& /*rendererData*/, RenderResources& /*renderResources*/)
	{
	}

	ElementRendererData::~ElementRendererData() = default;
}
