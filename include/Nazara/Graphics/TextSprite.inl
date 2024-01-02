// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline void TextSprite::Clear()
	{
		m_atlases.clear();
		m_vertices.clear();
		OnElementInvalidated(this);
	}

	inline void TextSprite::SetMaterial(std::shared_ptr<MaterialInstance> material)
	{
		assert(material);

		if (m_material != material)
		{
			OnMaterialInvalidated(this, 0, material);
			m_material = std::move(material);

			OnElementInvalidated(this);
		}
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
