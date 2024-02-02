// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline unsigned int TaskScheduler::GetWorkerCount() const
	{
		return m_workerCount;
	}
}

#include <Nazara/Core/DebugOff.hpp>
