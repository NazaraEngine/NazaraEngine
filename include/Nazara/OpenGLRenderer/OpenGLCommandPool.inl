// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLCommandPool.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

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

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
