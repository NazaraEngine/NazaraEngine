// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline DebugDrawer& FramePipeline::GetDebugDrawer()
	{
		return m_debugDrawer;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
