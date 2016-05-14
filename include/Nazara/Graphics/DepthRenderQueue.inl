// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	bool DepthRenderQueue::IsMaterialSuitable(const Material* material) const
	{
		NazaraAssert(material, "Invalid material");

		return material->HasDepthMaterial() || (material->IsEnabled(RendererParameter_DepthBuffer) && material->IsEnabled(RendererParameter_DepthWrite) && material->IsShadowCastingEnabled());
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
