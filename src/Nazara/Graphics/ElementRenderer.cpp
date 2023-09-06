// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ElementRenderer::~ElementRenderer() = default;

	void ElementRenderer::Prepare(const ViewerInstance& /*viewerInstance*/, ElementRendererData& /*rendererData*/, RenderFrame& /*currentFrame*/, std::size_t /*elementCount*/, const Pointer<const RenderElement>* /*elements*/, SparsePtr<const RenderStates> /*renderStates*/)
	{
	}

	void ElementRenderer::PrepareEnd(RenderFrame& /*currentFrame*/, ElementRendererData& /*rendererData*/)
	{
	}

	void ElementRenderer::Reset(ElementRendererData& /*rendererData*/, RenderFrame& /*currentFrame*/)
	{
	}

	ElementRendererData::~ElementRendererData() = default;
}
