// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ErrorFlags.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const TextureRef& PhongLightingMaterial::GetAlphaMap() const
	{
		return m_material->GetTexture(m_textureIndexes.alpha);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
