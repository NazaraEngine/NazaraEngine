// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/RenderElementOwner.hpp>
#include <Nazara/Graphics/RenderElementPool.hpp>

namespace Nz
{
	RenderElementOwner::~RenderElementOwner()
	{
		if (m_pool)
			m_pool->Free(m_poolIndex);
	}
}
