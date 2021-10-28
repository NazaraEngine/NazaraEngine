// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ElementRenderer::~ElementRenderer() = default;

	void ElementRenderer::Prepare(const ViewerInstance& /*viewerInstance*/, ElementRendererData& /*rendererData*/, RenderFrame& /*currentFrame*/, const Pointer<const RenderElement>* /*elements*/, std::size_t /*elementCount*/)
	{
	}

	void ElementRenderer::Reset(ElementRendererData& /*rendererData*/, RenderFrame& /*currentFrame*/)
	{
	}

	ElementRendererData::~ElementRendererData() = default;
}
