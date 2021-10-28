// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/TextSprite.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline void TextSprite::Clear()
	{
		m_atlases.clear();
		m_vertices.clear();
	}

	inline void TextSprite::SetMaterial(std::shared_ptr<Material> material)
	{
		m_material = std::move(material);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
