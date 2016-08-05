// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{

	/*!
	* \brief Checks whether the material is suitable to fit in the render queue
	* \return true If it is the case
	*
	* \param material Material to verify
	*/

	bool DepthRenderQueue::IsMaterialSuitable(const Material* material) const
	{
		NazaraAssert(material, "Invalid material");

		return material->HasDepthMaterial() || (material->IsDepthBufferEnabled() && material->IsDepthWriteEnabled() && material->IsShadowCastingEnabled());
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
