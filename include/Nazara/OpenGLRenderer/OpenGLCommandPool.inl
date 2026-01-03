// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <stdexcept>

namespace Nz
{
	inline void OpenGLCommandPool::TryToShrink()
	{
		std::size_t poolCount = m_commandPools.size();
		if (poolCount >= 2 && m_commandPools.back().freeCommands.TestAll())
		{
			for (std::size_t i = poolCount - 1; i > 0; --i)
			{
				if (!m_commandPools[i].freeCommands.TestAll())
					break;

				poolCount--;
			}

			m_commandPools.resize(poolCount);
		}
	}
}
