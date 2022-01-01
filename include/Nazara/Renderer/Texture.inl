// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline unsigned int Texture::GetLevelSize(unsigned int size, unsigned int level)
	{
		if (size == 0) // Possible dans le cas d'une image invalide
			return 0;

		return std::max(size >> level, 1U);
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
